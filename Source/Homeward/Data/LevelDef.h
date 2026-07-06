#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PieceDef.h"
#include "LevelDef.generated.h"

USTRUCT(BlueprintType)
struct FPieceRequirement
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPieceDef* PieceDef;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Count;
};

UCLASS(BlueprintType)
class HOMEWARD_API ULevelDef : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	FName LevelID;

	// Width, Depth, Layers (Height)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	FIntVector GridSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FPieceRequirement> Pieces;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	int32 ParSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	FName KeepsakeID;
};
