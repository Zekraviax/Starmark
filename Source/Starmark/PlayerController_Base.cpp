#include "PlayerController_Base.h"


#include "NavigationSystem.h"
#include "AIController.h"
#include "AIController_Avatar.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Widget_HUD_Battle.h"
#include "WidgetComponent_AvatarBattleData.h"
#include "Starmark_GameState.h"
#include "Starmark_PlayerState.h"
#include "PlayerPawn_Static.h"
#include "Player_SaveData.h"


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
	DOREPLIFETIME(APlayerController_Base, PlayerProfileReference);
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


// ------------------------- Player
void APlayerController_Base::LoadPlayerProfile()
{
	//Cast<AStarmark_GameInstance>()
	USaveGame* SaveGameObject = UGameplayStatics::LoadGameFromSlot("PlayerProfile", 0);

	if (SaveGameObject->IsValidLowLevel()) {
		UPlayer_SaveData* PlayerProfile = Cast<UPlayer_SaveData>(SaveGameObject);

		if (PlayerProfile->IsValidLowLevel()) {
			Cast<AStarmark_PlayerState>(PlayerState)->UpdatePlayerData(PlayerProfile);
		}

		PlayerProfileReference = PlayerProfile;
	}
}


// ------------------------- Avatar
void APlayerController_Base::OnRepNotify_CurrentSelectedAvatar()
{
	AStarmark_PlayerState* PlayerStateReference = Cast<AStarmark_PlayerState>(PlayerState);
	AStarmark_GameState* GameStateReference = Cast<AStarmark_GameState>(PlayerState);

	// (Default) Player party initialization
	if (PlayerStateReference) {
		PlayerStateReference->PlayerState_BeginBattle();

		// Widget initialization
		CreateBattleWidget();
		CurrentSelectedAvatar->AvatarData = PlayerStateReference->PlayerState_PlayerParty[0];	

		if (BattleWidgetReference) {
			SetBattleWidgetVariables();

			BattleWidgetReference->UpdateTurnOrderText(GameStateReference->CurrentTurnOrderText);
		}

		// Avatar initialization
		CurrentSelectedAvatar->BeginPlayWorkaroundFunction_Implementation(PlayerStateReference->PlayerState_PlayerParty[0], BattleWidgetReference);

		UpdateAvatarsDecalsAndWidgets();
	}
	else {
		GetWorld()->GetTimerManager().SetTimer(PlayerStateTimerHandle, this, &APlayerController_Base::OnRepNotify_CurrentSelectedAvatar, 0.5f, false);
	}
}


void APlayerController_Base::UpdateAvatarsDecalsAndWidgets()
{
	for (TObjectIterator<ACharacter_Pathfinder> Itr; Itr; ++Itr) {
		ACharacter_Pathfinder* FoundActor = *Itr;

		if (FoundActor->ActorSelected->IsValidLowLevel())
			FoundActor->ActorSelected->SetVisibility(false);

		if (FoundActor->AvatarBattleDataComponent_Reference->IsValidLowLevel())
			FoundActor->AvatarBattleDataComponent_Reference->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (CurrentSelectedAvatar->IsValidLowLevel()) {
		if (CurrentSelectedAvatar->ActorSelected_DynamicMaterial)
			CurrentSelectedAvatar->ActorSelected_DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor::Green);

		CurrentSelectedAvatar->ActorSelected->SetVisibility(true);
	}
}


// ------------------------- Battle
void APlayerController_Base::OnPrimaryClick(AActor* ClickedActor)
{
	if (ClickedActor) {
		if (ClickedActor->GetClass()->GetName().Contains("Character")) {
			// Select Avatar To Control
			//if (CurrentSelectedAvatar != ClickedActor && PlayerClickMode == E_PlayerCharacter_ClickModes::E_SelectCharacterToControl) {
			//	Cast<ACharacter_Pathfinder>(ClickedActor)->OnAvatarClicked();
			//}
			// Select Avatar to Begin Attack
			if (CurrentSelectedAvatar != ClickedActor) {
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

		// Update HUD only if the player clicked on an actor
		if (BattleWidgetReference)
			BattleWidgetReference->OnPlayerClick();
	}
}


void APlayerController_Base::SendMoveCommandToServer_Implementation(FVector MoveLocation)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Send Move Command To Server")));
	//if (Cast<AAIController_Avatar>(CurrentSelectedAvatar->GetController())) {
	//	Cast<AAIController_Avatar>(CurrentSelectedAvatar->GetController())->GetMoveCommandFromPlayer_Implementation(MoveLocation);
	//}
}


void APlayerController_Base::ReceiveChangeActingPlayerStateFromServer_Implementation(bool NewActingPlayerState)
{
	AStarmark_GameState* GameStateReference = Cast<AStarmark_GameState>(GetWorld()->GetGameState());

	ChangeActingPlayerState(NewActingPlayerState);
}


void APlayerController_Base::ChangeActingPlayerState(bool NewActingPlayerState)
{
	IsCurrentlyActingPlayer = NewActingPlayerState;

	if (BattleWidgetReference)
		BattleWidgetReference->OnPlayerCurrentlyActingStateChanged();
}


void APlayerController_Base::SendEndOfTurnCommandToServer_Implementation()
{
	Cast<AStarmark_GameState>(GetWorld()->GetGameState())->AvatarEndTurn_Implementation();
}