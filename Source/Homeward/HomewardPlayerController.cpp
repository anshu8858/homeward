#include "HomewardPlayerController.h"

AHomewardPlayerController::AHomewardPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
}

void AHomewardPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindTouch(IE_Pressed, this, &AHomewardPlayerController::OnTouchPressed);
		InputComponent->BindTouch(IE_Released, this, &AHomewardPlayerController::OnTouchReleased);
		InputComponent->BindTouch(IE_Repeat, this, &AHomewardPlayerController::OnTouchMoved);
	}
}

void AHomewardPlayerController::OnTouchPressed(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	bIsDragging = true;
	LastTouchLocation = Location;
	// Raycast to find the piece to drag, then delegate to UDragController
}

void AHomewardPlayerController::OnTouchReleased(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (bIsDragging)
	{
		// If duration was short and movement was minimal, trigger a Yaw Rotation.
		// Otherwise, drop the piece onto the grid.
		bIsDragging = false;
	}
}

void AHomewardPlayerController::OnTouchMoved(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (bIsDragging)
	{
		// Calculate delta and translate piece via UDragController
		LastTouchLocation = Location;
	}
}
