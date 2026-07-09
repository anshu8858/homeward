#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VignettePlayer.generated.h"

class UChapterDef;

UCLASS()
class HOMEWARD_API AVignettePlayer : public AActor
{
	GENERATED_BODY()

public:
	AVignettePlayer();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Signature matches AHomewardGameMode::FOnChapterCompleted so this can
	// be bound directly to that delegate (in Blueprint, or via
	// AddDynamic in C++) without any glue in between. The real
	// LevelSequence playback is Editor-authored content this class can
	// only hold a reference to, not create -- see the .cpp for what's
	// still a placeholder.
	UFUNCTION(BlueprintCallable, Category = "Vignette")
	void PlayVignette(UChapterDef* ChapterDef);

	UFUNCTION(BlueprintCallable, Category = "Vignette")
	void SkipVignette();

	UFUNCTION(BlueprintPure, Category = "Vignette")
	UChapterDef* GetCurrentChapter() const { return CurrentChapterDef; }

	// Fired when the vignette finishes playing or is skipped
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVignetteFinished);

	UPROPERTY(BlueprintAssignable, Category = "Vignette")
	FOnVignetteFinished OnVignetteFinished;

private:
	UPROPERTY()
	UChapterDef* CurrentChapterDef;

	bool bIsPlaying;
	float PlaybackTime;
	const float SkippableThreshold = 3.0f;
};
