#include "Piece.h"
#include "Components/StaticMeshComponent.h"

APiece::APiece()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	PieceDef = nullptr;
	GridOrigin = FIntVector::ZeroValue;
	RotationState = 0;
	bIsPlaced = false;
}

void APiece::InitializeFromDef(UPieceDef* InPieceDef)
{
	PieceDef = InPieceDef;
	if (PieceDef && PieceDef->Mesh && MeshComponent)
	{
		MeshComponent->SetStaticMesh(PieceDef->Mesh);
	}
}

void APiece::SetRotationState(int32 NewRotationState)
{
	RotationState = ((NewRotationState % 4) + 4) % 4; // normalize to [0,3]
	SetActorRotation(FRotator(0.0f, RotationState * 90.0f, 0.0f));
}
