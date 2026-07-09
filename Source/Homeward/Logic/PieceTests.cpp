#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Piece.h"
#include "../Data/PieceDef.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPieceRotationTest, "Homeward.Logic.Piece.Rotation", EAutomationTestFlags::EditorContext | EAutomationTestFlags::SmokeFilter)

bool FPieceRotationTest::RunTest(const FString& Parameters)
{
	APiece* TestPiece = NewObject<APiece>();

	TestPiece->SetRotationState(1);
	TestEqual(TEXT("RotationState should be 1"), TestPiece->RotationState, 1);
	TestEqual(TEXT("Yaw should be 90 degrees"), TestPiece->GetActorRotation().Yaw, 90.0f);

	TestPiece->SetRotationState(4); // wraps to 0
	TestEqual(TEXT("RotationState should wrap to 0"), TestPiece->RotationState, 0);

	TestPiece->SetRotationState(-1); // wraps to 3
	TestEqual(TEXT("Negative rotation should wrap to 3"), TestPiece->RotationState, 3);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPieceInitializeTest, "Homeward.Logic.Piece.Initialize", EAutomationTestFlags::EditorContext | EAutomationTestFlags::SmokeFilter)

bool FPieceInitializeTest::RunTest(const FString& Parameters)
{
	APiece* TestPiece = NewObject<APiece>();
	UPieceDef* Def = NewObject<UPieceDef>();
	Def->PieceID = FName("TestPiece");

	TestPiece->InitializeFromDef(Def);
	TestEqual(TEXT("PieceDef should be assigned"), TestPiece->PieceDef, Def);
	TestFalse(TEXT("A freshly initialized piece should not be marked placed"), TestPiece->bIsPlaced);

	return true;
}
