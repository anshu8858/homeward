#include "VignettePlayer.h"
#include "../Data/ChapterDef.h"

AVignettePlayer::AVignettePlayer()
{
	PrimaryActorTick.bCanEverTick = true;
	bIsPlaying = false;
	PlaybackTime = 0.0f;
	CurrentChapterDef = nullptr;
}

void AVignettePlayer::BeginPlay()
{
	Super::BeginPlay();
}

void AVignettePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsPlaying)
	{
		PlaybackTime += DeltaTime;
		
		// In a real implementation, we would evaluate a LevelSequence here.
		// For the scaffold, we just let it run.
	}
}

void AVignettePlayer::PlayVignette(UChapterDef* ChapterDef)
{
	CurrentChapterDef = ChapterDef;
	bIsPlaying = true;
	PlaybackTime = 0.0f;

	// TODO (Editor-gated): load and play ChapterDef's LevelSequence asset
	// once ChapterDef carries one (it doesn't yet -- see ChapterDef.h,
	// which only has PaletteTexture/BackgroundMusic today) and vignette
	// sequence assets exist to author. Until then this only tracks
	// playback state/timing so SkipVignette and OnVignetteFinished behave
	// correctly against a real timeline later.
}

void AVignettePlayer::SkipVignette()
{
	if (bIsPlaying && PlaybackTime >= SkippableThreshold)
	{
		bIsPlaying = false;
		OnVignetteFinished.Broadcast();
	}
}
