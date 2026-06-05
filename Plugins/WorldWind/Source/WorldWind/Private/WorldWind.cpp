// Copyright Epic Games, Inc. All Rights Reserved.

#include "WorldWind.h"

#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "WorldWindSettings.h"

#define LOCTEXT_NAMESPACE "FWorldWindModule"

void FWorldWindModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module



	#if WITH_EDITORONLY_DATA
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings")) 
	{
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", "WorldWind",
			LOCTEXT("RuntimeGeneralName", "WorldWind Plugin"),
			LOCTEXT("RuntimeGeneralDescription", "General settings for the WorldWind Plugin"),
			GetMutableDefault<UWorldWindSettings>());
		
		/*if (SettingsSection.IsValid()) { 
			SettingsSection->OnModified().BindRaw(this, &FSimulationModule::HandleSettingsSaved); 
		}*/
	}
#endif
}

void FWorldWindModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FWorldWindModule, WorldWind)