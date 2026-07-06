#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HomewardGameMode.generated.h"

UCLASS()
class HOMEWARD_API AHomewardGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHomewardGameMode();

	// Called when a level is successfully packed
	UFUNCTION(BlueprintCallable, Category = "Game")
	void OnLevelCompleted();

protected:
	virtual void BeginPlay() override;
};
