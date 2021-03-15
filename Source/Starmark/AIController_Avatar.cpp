#include "AIController_Avatar.h"


#include "BehaviorTree/BlackboardComponent.h"


// ------------------------- AI
void AAIController_Avatar::GetMoveCommandFromPlayer_Implementation(FVector MoveLocation)
{
	GetBlackboardComponent().SetValueAsVector("TargetLocation", MoveLocation);
}