#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "GridModel.h"
#include "../Data/PieceDef.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGridModelBasicTest, "Homeward.Logic.GridModel.BasicPlacement", EAutomationTestFlags::EditorContext | EAutomationTestFlags::SmokeFilter)

bool FGridModelBasicTest::RunTest(const FString& Parameters)
{
	FGridModel Grid;
	Grid.Initialize(FIntVector(3, 3, 3)); // 3x3x3 grid

	// Mock a 1x1x1 piece
	UPieceDef* MockPiece = NewObject<UPieceDef>();
	MockPiece->Cells.Add(FIntVector(0, 0, 0));

	// Test placing a piece
	bool bCanPlace = Grid.CanPlacePiece(MockPiece, FIntVector(0, 0, 0), 0);
	TestTrue(TEXT("Should be able to place 1x1 at origin"), bCanPlace);

	Grid.PlacePiece(MockPiece, FIntVector(0, 0, 0), 0);

	// Try placing again at the same spot
	bool bCanPlaceAgain = Grid.CanPlacePiece(MockPiece, FIntVector(0, 0, 0), 0);
	TestFalse(TEXT("Should not be able to place on an occupied cell"), bCanPlaceAgain);

	// Test Undo
	Grid.UndoLastMove();
	bCanPlaceAgain = Grid.CanPlacePiece(MockPiece, FIntVector(0, 0, 0), 0);
	TestTrue(TEXT("Should be able to place after undo"), bCanPlaceAgain);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGridModelRotationTest, "Homeward.Logic.GridModel.Rotation", EAutomationTestFlags::EditorContext | EAutomationTestFlags::SmokeFilter)

