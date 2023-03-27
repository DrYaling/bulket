#include "RustGameModule.h"

#include "PropertyEditorModule.h"
#include "../Editor/AnimParamDetail.h"
#include "Modules/ModuleManager.h"
#include "../FFI/RustApi.h"

void RustGameModule::StartupModule()
{
	IModuleInterface::StartupModule();
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout("AnimParams", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FAnimParamDetail::MakeInstance));
	UE_LOG(LogNative, Warning, TEXT("StartupModule ProjectU"));
}

void RustGameModule::ShutdownModule()
{
	IModuleInterface::ShutdownModule();
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.UnregisterCustomPropertyTypeLayout("AnimParams");
}

IMPLEMENT_PRIMARY_GAME_MODULE(RustGameModule, RustPlugin, "ProjectU");