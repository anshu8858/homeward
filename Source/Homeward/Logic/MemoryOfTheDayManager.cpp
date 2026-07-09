#include "MemoryOfTheDayManager.h"
#include "SaveManager.h"
#include "Engine/GameInstance.h"

void UMemoryOfTheDayManager::EvaluateStreak() const
{
	USaveManager* SaveManagerSubsystem = GetGameInstance()->GetSubsystem<USaveManager>();
	if (!SaveManagerSubsystem || !SaveManagerSubsystem->CurrentSave) return;

	UHomewardSaveGame* Save = SaveManagerSubsystem->CurrentSave;
	if (Save->DailyStreak <= 0) return; // nothing to break

	const FTimespan SinceLastCompletion = FDateTime::Now() - Save->LastDailyCompletionDate;
	if (SinceLastCompletion.GetDays() > StreakGraceDays)
	{
		Save->DailyStreak = 0;
	}
}

bool UMemoryOfTheDayManager::IsDailyChallengeAvailable() const
{
	EvaluateStreak();

	USaveManager* SaveManagerSubsystem = GetGameInstance()->GetSubsystem<USaveManager>();
	if (!SaveManagerSubsystem || !SaveManagerSubsystem->CurrentSave) return true;

	return FDateTime::Now().GetDate() != SaveManagerSubsystem->CurrentSave->LastDailyCompletionDate.GetDate();
}

FName UMemoryOfTheDayManager::GetTodayChallengeID() const
{
	// Pick one of the 30 pre-baked remixes based on the day of the year.
	const int32 DayOfYear = FDateTime::Now().GetDayOfYear();
	const int32 ChallengeIndex = DayOfYear % 30;
	return FName(*FString::Printf(TEXT("Daily_%d"), ChallengeIndex));
}

void UMemoryOfTheDayManager::RecordDailyCompletion()
{
	if (!IsDailyChallengeAvailable()) return; // already completed today

	USaveManager* SaveManagerSubsystem = GetGameInstance()->GetSubsystem<USaveManager>();
	if (!SaveManagerSubsystem || !SaveManagerSubsystem->CurrentSave) return;

	UHomewardSaveGame* Save = SaveManagerSubsystem->CurrentSave;
	// IsDailyChallengeAvailable() above already ran EvaluateStreak(), so a
	// broken streak has already been zeroed -- this increment is correct
	// whether we're continuing a streak or starting a fresh one.
	Save->DailyStreak++;
	Save->LastDailyCompletionDate = FDateTime::Now();
	SaveManagerSubsystem->SaveGame();
}

int32 UMemoryOfTheDayManager::GetCurrentStreak() const
{
	EvaluateStreak();
	USaveManager* SaveManagerSubsystem = GetGameInstance()->GetSubsystem<USaveManager>();
	return (SaveManagerSubsystem && SaveManagerSubsystem->CurrentSave) ? SaveManagerSubsystem->CurrentSave->DailyStreak : 0;
}
