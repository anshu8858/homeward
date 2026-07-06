#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "HomewardSaveGame.generated.h"

UCLASS()
class HOMEWARD_API UHomewardSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UHomewardSaveGame();

	UPROPERTY(VisibleAnywhere, Category = "Save Data")
	int32 SchemaVersion;

	UPROPERTY(VisibleAnywhere, Category = "Save Data")
	TMap<FString, int32> LevelScores;

	UPROPERTY(VisibleAnywhere, Category = "Save Data")
	TArray<FString> UnlockedKeepsakes;
};