bool FGridModelRotationTest::RunTest(const FString& Parameters)
{
	// Test 90 degree yaw rotation math
	TArray<FIntVector> Cells;
	Cells.Add(FIntVector(1, 0, 0));

	// Rot 1 (90 deg): x,y -> -y,x => (0, 1, 0)
	TArray<FIntVector> Rot1 = FGridModel::GetRotatedCells(Cells, 1);
	TestEqual(TEXT("Rotation 90 deg yaw X"), Rot1[0].X, 0);
	TestEqual(TEXT("Rotation 90 deg yaw Y"), Rot1[0].Y, 1);
	TestEqual(TEXT("Rotation 90 deg yaw Z"), Rot1[0].Z, 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGridModelActiveLayerTest, "Homeward.Logic.GridModel.ActiveLayer", EAutomationTestFlags::EditorContext | EAutomationTestFlags::SmokeFilter)

bool FGridModelActiveLayerTest::RunTest(const FString& Parameters)
{
	// 2 wide x 1 deep x 2 layers -- layer 0 needs both cells filled before
	// layer 1 unlocks.
	FGridModel Grid;
	Grid.Initialize(FIntVector(2, 1, 2));

	UPieceDef* Piece = NewObject<UPieceDef>();
	Piece->Cells.Add(FIntVector(0, 0, 0));

	TestEqual(TEXT("Active layer starts at 0"), Grid.GetActiveLayer(), 0);
	TestFalse(TEXT("Should not be able to place directly on layer 1 while layer 0 is incomplete"),
		Grid.CanPlacePiece(Piece, FIntVector(0, 0, 1), 0));

	Grid.PlacePiece(Piece, FIntVector(0, 0, 0), 0);
	Grid.PlacePiece(Piece, FIntVector(1, 0, 0), 0);

	TestTrue(TEXT("Layer 0 should be complete"), Grid.IsLayerComplete(0));
	TestEqual(TEXT("Active layer advances to 1 once layer 0 is full"), Grid.GetActiveLayer(), 1);
	TestTrue(TEXT("Should now be able to place on layer 1"),
		Grid.CanPlacePiece(Piece, FIntVector(0, 0, 1), 0));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGridModelFragileHeavyTest, "Homeward.Logic.GridModel.FragileHeavy", EAutomationTestFlags::EditorContext | EAutomationTestFlags::SmokeFilter)

bool FGridModelFragileHeavyTest::RunTest(const FString& Parameters)
{
	// 1x1x2: layer 0 is the true bottom, layer 1 is the true top.
	FGridModel Grid;
	Grid.Initialize(FIntVector(1, 1, 2));

	UPieceDef* HeavyPiece = NewObject<UPieceDef>();
	HeavyPiece->Cells.Add(FIntVector(0, 0, 0));
	HeavyPiece->bIsHeavy = true;

	UPieceDef* FragilePiece = NewObject<UPieceDef>();
	FragilePiece->Cells.Add(FIntVector(0, 0, 0));
	FragilePiece->bIsFragile = true;

	UPieceDef* NormalPiece = NewObject<UPieceDef>();
	NormalPiece->Cells.Add(FIntVector(0, 0, 0));

	TestTrue(TEXT("Heavy piece may be placed on the bottom layer"),
		Grid.CanPlacePiece(HeavyPiece, FIntVector(0, 0, 0), 0));

	Grid.PlacePiece(HeavyPiece, FIntVector(0, 0, 0), 0); // completes layer 0 -> active layer becomes 1

	TestTrue(TEXT("Fragile piece may be placed on the top layer"),
		Grid.CanPlacePiece(FragilePiece, FIntVector(0, 0, 1), 0));

	Grid.UndoLastMove(); // back to an empty grid, active layer 0 again

	TestFalse(TEXT("Fragile piece must not be placeable on a non-top layer"),
		Grid.CanPlacePiece(FragilePiece, FIntVector(0, 0, 0), 0));

	Grid.PlacePiece(NormalPiece, FIntVector(0, 0, 0), 0); // completes layer 0 -> active layer becomes 1

	TestFalse(TEXT("Heavy piece must not be placeable on a non-bottom layer"),
		Grid.CanPlacePiece(HeavyPiece, FIntVector(0, 0, 1), 0));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGridModelTallReservationTest, "Homeward.Logic.GridModel.TallReservation", EAutomationTestFlags::EditorContext | EAutomationTestFlags::SmokeFilter)

bool FGridModelTallReservationTest::RunTest(const FString& Parameters)
{
	// 1x1x2: a Tall piece placed on layer 0 should also reserve layer 1's
	// only cell, completing the whole level in a single placement.
	FGridModel Grid;
	Grid.Initialize(FIntVector(1, 1, 2));

	UPieceDef* TallPiece = NewObject<UPieceDef>();
	TallPiece->Cells.Add(FIntVector(0, 0, 0));
	TallPiece->bIsTall = true;

	UPieceDef* NormalPiece = NewObject<UPieceDef>();
	NormalPiece->Cells.Add(FIntVector(0, 0, 0));

	TestTrue(TEXT("Tall piece should be placeable on the active (bottom) layer"),
		Grid.CanPlacePiece(TallPiece, FIntVector(0, 0, 0), 0));

	Grid.PlacePiece(TallPiece, FIntVector(0, 0, 0), 0);

	TestTrue(TEXT("Layer 0 filled by the tall piece's own cell"), Grid.IsLayerComplete(0));
	TestTrue(TEXT("Layer 1 filled by the tall piece's reservation"), Grid.IsLayerComplete(1));
	TestTrue(TEXT("Level complete from a single tall placement"), Grid.IsLevelComplete());
	TestFalse(TEXT("Nothing further may be placed once the level is complete"),
		Grid.CanPlacePiece(NormalPiece, FIntVector(0, 0, 1), 0));

	Grid.UndoLastMove();

	TestFalse(TEXT("Undo should free the tall piece's own cell"), Grid.IsLayerComplete(0));
	TestFalse(TEXT("Undo should also free the tall piece's reservation"), Grid.IsLayerComplete(1));
	TestTrue(TEXT("Tall piece should be placeable again after undo"),
		Grid.CanPlacePiece(TallPiece, FIntVector(0, 0, 0), 0));

	return true;
}
