#include "GridModel.h"

FGridModel::FGridModel()
{
}

FGridModel::~FGridModel()
{
}

void FGridModel::Initialize(const FIntVector& InGridSize)
{
	GridSize = InGridSize;
	GridCells.Init(false, GridSize.X * GridSize.Y * GridSize.Z);
	PlacedHistory.Empty();
}

bool FGridModel::IsValidPosition(const FIntVector& Position) const
{
	return Position.X >= 0 && Position.X < GridSize.X &&
	       Position.Y >= 0 && Position.Y < GridSize.Y &&
	       Position.Z >= 0 && Position.Z < GridSize.Z;
}

int32 FGridModel::GetCellIndex(const FIntVector& Position) const
{
	return (Position.Z * GridSize.X * GridSize.Y) + (Position.Y * GridSize.X) + Position.X;
}

TArray<FIntVector> FGridModel::GetRotatedCells(const TArray<FIntVector>& LocalCells, int32 RotationState)
{
	TArray<FIntVector> Rotated;
	Rotated.Reserve(LocalCells.Num());

	for (const FIntVector& Cell : LocalCells)
	{
		FIntVector RCell = Cell;
		// 90 degree yaw rotation: (x, y) -> (-y, x)
		for (int32 i = 0; i < RotationState; ++i)
		{
			int32 TempX = RCell.X;
			RCell.X = -RCell.Y;
			RCell.Y = TempX;
		}
		Rotated.Add(RCell);
	}
	return Rotated;
}

TArray<FIntVector> FGridModel::GetEffectiveCells(UPieceDef* PieceDef, int32 RotationState) const
{
	TArray<FIntVector> Effective = GetRotatedCells(PieceDef->Cells, RotationState);

	if (PieceDef->bIsTall)
	{
		// Reserve the same footprint one layer up. Iterate over the original
		// count only -- we're appending to the same array.
		const int32 OwnCellCount = Effective.Num();
		for (int32 i = 0; i < OwnCellCount; ++i)
		{
			FIntVector Reserved = Effective[i];
			Reserved.Z += 1;
			Effective.Add(Reserved);
		}
	}

	return Effective;
}

int32 FGridModel::GetActiveLayer() const
{
	for (int32 z = 0; z < GridSize.Z; ++z)
	{
		if (!IsLayerComplete(z))
		{
			return z;
		}
	}
	return GridSize.Z; // every layer full -> level complete, nothing more to place
}

bool FGridModel::CanPlacePiece(UPieceDef* PieceDef, const FIntVector& Origin, int32 RotationState) const
{
	if (!PieceDef) return false;

	// Pieces are packed one horizontal layer at a time: a piece's own cells
	// (as opposed to a Tall piece's reservation above them) must all land
	// exactly on the active layer -- the lowest layer not yet complete.
	const int32 ActiveLayer = GetActiveLayer();
	if (ActiveLayer >= GridSize.Z) return false; // level already complete

	TArray<FIntVector> OwnCells = GetRotatedCells(PieceDef->Cells, RotationState);
	for (const FIntVector& Cell : OwnCells)
	{
		if (Origin.Z + Cell.Z != ActiveLayer)
		{
			return false;
		}
	}

	// Heavy pieces may only rest on the true bottom of the box; fragile
	// pieces may only sit on the true top, since nothing may ever be placed
	// above them. Both rules are naturally inert in a single-layer grid.
	if (PieceDef->bIsHeavy && ActiveLayer != 0) return false;
	if (PieceDef->bIsFragile && ActiveLayer != GridSize.Z - 1) return false;

	TArray<FIntVector> EffectiveCells = GetEffectiveCells(PieceDef, RotationState);
	for (const FIntVector& Cell : EffectiveCells)
	{
		FIntVector WorldPos = Origin + Cell;
		if (!IsValidPosition(WorldPos))
		{
			return false;
		}
		if (GridCells[GetCellIndex(WorldPos)])
		{
			return false;
		}
	}
	return true;
}

void FGridModel::PlacePiece(UPieceDef* PieceDef, const FIntVector& Origin, int32 RotationState)
{
	if (!PieceDef) return;

	TArray<FIntVector> EffectiveCells = GetEffectiveCells(PieceDef, RotationState);

	for (const FIntVector& Cell : EffectiveCells)
	{
		FIntVector WorldPos = Origin + Cell;
		GridCells[GetCellIndex(WorldPos)] = true;
	}

	FPlacedPiece Placed;
	Placed.PieceDef = PieceDef;
	Placed.Origin = Origin;
	Placed.RotationState = RotationState;
	PlacedHistory.Add(Placed);
}

bool FGridModel::UndoLastMove()
{
	if (PlacedHistory.Num() == 0) return false;

	FPlacedPiece LastPiece = PlacedHistory.Pop();

	TArray<FIntVector> EffectiveCells = GetEffectiveCells(LastPiece.PieceDef, LastPiece.RotationState);

	for (const FIntVector& Cell : EffectiveCells)
	{
		FIntVector WorldPos = LastPiece.Origin + Cell;
		GridCells[GetCellIndex(WorldPos)] = false;
	}
	return true;
}

void FGridModel::ResetGrid()
{
	for (int32 i = 0; i < GridCells.Num(); ++i)
	{
		GridCells[i] = false;
	}
	PlacedHistory.Empty();
}

bool FGridModel::IsLayerComplete(int32 LayerIndex) const
{
	if (LayerIndex < 0 || LayerIndex >= GridSize.Z) return false;

	for (int32 y = 0; y < GridSize.Y; ++y)
	{
		for (int32 x = 0; x < GridSize.X; ++x)
		{
			FIntVector Pos(x, y, LayerIndex);
			if (!GridCells[GetCellIndex(Pos)])
			{
				return false;
			}
		}
	}
	return true;
}

bool FGridModel::IsLevelComplete() const
{
	for (int32 i = 0; i < GridSize.Z; ++i)
	{
		if (!IsLayerComplete(i))
		{
			return false;
		}
	}
	return true;
}
