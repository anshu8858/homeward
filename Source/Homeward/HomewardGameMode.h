#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HomewardGameMode.generated.h"

class APackingBox;
class ULevelDef;
class UChapterDef;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGentleVariantSuggested, bool, bUseGentleVariant);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChapterCompleted, UChapterDef*, CompletedChapter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelAdvanceRequested, int32, NextLevelIndex);

// Owns the game-loop glue between grid logic (APackingBox/FGridModel),
// player-facing systems (DDA, Memory-of-the-Day), and level flow. This
// class decides WHAT should happen next (advance to level N, play chapter
// X's vignette, use the gentle variant); loading the actual next map/
// sequence in response to its delegates is Blueprint/editor glue, since
// that's bound to content assets this class has no business owning.
UCLASS()
class HOMEWARD_API AHomewardGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHomewardGameMode();

	// Called by the level's APackingBox in its own BeginPlay, so the
	// GameMode always knows which box is active without a separate
	// level-Blueprint wiring step.
	UFUNCTION(BlueprintCallable, Category = "Game")
	void RegisterActivePackingBox(APackingBox* Box);

	// Begins tracking a level: resets the DDA hint timer, records the
	// start time used to compute this attempt's elapsed time, and
	// (re)initializes the active packing box from LevelDef.
	UFUNCTION(BlueprintCallable, Category = "Game")
	void StartLevel(ULevelDef* LevelDef, UChapterDef* ChapterDef);

	// Called when a level is successfully packed. Evaluates DDA's
	// gentle-variant suggestion for the next attempt, advances to the
	// next level or fires the chapter-complete vignette hook, and records
	// completion with the daily-challenge manager if this was today's.
	UFUNCTION(BlueprintCallable, Category = "Game")
	void OnLevelCompleted();

	// Polled by UI (e.g. a UMG tick binding) to drive the honest-DDA hint
	// shimmer. Also nudges the DDA threshold's time-based decay, so
	// nothing else needs to tick just to keep it current.
	UFUNCTION(BlueprintCallable, Category = "Game")
	bool ShouldShowHintShimmer();

	UPROPERTY(BlueprintAssignable, Category = "Game")
	FOnGentleVariantSuggested OnGentleVariantSuggested;

	UPROPERTY(BlueprintAssignable, Category = "Game")
	FOnChapterCompleted OnChapterCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Game")
	FOnLevelAdvanceRequested OnLevelAdvanceRequested;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TWeakObjectPtr<APackingBox> ActivePackingBox;

	UPROPERTY()
	ULevelDef* CurrentLevelDef;

	UPROPERTY()
	UChapterDef* CurrentChapterDef;

	float LevelStartTime;

	// Counts consecutive attempts at the *same* LevelDef (StartLevel bumps
	// this when called again for a level that didn't change, resets to 1
	// otherwise) -- fed into ShouldUseGentleVariant.
	int32 CurrentLevelAttempts;
};
