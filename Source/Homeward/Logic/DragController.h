#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DragController.generated.h"

class APiece;
class APackingBox;

// A component attached to the PlayerController that turns raw touch input
// into the game's single verb (drag) and single modifier (tap = 90 degree
// yaw rotate). All grid legality and coordinate conversion is delegated to
// the active APackingBox; this class only owns touch-vs-tap disambiguation
// and whichever piece is currently held.
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HOMEWARD_API UDragController : public UActorComponent
{
	GENERATED_BODY()

public:
	UDragController();

	// Must be set (by level/Blueprint glue) before touch input can do
	// anything -- this is the box the player is currently packing.
	UFUNCTION(BlueprintCallable, Category = "Packing")
	void SetActivePackingBox(APackingBox* InPackingBox);

	// Input handlers to be bound to EnhancedInput actions / forwarded from
	// the owning PlayerController's touch delegates.
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnTouchPressed(const FVector2D& TouchPosition);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnTouchMoved(const FVector2D& TouchPosition);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnTouchReleased(const FVector2D& TouchPosition);

private:
	UPROPERTY()
	TWeakObjectPtr<APackingBox> PackingBox;

	UPROPERTY()
	TWeakObjectPtr<APiece> HeldPiece;

	FVector2D LastTouchPosition;
	float TouchDownTime;
	bool bIsDragging;

	// Tunable parameters (Tap vs Drag threshold)
	const float TapTimeThreshold = 0.15f; // 150ms
	const float TapDistanceThreshold = 12.0f; // 12 dp

	// Vertical lift (in cells) applied to a held piece so the player's
	// thumb never occludes the placement ghost. The plan also calls for a
	// small up-screen offset, which additionally depends on the fixed
	// camera's screen-space "up" direction; left as a follow-up tuning
	// pass once this is running on-device rather than guessed here.
	const float DragVerticalCellOffset = 1.5f;

	// Deprojects a screen touch into a world position on the active
	// layer's packing plane. False if there's no active box, no valid
	// camera, or the ray is parallel to the plane.
	bool ProjectTouchToActiveLayer(const FVector2D& TouchPosition, FVector& OutWorldPos) const;

	// Line-traces from the touch position and returns the APiece hit, if
	// any (whether it's sitting in the tray or already placed).
	APiece* TraceForPiece(const FVector2D& TouchPosition) const;
};
