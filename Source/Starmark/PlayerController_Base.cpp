#include "PlayerController_Base.h"


#include "NavigationSystem.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/World.h"
#include "Widget_HUD_Battle.h"
#include "WidgetComponent_AvatarBattleData.h"
#include "Starmark_GameState.h"
#include "PlayerState_Base.h"
#include "PlayerPawn_Static.h"


APlayerController_Base::APlayerController_Base()
{
	bShowMouseCursor = true;

	IsCurrentlyActingPlayer = false;
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
void APlayerController_Base::UpdateBattleWidget(UWidget_HUD_Battle* BattleHUDReference, FAvatar_Struct NewAvatarData)
{
	if (BattleHUDReference->IsValidLowLevel()) {
		BattleHUDCodeReference = BattleHUDReference;
	}

	if (BattleHUDReference->IsValidLowLevel() && CurrentSelectedAvatar) {
		BattleHUDReference->AvatarBattleDataWidget->UpdateAvatarData(NewAvatarData);
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


// ------------------------- Mouse
void APlayerController_Base::OnPrimaryClick(AActor* ClickedActor)
{
	if (!CurrentSelectedAvatar) {
		CurrentSelectedAvatar = Cast<APlayerState_Base>(Cast<APlayerPawn_Static>(GetPawn())->GetPlayerState())->PlayerState_CurrentControlledAvatar;
	}


	if (ClickedActor && IsCurrentlyActingPlayer) {
		if (ClickedActor->GetClass()->GetName().Contains("Character")) {
			// Select Avatar To Control
			if (CurrentSelectedAvatar != ClickedActor && PlayerClickMode == E_PlayerCharacter_ClickModes::E_SelectCharacterToControl) {
				Cast<ACharacter_Pathfinder>(ClickedActor)->OnAvatarClicked();
			}
			// Select Avatar to Begin Attack
			else if (CurrentSelectedAvatar != ClickedActor && PlayerClickMode == E_PlayerCharacter_ClickModes::E_SelectCharacterToAttack) {
				// If we're attacking, and we clicked on a valid target in-range, launch an attack
				if (CurrentSelectedAvatar->ValidAttackTargetsArray.Num() > 0) {
					if (CurrentSelectedAvatar->ValidAttackTargetsArray.Contains(Cast<ACharacter_Pathfinder>(ClickedActor))) {
						GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Launch Attack")));
						CurrentSelectedAvatar->LaunchAttack_Implementation(CurrentSelectedAvatar->ValidAttackTargetsArray[0]);
					}
				}
				else {
					GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("No Valid Targets")));
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


// ------------------------- Battle
void APlayerController_Base::Server_SubtractHealth_Implementation(ACharacter_Pathfinder* Defender, int DamageDealt)
{
	Cast<AStarmark_GameMode>(GetWorld()->GetAuthGameMode())->Battle_SubtractHealth_Implementation(Defender, DamageDealt);
}