#include "HomewardSaveGame.h"

UHomewardSaveGame::UHomewardSaveGame()
{
	SchemaVersion = 1;
	DailyStreak = 0;
	LastDailyCompletionDate = FDateTime(0); // never completed
}
