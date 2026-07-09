#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "PackingBox.h"
#include "Piece.h"
#include "../Data/PieceDef.h"
#include "../Data/LevelDef.h"

// NewObject<APackingBox>()/NewObject<APiece>() below deliberately skip
// SpawnActor/BeginPlay -- these tests exercise pure grid/coordinate logic
// only (none of it calls GetWorld()), the same way GridModelTests.cpp
// exercises FGridModel without any Actor at all.

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPackingBoxPlacementTest, "Homeward.Logic.PackingBox.Placement", EAutomationTestFlags::EditorContext | EAutomationTestFlags::SmokeFilter)

bool FPackingBoxPlacementTest::RunTest(const FString& Parameters)
{
	APackingBox* Box = NewObject<APackingBox>();
	Box->CellSize = 100.0f;
	Box->GridOriginWorld = FVector::ZeroVector;

	ULevelDef* LevelDef = NewObject<ULevelDef>();
	LevelDef->GridSize = FIntVector(2, 2, 1);
	Box->InitializeFromLevelDef(LevelDef);

	UPieceDef* Def = NewObject<UPieceDef>();
	Def->Cells.Add(FIntVector(0, 0, 0));

	APiece* TestPiece = NewObject<APiece>();
	TestPiece->InitializeFromDef(Def);

	TestTrue(TEXT("Should be able to place at (0,0)"), Box->CanPlacePieceAt(TestPiece, 0, 0, 0));
	TestTrue(TEXT("Placement should succeed"), Box->TryPlacePiece(TestPiece, 0, 0, 0));
	TestTrue(TEXT("Piece should be marked placed"), TestPiece->bIsPlaced);

	const FVector Expected = Box->GridToWorld(FIntVector(0, 0, 0));
	TestEqual(TEXT("World X should match GridToWorld"), TestPiece->GetActorLocation().X, Expected.X);
	TestEqual(TEXT("World Y should match GridToWorld"), TestPiece->GetActorLocation().Y, Expected.Y);
	TestEqual(TEXT("World Z should match GridToWorld"), TestPiece->GetActorLocation().Z, Expected.Z);

	TestFalse(TEXT("Should not be able to place again at the same cell"), Box->CanPlacePieceAt(TestPiece, 0, 0, 0));

	APiece* Undone = Box->TryUndo();
	TestEqual(TEXT("Undo should return the piece that was placed"), Undone, TestPiece);
	TestFalse(TEXT("Undone piece should no longer be marked placed"), TestPiece->bIsPlaced);
	TestTrue(TEXT("Cell should be free again after undo"), Box->CanPlacePieceAt(TestPiece, 0, 0, 0));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPackingBoxCoordinateTest, "Homeward.Logic.PackingBox.Coordinates", EAutomationTestFlags::EditorContext | EAutomationTestFlags::SmokeFilter)

bool FPackingBoxCoordinateTest::RunTest(const FString& Parameters)
{
	APackingBox* Box = NewObject<APackingBox>();
	Box->CellSize = 100.0f;
	Box->GridOriginWorld = FVector(500.0f, 200.0f, 0.0f);

	const FVector WorldPos = Box->GridToWorld(FIntVector(2, 1, 0));
	// FVector components are double in UE5; these must be double literals
	// too, or TestEqual can't pick between its float/double overloads.
	TestEqual(TEXT("GridToWorld X"), WorldPos.X, 500.0 + 250.0);
	TestEqual(TEXT("GridToWorld Y"), WorldPos.Y, 200.0 + 150.0);
	TestEqual(TEXT("GridToWorld Z"), WorldPos.Z, 50.0);

	int32 OutX, OutY;
	TestTrue(TEXT("WorldToGridColumn should succeed"), Box->WorldToGridColumn(WorldPos, OutX, OutY));
	TestEqual(TEXT("Round-trip X"), OutX, 2);
	TestEqual(TEXT("Round-trip Y"), OutY, 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPackingBoxLevelCompleteTest, "Homeward.Logic.PackingBox.LevelComplete", EAutomationTestFlags::EditorContext | EAutomationTestFlags::SmokeFilter)

bool FPackingBoxLevelCompleteTest::RunTest(const FString& Parameters)
{
	APackingBox* Box = NewObject<APackingBox>();
	Box->CellSize = 100.0f;

	ULevelDef* LevelDef = NewObject<ULevelDef>();
	LevelDef->GridSize = FIntVector(2, 1, 1); // 2 cells total
	Box->InitializeFromLevelDef(LevelDef);

	UPieceDef* Def = NewObject<UPieceDef>();
	Def->Cells.Add(FIntVector(0, 0, 0));

	APiece* PieceA = NewObject<APiece>();
	PieceA->InitializeFromDef(Def);
	APiece* PieceB = NewObject<APiece>();
	PieceB->InitializeFromDef(Def);

	TestFalse(TEXT("Level should not be complete before any placement"), Box->IsLevelComplete());
	Box->TryPlacePiece(PieceA, 0, 0, 0);
	TestFalse(TEXT("Level should not be complete with one of two cells filled"), Box->IsLevelComplete());
	Box->TryPlacePiece(PieceB, 1, 0, 0);
	TestTrue(TEXT("Level should be complete once every cell is filled"), Box->IsLevelComplete());

	return true;
}
