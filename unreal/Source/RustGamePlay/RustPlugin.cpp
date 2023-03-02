// Copyright Epic Games, Inc. All Rights Reserved.

#include "RustPlugin.h"
#include "CoreMinimal.h"
#if WITH_EDITOR
#include "Editor/EditorEngine.h"
#include "LevelEditor.h"
#endif
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "DirectoryWatcherModule.h"
#include "IDirectoryWatcher.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "Framework/Notifications/NotificationManager.h"
#include "HAL/PlatformFileManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include <stdint.h>
#include "RustGameMode.h"
#include "Binder.h"
static const FName RustPluginTabName("ProjectU");

#define LOCTEXT_NAMESPACE "FRustPluginModule"
FString PlatformExtensionName()
{
#if PLATFORM_LINUX || PLATFORM_MAC
	return FString(TEXT("so"));
#elif PLATFORM_WINDOWS
        return FString(TEXT("dll"));
#endif
}

UnrealBindings CreateBindings()
{
	SoundFns sound_fns;
	sound_fns.play_sound_at_location = PlaySoundAtLocation;

	EditorComponentFns editor_component_fns;
	editor_component_fns.get_editor_component_bool = &GetEditorComponentBool;
	editor_component_fns.get_editor_component_float = &GetEditorComponentFloat;
	editor_component_fns.get_editor_component_quat = &GetEditorComponentQuat;
	editor_component_fns.get_editor_component_vector = &GetEditorComponentVector;
	editor_component_fns.get_editor_component_uobject = &GetEditorComponentUObject;
	editor_component_fns.get_editor_components = &GetEditorComponentUuids;

	PhysicsFns physics_fns = {};
	physics_fns.add_force = &AddForce;
	physics_fns.add_impulse = &AddImpulse;
	physics_fns.set_velocity = &SetVelocity;
	physics_fns.get_velocity = &GetVelocity;
	physics_fns.is_simulating = &IsSimulating;
	physics_fns.line_trace = &LineTrace;
	physics_fns.get_bounding_box_extent = &GetBoundingBoxExtent;
	physics_fns.sweep = &Sweep;
	physics_fns.sweep_multi = &SweepMulti;
	physics_fns.get_collision_shape = &GetCollisionShape;

	ActorFns actor_fns = {};
	actor_fns.get_spatial_data = &GetSpatialData;
	actor_fns.set_spatial_data = &SetSpatialData;
	//actor_fns.set_entity_for_actor = &SetEntityForActor;
	actor_fns.set_view_target = &SetViewTarget;
	actor_fns.get_actor_components = &GetActorComponents;
	actor_fns.get_registered_classes = &GetRegisteredClasses;
	actor_fns.get_class = &GetClass;
	/*actor_fns.set_owner = &SetOwner;
	actor_fns.get_actor_name = &GetActorName;
	actor_fns.is_moveable = &IsMoveable;*/
	actor_fns.register_actor_on_overlap = &RegisterActorOnOverlap;
	actor_fns.register_actor_on_hit = &RegisterActorOnHit;
	actor_fns.get_root_component = &GetRootComponent;
	actor_fns.destroy_actor = &DestroyActor;
	UnrealBindings b = {};
	b.gameplay_fns = GameplayFns::Create();
	b.actor_fns = actor_fns;
	//b.sound_fns = sound_fns;
	b.physics_fns = physics_fns;
	b.editor_component_fns = editor_component_fns;
	b.log = &Log;
	b.iterate_actors = &IterateActors;
	b.get_action_state = &GetActionState;
	b.get_axis_value = &GetAxisValue;
	b.spawn_actor = &SpawnActor;
	b.get_mouse_delta = &GetMouseDelta;
	b.visual_log_segment = &VisualLogSegment;
	b.visual_log_capsule = &VisualLogCapsule;
	b.visual_log_location = &VisualLogLocation;
	b.spawn_actor_bp = &SpawnActorBP;
	return b;
}
FString FPlugin::PluginFolderPath()
{
	return FPaths::Combine(*FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()), TEXT("Binaries"));
}

FString FPlugin::PluginPath()
{
	return FPaths::Combine(*PluginFolderPath(), *PluginFileName());
}

FString FPlugin::PluginFileName()
{
	return FString::Printf(TEXT("%s.%s"), TEXT(RUST_DLL_NAME), *PlatformExtensionName());
}

