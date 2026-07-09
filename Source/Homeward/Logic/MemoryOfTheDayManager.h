#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MemoryOfTheDayManager.generated.h"

UCLASS()
class HOMEWARD_API UMemoryOfTheDayManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Daily")
	bool IsDailyChallengeAvailable() const;

	UFUNCTION(BlueprintCallable, Category = "Daily")
	FName GetTodayChallengeID() const;

	UFUNCTION(BlueprintCallable, Category = "Daily")
	void RecordDailyCompletion();

	UFUNCTION(BlueprintPure, Category = "Daily")
	int32 GetCurrentStreak() const;

private:
	// Pause-not-break: a streak only resets once more than this many days
	// have passed since the last completion.
	static constexpr int32 StreakGraceDays = 2;

	// Breaks the streak in the save data if the grace window has elapsed.
	// Called before every read/write below so the streak is never stale.
	// Streak state lives in UHomewardSaveGame (via USaveManager), not here,
	// so there is a single source of truth that actually gets persisted.
	void EvaluateStreak() const;
};
