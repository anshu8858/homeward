#include "SaveManager.h"
#include "Kismet/GameplayStatics.h"

void USaveManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadGame();
}

void USaveManager::Deinitialize()
{
	SaveGame();
	Super::Deinitialize();
}

void USaveManager::SaveGame()
{
	if (CurrentSave)
	{
		UGameplayStatics::SaveGameToSlot(CurrentSave, SaveSlotName, UserIndex);
	}
}

void USaveManager::LoadGame()
{
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex))
	{
		CurrentSave = Cast<UHomewardSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
	}
	else
	{
		CurrentSave = Cast<UHomewardSaveGame>(UGameplayStatics::CreateSaveGameObject(UHomewardSaveGame::StaticClass()));
	}
}
