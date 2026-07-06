#include "VignettePlayer.h"

AVignettePlayer::AVignettePlayer()
{
	PrimaryActorTick.bCanEverTick = true;
	bIsPlaying = false;
	PlaybackTime = 0.0f;
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

void AVignettePlayer::PlayVignette(FName ChapterID)
{
	bIsPlaying = true;
	PlaybackTime = 0.0f;
	
	// Load level sequence and play it
}

void AVignettePlayer::SkipVignette()
{
	if (bIsPlaying && PlaybackTime >= SkippableThreshold)
	{
		bIsPlaying = false;
		OnVignetteFinished.Broadcast();
	}
}
