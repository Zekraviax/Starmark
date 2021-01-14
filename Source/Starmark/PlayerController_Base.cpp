#include "PlayerController_Base.h"


#include "NavigationSystem.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/World.h"


APlayerController_Base::APlayerController_Base()
{
	bShowMouseCursor = true;
}


void APlayerController_Base::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

}


void APlayerController_Base::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

}


// ------------------------- Base
void APlayerController_Base::SetRandomPawnAsSelectedPawn(ACharacter_Pathfinder* RandomPawnReference)
{
	for (TObjectIterator<ACharacter_Pathfinder> Itr; Itr; ++Itr) {
		ACharacter_Pathfinder* FoundActor = *Itr;

		if (FoundActor == RandomPawnReference) {
			CurrentSelectedPawn = RandomPawnReference;
			CurrentSelectedPawn->ActorSelected->SetVisibility(true);
			CurrentSelectedPawn->CursorToWorld->SetVisibility(true);
			CurrentSelectedPawn->ActorSelected->SetWorldLocation(FVector(CurrentSelectedPawn->GetActorLocation().X, CurrentSelectedPawn->GetActorLocation().Y, 1.f));
			CurrentSelectedPawn->ActorSelected_DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor::Red);
		} else {
			FoundActor->CursorToWorld->SetVisibility(false);
		}
	}
}


// ------------------------- Mouse
void APlayerController_Base::OnPrimaryClick(AActor* ClickedActor)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, ("Clicked Actor Class: &s", ClickedActor->GetClass()->GetName()));

	if (ClickedActor->GetClass() == ACharacter_Pathfinder::StaticClass()) {
		// Select Avatar
		// Attack Avatar
	} else {
		// If all else fails, assume we clicked on a plane that we can move our controller Avatar on
		Cast<AAIController>(CurrentSelectedPawn->GetController())->GetBlackboardComponent()->SetValueAsVector("TargetLocation", CursorLocationSnappedToGrid);
	}
}
