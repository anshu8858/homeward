#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HomewardPlayerController.generated.h"

UCLASS()
class HOMEWARD_API AHomewardPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AHomewardPlayerController();

protected:
	virtual void SetupInputComponent() override;

	// Touch Input handlers
	void OnTouchPressed(const ETouchIndex::Type FingerIndex, const FVector Location);
	void OnTouchReleased(const ETouchIndex::Type FingerIndex, const FVector Location);
	void OnTouchMoved(const ETouchIndex::Type FingerIndex, const FVector Location);

private:
	bool bIsDragging;
	FVector LastTouchLocation;
};
