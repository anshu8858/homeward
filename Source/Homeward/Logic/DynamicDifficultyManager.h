#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DynamicDifficultyManager.generated.h"

UCLASS()
class HOMEWARD_API UDynamicDifficultyManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Call this when a level starts to reset DDA state
	UFUNCTION(BlueprintCallable, Category = "DDA")
	void OnLevelStart();

	// Call this periodically or when a move is made
	UFUNCTION(BlueprintCallable, Category = "DDA")
	void UpdateDDA(float TimeInLevel);

	// Determine if the "hint shimmer" should be shown based on DDA rules (45s -> 15s)
	UFUNCTION(BlueprintPure, Category = "DDA")
	bool ShouldShowHintShimmer(float CurrentTimeInLevel) const;

	// Suggests if the next level load should use the "gentle" variant.
	// PreviousLevelParSeconds comes from the LevelDef that was just played.
	UFUNCTION(BlueprintPure, Category = "DDA")
	bool ShouldUseGentleVariant(int32 PreviousLevelAttempts, float PreviousLevelTime, float PreviousLevelParSeconds) const;

private:
	float CurrentHintThreshold; // Modulated between 45.0f and 15.0f
	const float BaseHintThreshold = 45.0f;
	const float MinHintThreshold = 15.0f;
};
