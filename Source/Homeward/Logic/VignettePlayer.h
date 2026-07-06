#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VignettePlayer.generated.h"

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

	UFUNCTION(BlueprintCallable, Category = "Vignette")
	void PlayVignette(FName ChapterID);

	UFUNCTION(BlueprintCallable, Category = "Vignette")
	void SkipVignette();

	// Fired when the vignette finishes playing or is skipped
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVignetteFinished);
	
	UPROPERTY(BlueprintAssignable, Category = "Vignette")
	FOnVignetteFinished OnVignetteFinished;

private:
	bool bIsPlaying;
	float PlaybackTime;
	const float SkippableThreshold = 3.0f;
};
