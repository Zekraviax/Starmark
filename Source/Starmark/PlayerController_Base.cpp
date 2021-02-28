#include "PlayerController_Base.h"


#include "NavigationSystem.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/World.h"
#include "Widget_HUD_Battle.h"
#include "WidgetComponent_AvatarBattleData.h"
#include "Starmark_GameState.h"


APlayerController_Base::APlayerController_Base()
{
	bShowMouseCursor = true;

	PlayerClickMode = E_PlayerCharacter_ClickModes::E_Nothing;
}


void APlayerController_Base::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps); 

	DOREPLIFETIME(APlayerController_Base, CurrentSelectedAvatar);
	DOREPLIFETIME(APlayerController_Base, BattleHUDCodeReference);
	DOREPLIFETIME(APlayerController_Base, PlayerParty);
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


// ------------------------- Widgets
void APlayerController_Base::UpdateBattleWidget(UWidget_HUD_Battle* BattleHUDReference)
{
	BattleHUDReference->AvatarBattleDataWidget->LinkedAvatar = CurrentSelectedAvatar;
	BattleHUDReference->AvatarBattleDataWidget->SetAvatarData();
	//AvatarTurnOrder.Num()
	AStarmark_GameState* GameStateRef = Cast<AStarmark_GameState>(GetWorld()->GetGameState());

	if (GameStateRef) {
		for (int i = 0; i < GameStateRef->AvatarTurnOrder.Num(); i++) {
			BattleHUDReference->TurnOrderTextBlock->SetText(FText::FromString(BattleHUDReference->TurnOrderTextBlock->GetText().ToString() + "\n" + GameStateRef->AvatarTurnOrder[i]->AvatarData.AvatarName));
		}
	}
}


// ------------------------- Avatar
void APlayerController_Base::SetRandomPawnAsSelectedPawn(ACharacter_Pathfinder* RandomPawnReference)
{
	for (TObjectIterator<ACharacter_Pathfinder> Itr; Itr; ++Itr) {
		ACharacter_Pathfinder* FoundActor = *Itr;

		if (FoundActor == RandomPawnReference) {
			CurrentSelectedAvatar = RandomPawnReference;

			CurrentSelectedAvatar->ActorSelected->SetVisibility(true);
			CurrentSelectedAvatar->CursorToWorld->SetVisibility(true);

			CurrentSelectedAvatar->ActorSelected_DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor::Green);
			break;
		}
	}
}


void APlayerController_Base::UpdateSelectedAvatar()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, ("PlayerController UpdateSelectedAvatar"));

	for (TObjectIterator<ACharacter_Pathfinder> Itr; Itr; ++Itr) {
		ACharacter_Pathfinder* FoundActor = *Itr;

		if (FoundActor->ActorSelected->IsValidLowLevel())
			FoundActor->ActorSelected->SetVisibility(false);

		if (FoundActor->CursorToWorld->IsValidLowLevel())
			FoundActor->CursorToWorld->SetVisibility(false);
	}

	if (CurrentSelectedAvatar->IsValidLowLevel()) {
		CurrentSelectedAvatar->ActorSelected->SetVisibility(true);
		CurrentSelectedAvatar->CursorToWorld->SetVisibility(true);

		if (CurrentSelectedAvatar->ActorSelected_DynamicMaterial)
			CurrentSelectedAvatar->ActorSelected_DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor::Green);
	}
}


void APlayerController_Base::SpawnPartyMember_Implementation()
{
	//if (PlayerParty.Num() > 0) {
		ACharacter_Pathfinder* NewAvatar = GetWorld()->SpawnActor<ACharacter_Pathfinder>(ACharacter_Pathfinder::StaticClass(), FVector(0.f, 0.f, 91.f) , FRotator::ZeroRotator);
		CurrentSelectedAvatar = NewAvatar;
		NewAvatar->BeginPlayWorkaroundFunction();
	//}
}


bool APlayerController_Base::SpawnPartyMember_Validate()
{
	return true;
}


// ------------------------- Mouse
void APlayerController_Base::OnPrimaryClick_Implementation(AActor* ClickedActor)
{
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
			// If all else fails, assume we clicked on a plane that we can move our controller Avatar to
			if (CurrentSelectedAvatar) {
				if (CurrentSelectedAvatar->GetController()->IsValidLowLevel()) {
					Cast<AAIController>(CurrentSelectedAvatar->GetController())->GetBlackboardComponent()->SetValueAsVector("TargetLocation", CursorLocationSnappedToGrid);
				}
			}
		}
	}
}


bool APlayerController_Base::OnPrimaryClick_Validate(AActor* ClickedActor)
{
	return true;
}


void APlayerController_Base::UpdateCursorSelectedMaterial()
{

}