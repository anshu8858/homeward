#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HomewardSaveGame.h"
#include "SaveManager.generated.h"

UCLASS()
class HOMEWARD_API USaveManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Save")
	void SaveGame();

	UFUNCTION(BlueprintCallable, Category = "Save")
	void LoadGame();

	UPROPERTY(BlueprintReadOnly, Category = "Save")
	UHomewardSaveGame* CurrentSave;

private:
	const FString SaveSlotName = TEXT("HomewardSaveSlot");
	const int32 UserIndex = 0;
};
