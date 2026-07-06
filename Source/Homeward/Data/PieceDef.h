#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PieceDef.generated.h"

UCLASS(BlueprintType)
class HOMEWARD_API UPieceDef : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Piece")
	FName PieceID;

	// The relative 3D positions of the cells making up this piece.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Piece")
	TArray<FIntVector> Cells;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Piece")
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Piece")
	int32 PaletteRow;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flags")
	bool bIsFragile;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flags")
	bool bIsHeavy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flags")
	bool bIsKeepsake;
};
