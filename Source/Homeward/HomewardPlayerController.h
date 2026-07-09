#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HomewardPlayerController.generated.h"

class UDragController;

UCLASS()
class HOMEWARD_API AHomewardPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AHomewardPlayerController();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	UDragController* DragController;

protected:
	virtual void SetupInputComponent() override;

	// Touch input handlers -- thin forwards into DragController, which owns
	// all tap-vs-drag disambiguation and grid interaction logic. This is
	// the only place touch state lives; it used to be duplicated here too
	// (bIsDragging/LastTouchLocation) with no connection to DragController
	// at all, so the two could desync.
	void OnTouchPressed(const ETouchIndex::Type FingerIndex, const FVector Location);
	void OnTouchReleased(const ETouchIndex::Type FingerIndex, const FVector Location);
	void OnTouchMoved(const ETouchIndex::Type FingerIndex, const FVector Location);
};
