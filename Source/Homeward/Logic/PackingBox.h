#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridModel.h"
#include "PackingBox.generated.h"

class ULevelDef;
class APiece;

// Owns the logical FGridModel for the level currently being packed, and
// mediates between grid-space (FIntVector cells) and world-space (the
// physical box placed in the level). DragController talks to this, never
// to FGridModel directly, so coordinate conversion lives in exactly one
// place.
UCLASS()
class HOMEWARD_API APackingBox : public AActor
{
	GENERATED_BODY()

public:
	APackingBox();

protected:
	virtual void BeginPlay() override;

public:
	// World-space position of grid cell (0,0,0)'s corner.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	FVector GridOriginWorld;

	// Edge length of one grid cell in world units.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	float CellSize;

	UFUNCTION(BlueprintCallable, Category = "Grid")
	void InitializeFromLevelDef(ULevelDef* LevelDef);

	// Converts a grid cell to the world-space position of its center.
	UFUNCTION(BlueprintCallable, Category = "Grid")
	FVector GridToWorld(const FIntVector& Cell) const;

	// Converts a world-space position to an (X,Y) grid column. Does not
	// resolve Z -- placement always targets the active layer, never
	// whatever layer a touch happened to project onto.
	UFUNCTION(BlueprintCallable, Category = "Grid")
	bool WorldToGridColumn(const FVector& WorldPos, int32& OutX, int32& OutY) const;

	// World-space Z of the horizontal plane a drag should project onto
	// right now -- the active layer's packing plane.
	UFUNCTION(BlueprintCallable, Category = "Grid")
	float GetActiveLayerWorldZ() const;

	UFUNCTION(BlueprintCallable, Category = "Grid")
	bool CanPlacePieceAt(APiece* Piece, int32 GridX, int32 GridY, int32 RotationState) const;

	// Places the piece both in the logical grid and in the world, and
	// records it so TryUndo can reverse exactly this placement.
	UFUNCTION(BlueprintCallable, Category = "Grid")
	bool TryPlacePiece(APiece* Piece, int32 GridX, int32 GridY, int32 RotationState);

	// Reverses the most recent placement and returns the piece that was
	// un-placed (nullptr if there was nothing to undo).
	UFUNCTION(BlueprintCallable, Category = "Grid")
	APiece* TryUndo();

	UFUNCTION(BlueprintCallable, Category = "Grid")
	bool IsLevelComplete() const;

	FGridModel& GetGridModel() { return GridModel; }
	const FGridModel& GetGridModel() const { return GridModel; }

private:
	FGridModel GridModel;
	FIntVector GridSize;

	// Parallel to FGridModel's own placement history -- GridModel only
	// knows UPieceDef+origin+rotation, not which world Actor represents it.
	TArray<APiece*> PlacedPieceHistory;
};
