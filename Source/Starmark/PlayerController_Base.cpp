#include "PlayerController_Base.h"


#include "NavigationSystem.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/World.h"


APlayerController_Base::APlayerController_Base()
{
	bShowMouseCursor = true;

	PlayerClickMode = E_PlayerCharacter_ClickModes::E_Nothing;
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
			CurrentSelectedAvatar = RandomPawnReference;

			CurrentSelectedAvatar->ActorSelected->SetVisibility(true);
			CurrentSelectedAvatar->CursorToWorld->SetVisibility(true);
			//CurrentSelectedAvatar->ActorSelected->SetWorldLocation(FVector(CurrentSelectedAvatar->GetActorLocation().X, CurrentSelectedAvatar->GetActorLocation().Y, 1.f));

			CurrentSelectedAvatar->ActorSelected_DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor::Green);

			//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, ("Found Actor"));
			break;
		}
	}
}


void APlayerController_Base::UpdateSelectedAvatar()
{
	for (TObjectIterator<ACharacter_Pathfinder> Itr; Itr; ++Itr) {
		ACharacter_Pathfinder* FoundActor = *Itr;

		if (FoundActor->ActorSelected->IsValidLowLevel())
			FoundActor->ActorSelected->SetVisibility(false);

		if (FoundActor->CursorToWorld->IsValidLowLevel())
			FoundActor->CursorToWorld->SetVisibility(false);
	}

	CurrentSelectedAvatar->ActorSelected->SetVisibility(true);
	CurrentSelectedAvatar->CursorToWorld->SetVisibility(true);

	if (CurrentSelectedAvatar->ActorSelected_DynamicMaterial)
		CurrentSelectedAvatar->ActorSelected_DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor::Green);
}


// ------------------------- Mouse
void APlayerController_Base::OnPrimaryClick(AActor* ClickedActor)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, ("Clicked Actor Class: &s", ClickedActor->GetClass()->GetName()));

	if (ClickedActor) {
		if (ClickedActor->GetClass()->GetName().Contains("Character")) {
			// Select Avatar To Control
			if (CurrentSelectedAvatar != ClickedActor && PlayerClickMode == E_PlayerCharacter_ClickModes::E_SelectCharacterToControl) {
				Cast<ACharacter_Pathfinder>(ClickedActor)->OnAvatarClicked();
			}
			// Select Avatar to Begin Attack
			else if (CurrentSelectedAvatar != ClickedActor && PlayerClickMode == E_PlayerCharacter_ClickModes::E_SelectCharacterToAttack) {
				// If we're attacking, and we clicked on a target in-range, launch an attack
				if (CurrentSelectedAvatar->ValidAttackTargetsArray.Contains(Cast<ACharacter_Pathfinder>(ClickedActor))) {
					CurrentSelectedAvatar->LaunchAttack(Cast<ACharacter_Pathfinder>(ClickedActor));
				}
			}
		}
		else if (ClickedActor->GetClass()->GetName().Contains("StaticMesh") || ClickedActor->GetClass()->GetName().Contains("GridTile") && PlayerClickMode == E_PlayerCharacter_ClickModes::E_MoveCharacter) {
			// If all else fails, assume we clicked on a plane that we can move our controller Avatar on
			Cast<AAIController>(CurrentSelectedAvatar->GetController())->GetBlackboardComponent()->SetValueAsVector("TargetLocation", CursorLocationSnappedToGrid);
		}
		else {
			// Do nothing
		}
	}
}
