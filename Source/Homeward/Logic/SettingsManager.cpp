#include "SettingsManager.h"

void USettingsManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadSettings();
}

void USettingsManager::Deinitialize()
{
	SaveSettings();
	Super::Deinitialize();
}

void USettingsManager::SetMusicVolume(float Volume)
{
	MusicVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
	// Update audio mix
}

void USettingsManager::SetSFXVolume(float Volume)
{
	SFXVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
	// Update audio mix
}

void USettingsManager::SetHapticsEnabled(bool bEnabled)
{
	bHapticsEnabled = bEnabled;
}

void USettingsManager::SetGentleHintsEnabled(bool bEnabled)
{
	bGentleHintsEnabled = bEnabled;
}

void USettingsManager::SaveSettings()
{
	// Serialize to GConfig or SaveGame
}

void USettingsManager::LoadSettings()
{
	// Defaults
	MusicVolume = 1.0f;
	SFXVolume = 1.0f;
	bHapticsEnabled = true;
	bGentleHintsEnabled = true;
	
	// Load from GConfig or SaveGame
}
