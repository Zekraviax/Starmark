#include "PlayerController_Base.h"


#include "NavigationSystem.h"
#include "AIController.h"
#include "AIController_Avatar.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Widget_HUD_Battle.h"
#include "WidgetComponent_AvatarBattleData.h"
#include "Starmark_GameInstance.h"
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
	DOREPLIFETIME(APlayerController_Base, IsReadyToStartMultiplayerBattle);
	DOREPLIFETIME(APlayerController_Base, MultiplayerUniqueID);
}


void APlayerController_Base::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();
}


void APlayerController_Base::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (CurrentSelectedAvatar)
		SetBattleWidgetAndLinkedAvatar(BattleWidgetReference, CurrentSelectedAvatar->AvatarData);
}


// ------------------------- Widgets
void APlayerController_Base::CreateBattleWidget()
{
	if (BattleWidgetChildClass && IsLocalPlayerController()) {
		BattleWidgetReference = CreateWidget<UWidget_HUD_Battle>(this, BattleWidgetChildClass);

		if (BattleWidgetReference)
			BattleWidgetReference->AddToViewport();
	}
}


void APlayerController_Base::SetBattleWidgetVariables()
{
	AStarmark_GameState* GameStateReference = Cast<AStarmark_GameState>(GetWorld()->GetGameState());

	if (BattleWidgetReference->IsValidLowLevel()) {
		BattleWidgetReference->PlayerControllerReference = this;

		if (CurrentSelectedAvatar)
			BattleWidgetReference->AvatarBattleDataWidget->LinkedAvatar = CurrentSelectedAvatar->AvatarData;

		if (GameStateReference)
			BattleWidgetReference->UpdateTurnOrderText(GameStateReference->CurrentTurnOrderText);
	}
}


void APlayerController_Base::SetBattleWidgetAndLinkedAvatar(UWidget_HUD_Battle* NewBattleWidgetReference, FAvatar_Struct NewAvatarData)
{
	if (NewBattleWidgetReference->IsValidLowLevel()) {
		BattleWidgetReference = NewBattleWidgetReference;

		SetBattleWidgetVariables();
	}

	if (BattleWidgetReference->IsValidLowLevel() && CurrentSelectedAvatar)
		if (BattleWidgetReference->AvatarBattleDataWidget->IsValidLowLevel())
			BattleWidgetReference->AvatarBattleDataWidget->UpdateAvatarData(NewAvatarData);
}


// ------------------------- Avatar
void APlayerController_Base::OnRepNotify_CurrentSelectedAvatar_Implementation()
{
	AStarmark_PlayerState* PlayerStateReference = Cast<AStarmark_PlayerState>(PlayerState);

	// (Default) Player party initialization
	if (PlayerStateReference) {
		PlayerStateReference->PlayerState_BeginBattle();

		// Widget initialization
		if (BattleWidgetReference == NULL)
			CreateBattleWidget();
		
		CurrentSelectedAvatar->AvatarData = PlayerStateReference->PlayerState_PlayerParty[0];	

		// Avatar initialization
		CurrentSelectedAvatar->BeginPlayWorkaroundFunction_Implementation(PlayerStateReference->PlayerState_PlayerParty[0], BattleWidgetReference);

		//
		Server_SetReadyToStartMultiplayerBattle();
	}
	else {
		GetWorld()->GetTimerManager().SetTimer(PlayerStateTimerHandle, this, &APlayerController_Base::OnRepNotify_CurrentSelectedAvatar, 0.5f, false);
	}
}


void APlayerController_Base::UpdateAvatarsDecalsAndWidgets_Implementation(ACharacter_Pathfinder* CurrentlyActingAvatar)
{
	LocalUpdateAvatarsDecals(CurrentlyActingAvatar);
}


void APlayerController_Base::Server_SetReadyToStartMultiplayerBattle_Implementation()
{
	IsReadyToStartMultiplayerBattle = true;
}


void APlayerController_Base::LocalUpdateAvatarsDecals(ACharacter_Pathfinder* CurrentlyActingAvatar)
{
	TArray<AActor*> Avatars;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter_Pathfinder::StaticClass(), Avatars);

	for (int i = 0; i < Avatars.Num(); i++) {
		ACharacter_Pathfinder* FoundActor = Cast<ACharacter_Pathfinder>(Avatars[i]);

		if (FoundActor->AvatarBattleDataComponent_Reference->IsValidLowLevel())
			FoundActor->AvatarBattleDataComponent_Reference->SetVisibility(ESlateVisibility::Collapsed);

		if (FoundActor == CurrentlyActingAvatar) {
			FoundActor->ActorSelected->SetVisibility(true);

			if (FoundActor->MultiplayerControllerUniqueID == MultiplayerUniqueID)
				FoundActor->ActorSelected_DynamicMaterial_Colour = FLinearColor::Green;
			else
				FoundActor->ActorSelected_DynamicMaterial_Colour = FLinearColor::Red;
		}
		else
			FoundActor->ActorSelected->SetVisibility(false);

		FoundActor->ActorSelectedDynamicMaterialColourUpdate();
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
}


void APlayerController_Base::SendEndOfTurnCommandToServer_Implementation()
{
	Cast<AStarmark_GameState>(GetWorld()->GetGameState())->AvatarEndTurn_Implementation();
}