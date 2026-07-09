#include "HomewardGameMode.h"
#include "HomewardPlayerController.h"
#include "Logic/PackingBox.h"
#include "Logic/DynamicDifficultyManager.h"
#include "Logic/MemoryOfTheDayManager.h"
#include "Data/LevelDef.h"
#include "Data/ChapterDef.h"
#include "Engine/GameInstance.h"

AHomewardGameMode::AHomewardGameMode()
{
	PlayerControllerClass = AHomewardPlayerController::StaticClass();
	CurrentLevelDef = nullptr;
	CurrentChapterDef = nullptr;
	LevelStartTime = 0.0f;
	CurrentLevelAttempts = 0;
}

void AHomewardGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AHomewardGameMode::RegisterActivePackingBox(APackingBox* Box)
{
	ActivePackingBox = Box;
}

void AHomewardGameMode::StartLevel(ULevelDef* LevelDef, UChapterDef* ChapterDef)
{
	CurrentLevelAttempts = (LevelDef == CurrentLevelDef) ? CurrentLevelAttempts + 1 : 1;
	CurrentLevelDef = LevelDef;
	CurrentChapterDef = ChapterDef;
	LevelStartTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;

	if (UDynamicDifficultyManager* DDA = GetGameInstance()->GetSubsystem<UDynamicDifficultyManager>())
	{
		DDA->OnLevelStart();
	}

	if (ActivePackingBox.IsValid() && LevelDef)
	{
		ActivePackingBox->InitializeFromLevelDef(LevelDef);
	}
}

bool AHomewardGameMode::ShouldShowHintShimmer()
{
	UDynamicDifficultyManager* DDA = GetGameInstance()->GetSubsystem<UDynamicDifficultyManager>();
	if (!DDA || !GetWorld())
	{
		return false;
	}

	const float TimeInLevel = GetWorld()->GetTimeSeconds() - LevelStartTime;
	DDA->UpdateDDA(TimeInLevel);
	return DDA->ShouldShowHintShimmer(TimeInLevel);
}

void AHomewardGameMode::OnLevelCompleted()
{
	const float LevelTime = GetWorld() ? GetWorld()->GetTimeSeconds() - LevelStartTime : 0.0f;

	if (UDynamicDifficultyManager* DDA = GetGameInstance()->GetSubsystem<UDynamicDifficultyManager>())
	{
		if (CurrentLevelDef)
		{
			const bool bGentle = DDA->ShouldUseGentleVariant(CurrentLevelAttempts, LevelTime, CurrentLevelDef->ParSeconds);
			OnGentleVariantSuggested.Broadcast(bGentle);
		}
	}

	if (CurrentChapterDef && CurrentLevelDef)
	{
		const int32 LevelIndex = CurrentChapterDef->Levels.IndexOfByKey(CurrentLevelDef);
		const bool bIsLastLevelInChapter = LevelIndex != INDEX_NONE && LevelIndex == CurrentChapterDef->Levels.Num() - 1;

		if (bIsLastLevelInChapter)
		{
			OnChapterCompleted.Broadcast(CurrentChapterDef);
		}
		else if (LevelIndex != INDEX_NONE)
		{
			OnLevelAdvanceRequested.Broadcast(LevelIndex + 1);
		}
	}

	if (UMemoryOfTheDayManager* Daily = GetGameInstance()->GetSubsystem<UMemoryOfTheDayManager>())
	{
		if (CurrentLevelDef && CurrentLevelDef->LevelID == Daily->GetTodayChallengeID())
		{
			Daily->RecordDailyCompletion();
		}
	}
}
