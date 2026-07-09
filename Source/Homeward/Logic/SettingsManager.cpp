#include "SettingsManager.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/Paths.h"

namespace
{
	const TCHAR* SettingsSection = TEXT("Homeward.Settings");

	FString GetSettingsIniPath()
	{
		return FPaths::ProjectSavedDir() / TEXT("HomewardSettings.ini");
	}
}

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
	const FString IniPath = GetSettingsIniPath();
	GConfig->SetFloat(SettingsSection, TEXT("MusicVolume"), MusicVolume, IniPath);
	GConfig->SetFloat(SettingsSection, TEXT("SFXVolume"), SFXVolume, IniPath);
	GConfig->SetBool(SettingsSection, TEXT("HapticsEnabled"), bHapticsEnabled, IniPath);
	GConfig->SetBool(SettingsSection, TEXT("GentleHintsEnabled"), bGentleHintsEnabled, IniPath);
	GConfig->Flush(false, IniPath);
}

void USettingsManager::LoadSettings()
{
	// Defaults, overwritten below if a saved value exists.
	MusicVolume = 1.0f;
	SFXVolume = 1.0f;
	bHapticsEnabled = true;
	bGentleHintsEnabled = true;

	const FString IniPath = GetSettingsIniPath();
	GConfig->GetFloat(SettingsSection, TEXT("MusicVolume"), MusicVolume, IniPath);
	GConfig->GetFloat(SettingsSection, TEXT("SFXVolume"), SFXVolume, IniPath);
	GConfig->GetBool(SettingsSection, TEXT("HapticsEnabled"), bHapticsEnabled, IniPath);
	GConfig->GetBool(SettingsSection, TEXT("GentleHintsEnabled"), bGentleHintsEnabled, IniPath);
}
