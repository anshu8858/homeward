#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SettingsManager.generated.h"

UCLASS()
class HOMEWARD_API USettingsManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetMusicVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetSFXVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetHapticsEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetGentleHintsEnabled(bool bEnabled);

	UFUNCTION(BlueprintPure, Category = "Settings")
	float GetMusicVolume() const { return MusicVolume; }

	UFUNCTION(BlueprintPure, Category = "Settings")
	float GetSFXVolume() const { return SFXVolume; }

	UFUNCTION(BlueprintPure, Category = "Settings")
	bool GetHapticsEnabled() const { return bHapticsEnabled; }

	UFUNCTION(BlueprintPure, Category = "Settings")
	bool GetGentleHintsEnabled() const { return bGentleHintsEnabled; }

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SaveSettings();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void LoadSettings();

private:
	float MusicVolume;
	float SFXVolume;
	bool bHapticsEnabled;
	bool bGentleHintsEnabled;
};
