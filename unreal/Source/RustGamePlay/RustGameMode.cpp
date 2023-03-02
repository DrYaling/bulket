// Fill out your copyright notice in the Description page of Project Settings.


#include "RustGameMode.h"
#include "EngineUtils.h"
#include "UObject/ConstructorHelpers.h"
#include "EnhancedInputComponent.h"
#include "RustPlugin.h"
#include "RustApi.h"
#include "RustCharacter.h"
#include <Kismet/GameplayStatics.h>
#include <IDirectoryWatcher.h>
#include <DirectoryWatcherModule.h>
#include "RustGameInstance.h"
#include "RustActor.h"
#include "RustPawn.h"
ARustGameMode::ARustGameMode()
{
	//set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	//if (PlayerPawnBPClass.Class != NULL)
	//{
	//	DefaultPawnClass = PlayerPawnBPClass.Class;
	//}
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
	bBlockInput = false;
}

void ARustGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void ARustGameMode::StartPlay()
{
	const auto GameInstance = sGameInstance;
	if (!GameInstance)
		return;
	GameInstance->SetGameMode(this);
	const auto Plugin = GameInstance->GetPlugin();
	if (nullptr == Plugin)
	{
		UE_LOG(LogNative, Error, TEXT("PluginNative Not Initialized"));
		return;
	}
	Super::StartPlay();
	GetWorld()->AddOnActorSpawnedHandler(
		FOnActorSpawned::FDelegate::CreateUObject(this, &ARustGameMode::OnActorSpawnedHandler));


	if (!Plugin->IsLoaded() && !Plugin->TryLoad()) {
		UE_LOG(LogNative, Error, TEXT("PluginNative Load Fail"));
		return;
	}
	if (Plugin->Rust.begin_play() == ResultCode::Panic)
	{
		UE_LOG(LogNative, Error, TEXT("PluginNative begin_play Fail"));
	}
	else
	{
		Plugin->NeedsInit = false;
		UE_LOG(LogNative, Display, TEXT("PluginNative begin_play Success"));
	}
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AActor* Actor = *ActorItr;
		Actor->OnDestroyed.AddUniqueDynamic(this, &ARustGameMode::OnActorDestroyed);
		OnActorSpawnedHandler(Actor);
	}
}
void ARustGameMode::OnActorSpawnedHandler(AActor* actor)
{
	constexpr EventType Type = EventType::ActorSpawned;
	ActorSpawnedEvent Event;
	Event.unitState = nullptr;
	Event.actor = static_cast<AActorOpaque*>(actor);
	auto SGame = sGameInstance;
	if(const auto RustObject = Cast<IRustObjectInterface>(actor))
	{
		SGame->OnGameObjectSpawned(RustObject);
		auto UnitState = RustObject->GetUnitState();
		Event.unitState = &UnitState;
	}
	const auto Plugin = SGame->GetPlugin();
	Plugin->Rust.unreal_event(&Type, static_cast<void*>(&Event));
}
//
void ARustGameMode::Tick(float Dt)
{
	Super::Tick(Dt);
	auto Plugin = sGameInstance->GetPlugin();
	if (Plugin->NeedsInit)
	{
		StartPlay();
	}
	if (Plugin->IsLoaded() && Plugin->Rust.tick(Dt) == ResultCode::Panic)
	{
		Plugin->Unload();
	}
}

void ARustGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	auto Plugin = sGameInstance->GetPlugin();
	if (Plugin->IsLoaded() && Plugin->Rust.end_play(static_cast<REndPlayReason>(EndPlayReason)) == ResultCode::Panic)
	{
		Plugin->Unload();
	}
}

void ARustGameMode::OnReplicationPausedChanged(bool bIsReplicationPaused)
{
	Super::OnReplicationPausedChanged(bIsReplicationPaused);
}

void ARustGameMode::BeginDestroy()
{
	Super::BeginDestroy();
}

void ARustGameMode::OnActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	constexpr EventType Type = EventType::ActorBeginOverlap;
	ActorBeginOverlap Event;
	Event.overlapped_actor = static_cast<AActorOpaque*>(OverlappedActor);
	Event.other = static_cast<AActorOpaque*>(OtherActor);
	const auto Plugin = sGameInstance->GetPlugin();
	Plugin->Rust.unreal_event(&Type, static_cast<void*>(&Event));
}

void ARustGameMode::OnActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	constexpr EventType Type = EventType::ActorEndOverlap;
	ActorEndOverlap Event;
	Event.overlapped_actor = static_cast<AActorOpaque*>(OverlappedActor);
	Event.other = static_cast<AActorOpaque*>(OtherActor);
	const auto Plugin = sGameInstance->GetPlugin();
	Plugin->Rust.unreal_event(&Type, static_cast<void*>(&Event));
}

void ARustGameMode::OnActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	constexpr EventType Type = EventType::ActorOnHit;
	ActorHitEvent Event;
	Event.self_actor = static_cast<AActorOpaque*>(SelfActor);
	Event.other = static_cast<AActorOpaque*>(OtherActor);
	Event.normal_impulse = ToVector3(NormalImpulse);
	const auto Plugin = sGameInstance->GetPlugin();
	Plugin->Rust.unreal_event(&Type, static_cast<void*>(&Event));
}

void ARustGameMode::OnActorDestroyed(AActor* Actor)
{
	constexpr EventType Type = EventType::ActorDestroy;
	ActorDestroyEvent Event;
	auto SGame = sGameInstance;
	Event.actor = static_cast<AActorOpaque*>(Actor);
	const auto RustObject = Cast<IRustObjectInterface>(Actor);
	if (RustObject)
	{
		auto UnitState = RustObject->GetUnitState();
		Event.unitState = &UnitState;
	}
	const auto Plugin = SGame->GetPlugin();
	Plugin->Rust.unreal_event(&Type, static_cast<void*>(&Event));
	if (RustObject) {
		SGame->OnGameObjectDestroyed(RustObject);
	}
}
