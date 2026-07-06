#pragma once

#include "CoreMinimal.h"
#include "Data/PieceDef.h"
#include "Data/LevelDef.h"

// Represents a placed piece in the grid
struct FPlacedPiece
{
	UPieceDef* PieceDef;
	FIntVector Origin;
	int32 RotationState; // 0, 1, 2, 3 (yaw rotations)
};

class HOMEWARD_API FGridModel
{
public:
	FGridModel();
	~FGridModel();

	void Initialize(const FIntVector& InGridSize);

	// Returns true if the piece can be placed at the given origin with the specified rotation
	bool CanPlacePiece(UPieceDef* PieceDef, const FIntVector& Origin, int32 RotationState) const;

	// Places a piece in the grid. Assumes CanPlacePiece was already checked.
	void PlacePiece(UPieceDef* PieceDef, const FIntVector& Origin, int32 RotationState);

	// Removes the most recently placed piece
	bool UndoLastMove();

	// Resets the grid completely
	void ResetGrid();

	// Checks if the grid is completely filled up to a specific layer
	bool IsLayerComplete(int32 LayerIndex) const;

	// Checks if the level is completely filled
	bool IsLevelComplete() const;

	// Translates piece cells based on rotation (90 degree yaw increments)
	static TArray<FIntVector> GetRotatedCells(const TArray<FIntVector>& LocalCells, int32 RotationState);

private:
	FIntVector GridSize;
	
	// 3D grid representation. True = filled, False = empty.
	// Indexed by (z * GridSize.X * GridSize.Y) + (y * GridSize.X) + x
	TArray<bool> GridCells;

	TArray<FPlacedPiece> PlacedHistory;

	int32 GetCellIndex(const FIntVector& Position) const;
	bool IsValidPosition(const FIntVector& Position) const;
};
