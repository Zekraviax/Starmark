#include "AIController_Avatar.h"


#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"


// ------------------------- AI
void AAIController_Avatar::GetMoveCommandFromPlayer_Implementation(FVector MoveLocation)
{
	if (GetBlackboardComponent()->IsValidLowLevel()) {
		GetBlackboardComponent()->SetValueAsVector("TargetLocation", MoveLocation);
	}
}