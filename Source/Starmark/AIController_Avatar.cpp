#include "AIController_Avatar.h"

#include "Character_Pathfinder.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"


// ------------------------- AI
void AAIController_Avatar::GetMoveCommandFromPlayer_Implementation(FVector MoveLocation, int TilesPerTurnCost, ACharacter_Pathfinder* AvatarReference)
{
	//if (GetBlackboardComponent()->IsValidLowLevel()) {
	//	GetBlackboardComponent()->SetValueAsVector("TargetLocation", MoveLocation);

	//	AvatarReference->AvatarData.CurrentTileMoves -= TilesPerTurnCost;

	//	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Tiles Per Turn Cost: %d"), TilesPerTurnCost));
	//}

	AvatarReference->AvatarData.CurrentTileMoves -= TilesPerTurnCost;
}