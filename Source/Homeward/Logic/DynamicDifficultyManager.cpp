#include "DynamicDifficultyManager.h"

void UDynamicDifficultyManager::OnLevelStart()
{
	CurrentHintThreshold = BaseHintThreshold;
}

void UDynamicDifficultyManager::UpdateDDA(float TimeInLevel)
{
	// Gradually reduce the hint threshold as the player spends more time in the level without progress
	// Honest DDA: never rigged, just adjusts hint timing
	
	// Example: reduce threshold by 1 second for every 10 seconds of inactivity
	// (Actual implementation would track time since last successful move)
}

bool UDynamicDifficultyManager::ShouldShowHintShimmer(float CurrentTimeInLevel) const
{
	return CurrentTimeInLevel >= CurrentHintThreshold;
}

bool UDynamicDifficultyManager::ShouldUseGentleVariant(int32 PreviousLevelAttempts, float PreviousLevelTime) const
{
	// Gentle level variant is selected if the player struggled significantly on the previous level
	// e.g., Multiple restarts or taking 3x the par time.
	return PreviousLevelAttempts > 3 || PreviousLevelTime > 120.0f; // placeholder logic
}
