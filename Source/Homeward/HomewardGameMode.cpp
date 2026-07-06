#include "HomewardGameMode.h"
#include "HomewardPlayerController.h"

AHomewardGameMode::AHomewardGameMode()
{
	PlayerControllerClass = AHomewardPlayerController::StaticClass();
}

void AHomewardGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AHomewardGameMode::OnLevelCompleted()
{
	// Logic to transition to the next level or trigger the chapter vignette
}
