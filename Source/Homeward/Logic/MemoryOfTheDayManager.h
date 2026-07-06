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
	int32 GetCurrentStreak() const { return CurrentStreak; }

private:
	int32 CurrentStreak;
	FDateTime LastCompletionDate;
	
	// Called on initialization to check for broken streaks (pause-not-break allowed?)
	void EvaluateStreak();
};