FPlugin::FPlugin(): Rust(), Bindings(nullptr), NeedsInit(false), Handle(nullptr)
{
}
FPlugin::~FPlugin()
{
	Unload();
	Handle = nullptr;
}

void FPlugin::Unload()
{
	if (this->IsLoaded())
	{
		//destroy dll if nessary
		if (const auto DLL_Destroy = static_cast<void (*)()>(this->GetDllExport(TEXT("on_dll_destroy\0")))) {
			DLL_Destroy();
		}
		FPlatformProcess::FreeDllHandle(this->Handle);
		this->Handle = nullptr;
		// This is leaky. If we close the editor this will not delete the file
		// TODO: Cleanup unused dlls here

#if WITH_EDITOR
		if (!FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*this->TargetPath))
		{
			UE_LOG(LogNative, Warning, TEXT("Unable to delete File %s"), *this->TargetPath);
		}
#endif
	}
}

void* FPlugin::GetDllExport(FString apiName){
	return this->IsLoaded() ?  
	FPlatformProcess::GetDllExport(Handle, *apiName):
	nullptr;
}
bool FPlugin::TryLoad()
{
    // Loading ddls is a bit tricky, see https://fasterthanli.me/articles/so-you-want-to-live-reload-rust
    // The gist is we can't easily hot reload a dll if the dll uses the thread local storage (TLS).
    // The TLS will prevent the dll from being unloaded even when we call `dlclose`. And `dlopen` will return
    // the pointer to the previously loaded dll.
    // Essentially this means hot reloading will do nothing as we can't unload the currently loaded dll.
    // The workaround for this is give each dll a unique name. Here we append the unix timestamp at
    // the end of the file. That way we can force `dlopen` to load the dll.
    // Please note this is a hack, and this _should_ leak and increase the memory every time you hot reload.
    // This behavior is the same on Linux, Windows and most likely all the other platforms.

    const FString Path = PluginPath();
#if WITH_EDITOR
	FString LocalTargetPath = FPaths::Combine(*PluginFolderPath(),
	                                          *FString::Printf(
		                                          TEXT("%s-%i"), *PluginFileName(),
		                                          FDateTime::Now().ToUnixTimestamp()));
#else
	FString LocalTargetPath = PluginFolderPath();
#endif
	Unload();

#if WITH_EDITOR
	if (!FPlatformFileManager::Get().GetPlatformFile().CopyFile(*LocalTargetPath, *Path))
	{
		UE_LOG(LogNative, Warning, TEXT("Unable to copy File from %s to %s"), *Path, *LocalTargetPath);
		return false;
	}
#endif

	void* LocalHandle = FPlatformProcess::GetDllHandle(*LocalTargetPath);

	if (LocalHandle == nullptr)
	{
		UE_LOG(LogNative, Warning, TEXT("Dll open failed"));
		return false;
	}

	this->Handle = LocalHandle;

	void* LocalBindings = FPlatformProcess::GetDllExport(LocalHandle, TEXT("RegisterInternalBinders\0"));
	ensure(LocalBindings);

	this->Bindings = static_cast<EntryUnrealBindingsFn>(LocalBindings);

	this->TargetPath = LocalTargetPath;
	NeedsInit = true;
	CallEntryPoints();
	UE_LOG(LogNative, Display, TEXT("gameplay.Dll open success"));
	return true;
}

bool FPlugin::IsLoaded()
{
	return Handle != nullptr;
}

void FPlugin::CallEntryPoints()
{
	if (!IsLoaded())
		return;
	register_all(this);
	// Pass unreal function pointers to Rust, and also retrieve function pointers from Rust so we can call into Rust
	if ((ResultCode)this->Bindings(CreateBindings(), &Rust) == ResultCode::Success)
	{
		if (Rust.init() == ResultCode::Success) {
			UE_LOG(LogNative, Display, TEXT(" bind native success"));
		}
		else {
			UE_LOG(LogNative, Error, TEXT("init native fail"));
		}
	}
	else
	{
		UE_LOG(LogNative, Error, TEXT(" bind native fail"));
		// TODO: We had a panic when calling the entry point. We need to handle that better, otherwise unreal will segfault because the rust bindings are nullptrs
	}
}

#undef LOCTEXT_NAMESPACE
