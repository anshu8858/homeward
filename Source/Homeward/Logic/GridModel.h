#pragma once

#include "CoreMinimal.h"
#include "../Data/PieceDef.h"
#include "../Data/LevelDef.h"

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

	// The lowest layer that is not yet fully packed -- the only layer new
	// pieces may be placed into. Returns GridSize.Z once every layer (and
	// therefore the level) is complete.
	int32 GetActiveLayer() const;

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

	// A piece's own (rotated) cells plus, for Tall pieces, the same
	// footprint shifted one layer up -- the cells it reserves on the layer
	// above. This is the single place that knows how a piece occupies the
	// grid, so placement, collision, and undo all stay in sync.
	TArray<FIntVector> GetEffectiveCells(UPieceDef* PieceDef, int32 RotationState) const;
};
