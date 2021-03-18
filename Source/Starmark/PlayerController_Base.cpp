#include "PlayerController_Base.h"


#include "NavigationSystem.h"
#include "AIController.h"
#include "AIController_Avatar.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/World.h"
#include "Widget_HUD_Battle.h"
#include "WidgetComponent_AvatarBattleData.h"
#include "Starmark_GameState.h"
#include "Starmark_PlayerState.h"
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
	DOREPLIFETIME(APlayerController_Base, BattleWidgetReference);
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

	if (IsCurrentlyActingPlayer) 
		GEngine->AddOnScreenDebugMessage(-1, 0.15f, FColor::Green, FString::Printf(TEXT("IsCurrentlyActingPlayer")));
	else
		GEngine->AddOnScreenDebugMessage(-1, 0.15f, FColor::Red, FString::Printf(TEXT("IsNotCurrentlyActingPlayer")));

	if (CurrentSelectedAvatar) {
		SetBattleWidgetAndLinkedAvatar(BattleWidgetReference, CurrentSelectedAvatar->AvatarData);
	}
}


// ------------------------- Widgets
void APlayerController_Base::CreateBattleWidget()
{
	if (BattleWidgetChildClass && IsLocalPlayerController()) {
		BattleWidgetReference = CreateWidget<UWidget_HUD_Battle>(this, BattleWidgetChildClass);

		if (BattleWidgetReference) {
			BattleWidgetReference->AddToViewport();
		}
	}
}


void APlayerController_Base::SetBattleWidgetVariables()
{
	if (BattleWidgetReference->IsValidLowLevel()) {
		BattleWidgetReference->PlayerControllerReference = this;

		if (CurrentSelectedAvatar)
			BattleWidgetReference->AvatarBattleDataWidget->LinkedAvatar = CurrentSelectedAvatar->AvatarData;
	}
}


void APlayerController_Base::SetBattleWidgetAndLinkedAvatar(UWidget_HUD_Battle* NewBattleWidgetReference, FAvatar_Struct NewAvatarData)
{
	if (NewBattleWidgetReference->IsValidLowLevel()) {
		BattleWidgetReference = NewBattleWidgetReference;
	}

	if (BattleWidgetReference->IsValidLowLevel() && CurrentSelectedAvatar) {
		if (BattleWidgetReference->AvatarBattleDataWidget->IsValidLowLevel()) {
			BattleWidgetReference->AvatarBattleDataWidget->UpdateAvatarData(NewAvatarData);
		}
	}
}


// ------------------------- Avatar
void APlayerController_Base::OnRepNotify_CurrentSelectedAvatar()
{
	AStarmark_PlayerState* PlayerStateReference = Cast<AStarmark_PlayerState>(GetPawn()->GetPlayerState());

	// (Default) Player party initialization
	if (PlayerStateReference->PlayerState_PlayerParty.Num() <= 0) {
		PlayerStateReference->CreateDefaultPlayerParty();
	}

	// Widget initialization
	CreateBattleWidget();
	CurrentSelectedAvatar->AvatarData = PlayerStateReference->PlayerState_PlayerParty[0];

	if (BattleWidgetReference->IsValidLowLevel()) {
		SetBattleWidgetVariables();
	}

	// Avatar initialization
	CurrentSelectedAvatar->BeginPlayWorkaroundFunction_Implementation(PlayerStateReference->PlayerState_PlayerParty[0], BattleWidgetReference);
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


// ------------------------- Battle
void APlayerController_Base::OnPrimaryClick_Implementation(AActor* ClickedActor)
{
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
						CurrentSelectedAvatar->LaunchAttack_Implementation(CurrentSelectedAvatar->ValidAttackTargetsArray[0]);
					}
				}
			}
		}
		// && PlayerClickMode == E_PlayerCharacter_ClickModes::E_MoveCharacter
		else if (ClickedActor->GetClass()->GetName().Contains("StaticMesh") || ClickedActor->GetClass()->GetName().Contains("GridTile")) {
			// If all else fails, assume we clicked on a plane that we can move our controlled Avatar to
			if (CurrentSelectedAvatar->IsValidLowLevel()) {
				SendMoveCommandToServer_Implementation(ClickedActor->GetActorLocation());
			}
		}
	}
}


void APlayerController_Base::SendMoveCommandToServer_Implementation(FVector MoveLocation)
{
	if (Cast<AAIController_Avatar>(CurrentSelectedAvatar->GetController())) {
		Cast<AAIController_Avatar>(CurrentSelectedAvatar->GetController())->GetMoveCommandFromPlayer_Implementation(MoveLocation);
	}
}


void APlayerController_Base::Server_SubtractHealth_Implementation(ACharacter_Pathfinder* Defender, int DamageDealt)
{
	Cast<AStarmark_GameMode>(GetWorld()->GetAuthGameMode())->Battle_SubtractHealth_Implementation(Defender, DamageDealt);
}


void APlayerController_Base::ChangeActingPlayerState_Implementation(bool NewActingPlayerState)
{
	IsCurrentlyActingPlayer = NewActingPlayerState;
}


void APlayerController_Base::SendEndOfTurnCommandToServer_Implementation()
{
	Cast<AStarmark_GameMode>(GetWorld()->GetAuthGameMode())->EndOfTurn_Implementation();
}