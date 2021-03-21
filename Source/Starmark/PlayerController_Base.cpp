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

	// Multiplayer
	bReplicates = true;
}


void APlayerController_Base::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps); 

	DOREPLIFETIME(APlayerController_Base, CurrentSelectedAvatar);
	DOREPLIFETIME(APlayerController_Base, BattleWidgetReference);
	DOREPLIFETIME(APlayerController_Base, IsCurrentlyActingPlayer);
	DOREPLIFETIME(APlayerController_Base, PlayerClickMode);
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
	if (BattleWidgetReference) {
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
	if (PlayerStateReference) {
		PlayerStateReference->PlayerState_BeginBattle();

		// Widget initialization
		CreateBattleWidget();
		CurrentSelectedAvatar->AvatarData = PlayerStateReference->PlayerState_PlayerParty[0];	

		if (BattleWidgetReference) {
			SetBattleWidgetVariables();
		}

		// Avatar initialization
		CurrentSelectedAvatar->BeginPlayWorkaroundFunction_Implementation(PlayerStateReference->PlayerState_PlayerParty[0], BattleWidgetReference);
	}
}


void APlayerController_Base::UpdateAvatarsDecalsAndWidgets()
{
	for (TObjectIterator<ACharacter_Pathfinder> Itr; Itr; ++Itr) {
		ACharacter_Pathfinder* FoundActor = *Itr;

		if (FoundActor->ActorSelected->IsValidLowLevel())
			FoundActor->ActorSelected->SetVisibility(false);

		if (FoundActor->CursorToWorld->IsValidLowLevel())
			FoundActor->CursorToWorld->SetVisibility(false);
	}

	if (CurrentSelectedAvatar->IsValidLowLevel()) {
		//CurrentSelectedAvatar->ActorSelected->SetVisibility(true);
		//CurrentSelectedAvatar->CursorToWorld->SetVisibility(true);

		if (CurrentSelectedAvatar->ActorSelected_DynamicMaterial)
			CurrentSelectedAvatar->ActorSelected_DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor::Green);
	}
}


// ------------------------- Battle
void APlayerController_Base::OnPrimaryClick(AActor* ClickedActor)
{
	if (ClickedActor && IsCurrentlyActingPlayer) {
		if (ClickedActor->GetClass()->GetName().Contains("Character")) {
			// Select Avatar To Control
			//if (CurrentSelectedAvatar != ClickedActor && PlayerClickMode == E_PlayerCharacter_ClickModes::E_SelectCharacterToControl) {
			//	Cast<ACharacter_Pathfinder>(ClickedActor)->OnAvatarClicked();
			//}
			// Select Avatar to Begin Attack
			if (CurrentSelectedAvatar != ClickedActor) {
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Final Damage = %d"), CurrentSelectedAvatar->ValidAttackTargetsArray.Num()));
				// If we're attacking, and we clicked on a valid target in-range, launch an attack
				CurrentSelectedAvatar->LaunchAttack_Implementation(Cast<ACharacter_Pathfinder>(ClickedActor));
			}
		}
		// If all else fails, assume we clicked on a plane that we can move our controlled Avatar to
		else if (ClickedActor->GetClass()->GetName().Contains("StaticMesh") || ClickedActor->GetClass()->GetName().Contains("GridTile")) {
			if (CurrentSelectedAvatar->IsValidLowLevel()) {
				SendMoveCommandToServer_Implementation(ClickedActor->GetActorLocation());
			}
		}
	}
}


void APlayerController_Base::SendMoveCommandToServer_Implementation(FVector MoveLocation)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Send Move Command To Server")));
	//if (Cast<AAIController_Avatar>(CurrentSelectedAvatar->GetController())) {
	//	Cast<AAIController_Avatar>(CurrentSelectedAvatar->GetController())->GetMoveCommandFromPlayer_Implementation(MoveLocation);
	//}
}


void APlayerController_Base::Server_SubtractHealth_Implementation(ACharacter_Pathfinder* Defender, int DamageDealt)
{
	Cast<AStarmark_GameMode>(GetWorld()->GetAuthGameMode())->Battle_SubtractHealth_Implementation(Defender, DamageDealt);
}


void APlayerController_Base::ReceiveChangeActingPlayerStateFromServer_Implementation(bool NewActingPlayerState)
{
	ChangeActingPlayerState(NewActingPlayerState);
}


void APlayerController_Base::ChangeActingPlayerState(bool NewActingPlayerState)
{
	IsCurrentlyActingPlayer = NewActingPlayerState;
}


void APlayerController_Base::SendEndOfTurnCommandToServer_Implementation()
{
	Cast<AStarmark_GameState>(GetWorld()->GetGameState())->AvatarEndTurn_Implementation();
}