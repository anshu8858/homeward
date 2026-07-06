#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DragController.generated.h"

// A component that can be attached to the PlayerController to handle piece dragging and rotation
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class HOMEWARD_API UDragController : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDragController();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Input handlers to be bound to EnhancedInput actions
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnTouchPressed(const FVector2D& TouchPosition);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnTouchMoved(const FVector2D& TouchPosition);

	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnTouchReleased(const FVector2D& TouchPosition);

private:
	FVector2D LastTouchPosition;
	float TouchDownTime;
	bool bIsDragging;
	
	// Tunable parameters (Tap vs Drag threshold)
	const float TapTimeThreshold = 0.15f; // 150ms
	const float TapDistanceThreshold = 12.0f; // 12 dp
};
