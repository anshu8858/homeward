#include "HomewardPlayerController.h"
#include "Logic/DragController.h"

AHomewardPlayerController::AHomewardPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableTouchEvents = true;

	DragController = CreateDefaultSubobject<UDragController>(TEXT("DragController"));
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
	if (DragController)
	{
		DragController->OnTouchPressed(FVector2D(Location.X, Location.Y));
	}
}

void AHomewardPlayerController::OnTouchReleased(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (DragController)
	{
		DragController->OnTouchReleased(FVector2D(Location.X, Location.Y));
	}
}

void AHomewardPlayerController::OnTouchMoved(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (DragController)
	{
		DragController->OnTouchMoved(FVector2D(Location.X, Location.Y));
	}
}
