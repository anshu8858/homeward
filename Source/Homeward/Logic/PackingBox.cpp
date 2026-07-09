#include "PackingBox.h"
#include "Piece.h"
#include "../Data/LevelDef.h"
#include "../HomewardGameMode.h"
#include "Kismet/GameplayStatics.h"

APackingBox::APackingBox()
{
	PrimaryActorTick.bCanEverTick = false;
	GridOriginWorld = FVector::ZeroVector;
	CellSize = 100.0f;
	GridSize = FIntVector::ZeroValue;
}

void APackingBox::BeginPlay()
{
	Super::BeginPlay();

	// Self-register with the GameMode so it always knows which box is
	// active without a separate level-Blueprint wiring step.
	if (AHomewardGameMode* GameMode = Cast<AHomewardGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GameMode->RegisterActivePackingBox(this);
	}
}

void APackingBox::InitializeFromLevelDef(ULevelDef* LevelDef)
{
	if (!LevelDef) return;

	GridSize = LevelDef->GridSize;
	GridModel.Initialize(GridSize);
	PlacedPieceHistory.Empty();
}

FVector APackingBox::GridToWorld(const FIntVector& Cell) const
{
	return GridOriginWorld + FVector(
		(Cell.X + 0.5f) * CellSize,
		(Cell.Y + 0.5f) * CellSize,
		(Cell.Z + 0.5f) * CellSize);
}

bool APackingBox::WorldToGridColumn(const FVector& WorldPos, int32& OutX, int32& OutY) const
{
	if (CellSize <= 0.0f) return false;

	const FVector Local = WorldPos - GridOriginWorld;
	OutX = FMath::FloorToInt(Local.X / CellSize);
	OutY = FMath::FloorToInt(Local.Y / CellSize);
	return true;
}

float APackingBox::GetActiveLayerWorldZ() const
{
	const int32 ActiveLayer = GridModel.GetActiveLayer();
	return GridOriginWorld.Z + (ActiveLayer + 0.5f) * CellSize;
}

bool APackingBox::CanPlacePieceAt(APiece* Piece, int32 GridX, int32 GridY, int32 RotationState) const
{
	if (!Piece || !Piece->PieceDef) return false;

	const int32 ActiveLayer = GridModel.GetActiveLayer();
	const FIntVector Origin(GridX, GridY, ActiveLayer);
	return GridModel.CanPlacePiece(Piece->PieceDef, Origin, RotationState);
}

bool APackingBox::TryPlacePiece(APiece* Piece, int32 GridX, int32 GridY, int32 RotationState)
{
	if (!CanPlacePieceAt(Piece, GridX, GridY, RotationState)) return false;

	const int32 ActiveLayer = GridModel.GetActiveLayer();
	const FIntVector Origin(GridX, GridY, ActiveLayer);

	GridModel.PlacePiece(Piece->PieceDef, Origin, RotationState);

	Piece->GridOrigin = Origin;
	Piece->SetRotationState(RotationState);
	Piece->bIsPlaced = true;
	Piece->SetActorLocation(GridToWorld(Origin));

	PlacedPieceHistory.Add(Piece);

	// GetWorld() guard: this box may not be in a real world yet (e.g. a
	// unit test constructing it via NewObject rather than SpawnActor), in
	// which case there is no GameMode to notify and UGameplayStatics
	// would otherwise log a "no world" error trying to find one.
	if (GridModel.IsLevelComplete() && GetWorld())
	{
		if (AHomewardGameMode* GameMode = Cast<AHomewardGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			GameMode->OnLevelCompleted();
		}
	}

	return true;
}

APiece* APackingBox::TryUndo()
{
	if (PlacedPieceHistory.Num() == 0) return nullptr;
	if (!GridModel.UndoLastMove()) return nullptr;

	APiece* Piece = PlacedPieceHistory.Pop();
	if (Piece)
	{
		Piece->bIsPlaced = false;
	}
	return Piece;
}

bool APackingBox::IsLevelComplete() const
{
	return GridModel.IsLevelComplete();
}
