#pragma once
//#include "Engine.h"
#include "Modules/ModuleManager.h"

class FTWITCHINTERACTIONModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};