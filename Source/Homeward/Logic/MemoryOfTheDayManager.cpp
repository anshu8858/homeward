#include "MemoryOfTheDayManager.h"

bool UMemoryOfTheDayManager::IsDailyChallengeAvailable() const
{
	FDateTime Today = FDateTime::Now();
	return Today.GetDate() != LastCompletionDate.GetDate();
}

FName UMemoryOfTheDayManager::GetTodayChallengeID() const
{
	// Logic to pick one of the 30 pre-baked remixes based on the day of the year
	int32 DayOfYear = FDateTime::Now().GetDayOfYear();
	int32 ChallengeIndex = DayOfYear % 30;
	return FName(*FString::Printf(TEXT("Daily_%d"), ChallengeIndex));
}

void UMemoryOfTheDayManager::RecordDailyCompletion()
{
	if (IsDailyChallengeAvailable())
	{
		CurrentStreak++;
		LastCompletionDate = FDateTime::Now();
		// Save to SaveGame
	}
}

void UMemoryOfTheDayManager::EvaluateStreak()
{
	// Check if the streak was broken
	FDateTime Today = FDateTime::Now();
	FTimespan Difference = Today - LastCompletionDate;
	
	// Pause-not-break: allow up to 48 hours before breaking the streak
	if (Difference.GetDays() > 2)
	{
		CurrentStreak = 0;
	}
}
