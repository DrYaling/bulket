// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "RustApi.h"
#include "Containers/Map.h"
#include "Containers/UnrealString.h"
class AGameModeBase;
class ARustGameMode;
class FString;

#define RUST_DLL_NAME "gameplay"
struct FRustReflection
{
	FString Name;
	uint32 NumberOfFields;
	bool IsEditorComponent;
	TMap<uint32, FString> IndexToFieldName;
	TMap<FString, ReflectionType> FieldNameToType;
};

struct FReflectionData
{
	TMap<FGuid, FRustReflection> Types;
};
class FPlugin: public Plugin {
public:
	FString TargetPath;
	RustBindings Rust;
	EntryUnrealBindingsFn Bindings;

	bool NeedsInit;
	bool IsLoaded();
	bool TryLoad();
	static FString PluginFolderPath();
	static FString PluginPath();
	static FString PluginFileName();

	FPlugin();
	virtual ~FPlugin() override;
	FPlugin(const FPlugin&) = delete;
	FPlugin(const FPlugin&&) = delete;
	FPlugin(const FPlugin*) = delete;
	void Unload();
    virtual void* GetDllExport(FString apiName) override;
	FReflectionData ReflectionData;
private:
	void CallEntryPoints();
	void* Handle;
};
FString PlatformExtensionName();
