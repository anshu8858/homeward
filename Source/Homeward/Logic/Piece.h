#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Data/PieceDef.h"
#include "Piece.generated.h"

class UStaticMeshComponent;

// The in-world representation of a single packable item. Visuals (mesh,
// palette material) are applied from a UPieceDef; this class only tracks
// the piece's identity and its current placement state in grid space --
// coordinate conversion and grid legality live in APackingBox.
UCLASS()
class HOMEWARD_API APiece : public AActor
{
	GENERATED_BODY()

public:
	APiece();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Piece")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Piece")
	UPieceDef* PieceDef;

	// Grid-space origin while placed. Meaningless while bIsPlaced is false
	// (i.e. while sitting in the tray or being dragged).
	UPROPERTY(BlueprintReadOnly, Category = "Piece")
	FIntVector GridOrigin;

	UPROPERTY(BlueprintReadOnly, Category = "Piece")
	int32 RotationState;

	UPROPERTY(BlueprintReadOnly, Category = "Piece")
	bool bIsPlaced;

	// Assigns identity and applies the piece's mesh. Palette material
	// application is left to Blueprint/editor glue.
	UFUNCTION(BlueprintCallable, Category = "Piece")
	void InitializeFromDef(UPieceDef* InPieceDef);

	// Rotates 90 degrees yaw-only, cycling through 4 states. Purely visual +
	// bookkeeping; callers must have already confirmed the rotation is
	// legal via FGridModel before calling this.
	UFUNCTION(BlueprintCallable, Category = "Piece")
	void SetRotationState(int32 NewRotationState);
};
