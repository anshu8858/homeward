#include "DynamicDifficultyManager.h"

void UDynamicDifficultyManager::OnLevelStart()
{
	CurrentHintThreshold = BaseHintThreshold;
}

void UDynamicDifficultyManager::UpdateDDA(float TimeInLevel)
{
	// Honest DDA: never rigged, just nudges hint timing earlier the longer a
	// player goes without solving the level -- 1 second off the threshold
	// for every 10 seconds spent in the level, floored at MinHintThreshold.
	const float Decay = TimeInLevel / 10.0f;
	CurrentHintThreshold = FMath::Max(BaseHintThreshold - Decay, MinHintThreshold);
}

bool UDynamicDifficultyManager::ShouldShowHintShimmer(float CurrentTimeInLevel) const
{
	return CurrentTimeInLevel >= CurrentHintThreshold;
}

bool UDynamicDifficultyManager::ShouldUseGentleVariant(int32 PreviousLevelAttempts, float PreviousLevelTime, float PreviousLevelParSeconds) const
{
	// Gentle variant is selected if the player struggled significantly on
	// the previous level: multiple restarts, or taking 3x the level's own
	// par time (not a fixed constant -- a 20s level and a 90s level don't
	// share a struggle threshold).
	const bool bManyAttempts = PreviousLevelAttempts > 3;
	const bool bFarOverPar = PreviousLevelParSeconds > 0.0f && PreviousLevelTime > PreviousLevelParSeconds * 3.0f;
	return bManyAttempts || bFarOverPar;
}
