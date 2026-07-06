#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelDef.h"
#include "ChapterDef.generated.h"

UCLASS(BlueprintType)
class HOMEWARD_API UChapterDef : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chapter")
	FName ChapterID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chapter")
	FString ChapterName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chapter")
	TArray<ULevelDef*> Levels;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Art")
	UTexture2D* PaletteTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundBase* BackgroundMusic;
};
