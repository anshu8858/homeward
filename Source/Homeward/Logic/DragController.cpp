#include "DragController.h"
#include "PackingBox.h"
#include "Piece.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

UDragController::UDragController()
{
	PrimaryComponentTick.bCanEverTick = false;
	bIsDragging = false;
	TouchDownTime = 0.0f;
}

void UDragController::SetActivePackingBox(APackingBox* InPackingBox)
{
	PackingBox = InPackingBox;
}

bool UDragController::ProjectTouchToActiveLayer(const FVector2D& TouchPosition, FVector& OutWorldPos) const
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC || !PackingBox.IsValid())
	{
		return false;
	}

	FVector RayOrigin, RayDirection;
	if (!PC->DeprojectScreenPositionToWorld(TouchPosition.X, TouchPosition.Y, RayOrigin, RayDirection))
	{
		return false;
	}

	if (FMath::IsNearlyZero(RayDirection.Z))
	{
		return false; // ray parallel to the horizontal packing plane
	}

	const float PlaneZ = PackingBox->GetActiveLayerWorldZ();
	const float T = (PlaneZ - RayOrigin.Z) / RayDirection.Z;
	OutWorldPos = RayOrigin + RayDirection * T;
	return true;
}

APiece* UDragController::TraceForPiece(const FVector2D& TouchPosition) const
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC)
	{
		return nullptr;
	}

	FVector RayOrigin, RayDirection;
	if (!PC->DeprojectScreenPositionToWorld(TouchPosition.X, TouchPosition.Y, RayOrigin, RayDirection))
	{
		return nullptr;
	}

	FHitResult Hit;
	const FVector TraceEnd = RayOrigin + RayDirection * 100000.0f;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(DragControllerPieceTrace), false);
	if (GetWorld()->LineTraceSingleByChannel(Hit, RayOrigin, TraceEnd, ECC_Visibility, Params))
	{
		return Cast<APiece>(Hit.GetActor());
	}
	return nullptr;
}

void UDragController::OnTouchPressed(const FVector2D& TouchPosition)
{
	LastTouchPosition = TouchPosition;
	TouchDownTime = GetWorld()->GetTimeSeconds();
	bIsDragging = false;
	HeldPiece = TraceForPiece(TouchPosition);
}

void UDragController::OnTouchMoved(const FVector2D& TouchPosition)
{
	if (!HeldPiece.IsValid())
	{
		return;
	}

	if (!bIsDragging)
	{
		const float Distance = FVector2D::Distance(TouchPosition, LastTouchPosition);
		const float Duration = GetWorld()->GetTimeSeconds() - TouchDownTime;

		if (Distance > TapDistanceThreshold || Duration > TapTimeThreshold)
		{
			bIsDragging = true;
		}
	}

	if (bIsDragging && PackingBox.IsValid())
	{
		FVector WorldPos;
		if (ProjectTouchToActiveLayer(TouchPosition, WorldPos))
		{
			WorldPos.Z += DragVerticalCellOffset * PackingBox->CellSize;
			HeldPiece->SetActorLocation(WorldPos);
		}
	}
}

void UDragController::OnTouchReleased(const FVector2D& TouchPosition)
{
	const float Duration = GetWorld()->GetTimeSeconds() - TouchDownTime;

	if (HeldPiece.IsValid())
	{
		if (!bIsDragging && Duration <= TapTimeThreshold)
		{
			// Tap: rotate 90 degrees yaw-only, cycling through 4 states.
			const int32 NextRotation = (HeldPiece->RotationState + 1) % 4;
			HeldPiece->SetRotationState(NextRotation);
		}
		else if (bIsDragging && PackingBox.IsValid())
		{
			FVector WorldPos;
			if (ProjectTouchToActiveLayer(TouchPosition, WorldPos))
			{
				int32 GridX, GridY;
				if (PackingBox->WorldToGridColumn(WorldPos, GridX, GridY))
				{
					// On an invalid drop this simply does nothing further --
					// floating the piece back to its tray slot is a
					// Blueprint/juice concern; this class only decides
					// grid legality.
					PackingBox->TryPlacePiece(HeldPiece.Get(), GridX, GridY, HeldPiece->RotationState);
				}
			}
		}
	}

	bIsDragging = false;
	HeldPiece = nullptr;
}
