#pragma once
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

class RustGameModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

