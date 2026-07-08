#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "GridModel.h"
#include "../Data/PieceDef.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGridModelBasicTest, "Homeward.Logic.GridModel.BasicPlacement", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGridModelRotationTest, "Homeward.Logic.GridModel.Rotation", EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)

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
