#include "DragController.h"

UDragController::UDragController()
{
	PrimaryComponentTick.bCanEverTick = true;
	bIsDragging = false;
	TouchDownTime = 0.0f;
}

void UDragController::BeginPlay()
{
	Super::BeginPlay();
}

void UDragController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UDragController::OnTouchPressed(const FVector2D& TouchPosition)
{
	LastTouchPosition = TouchPosition;
	TouchDownTime = GetWorld()->GetTimeSeconds();
	bIsDragging = false;
	
	// Raycast to find if we touched a piece to pick it up
}

void UDragController::OnTouchMoved(const FVector2D& TouchPosition)
{
	if (!bIsDragging)
	{
		float Distance = FVector2D::Distance(TouchPosition, LastTouchPosition);
		float Duration = GetWorld()->GetTimeSeconds() - TouchDownTime;
		
		if (Distance > TapDistanceThreshold || Duration > TapTimeThreshold)
		{
			bIsDragging = true;
		}
	}
	
	if (bIsDragging)
	{
		// Update the position of the dragged piece in world space (+1.5 cell vertical offset)
	}
}

void UDragController::OnTouchReleased(const FVector2D& TouchPosition)
{
	float Duration = GetWorld()->GetTimeSeconds() - TouchDownTime;
	
	if (!bIsDragging && Duration <= TapTimeThreshold)
	{
		// It was a tap! Rotate the piece 90 degrees yaw-only.
	}
	else if (bIsDragging)
	{
		// Attempt to snap to the grid and place via GridModel
	}
	
	bIsDragging = false;
}
