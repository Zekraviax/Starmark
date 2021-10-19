#include "PlayerController_Base.h"


#include "Actor_GridTile.h"
#include "AIController.h"
#include "AIController_Avatar.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "NavigationSystem.h"
#include "PlayerPawn_Static.h"
#include "Player_SaveData.h"
#include "Starmark_GameMode.h"
#include "Starmark_GameInstance.h"
#include "Starmark_GameState.h"
#include "Starmark_PlayerState.h"
#include "Widget_HUD_Battle.h"
#include "WidgetComponent_AvatarBattleData.h"


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
	DOREPLIFETIME(APlayerController_Base, PlayerName);
}


void APlayerController_Base::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();
}


void APlayerController_Base::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	SetBattleWidgetVariables();
}


// ------------------------- Widgets
void APlayerController_Base::CreateBattleWidget()
{
	if (BattleWidgetChildClass && IsLocalPlayerController() && !BattleWidgetReference) {
		BattleWidgetReference = CreateWidget<UWidget_HUD_Battle>(this, BattleWidgetChildClass);

		if (IsValid(BattleWidgetReference))
			BattleWidgetReference->AddToViewport();
	}
}


void APlayerController_Base::SetBattleWidgetVariables()
{
	if (IsValid(BattleWidgetReference) && IsValid(CurrentSelectedAvatar)) {
		if (BattleWidgetReference->PlayerControllerReference != this)
			BattleWidgetReference->PlayerControllerReference = this;

		BattleWidgetReference->AvatarBattleDataWidget->UpdateAvatarData(CurrentSelectedAvatar->AvatarData);
		BattleWidgetReference->UpdateAvatarAttacksComponents();
		BattleWidgetReference->AvatarBattleDataWidget->GetAvatarStatusEffects(CurrentSelectedAvatar->CurrentStatusEffectsArray);
	}
}


void APlayerController_Base::Client_GetTurnOrderText_Implementation(const FString& NewTurnOrderText)
{
	Local_GetTurnOrderText(NewTurnOrderText);
}


void APlayerController_Base::Local_GetTurnOrderText(const FString & NewTurnOrderText)
{
	BattleWidgetReference->UpdateTurnOrderText(NewTurnOrderText);
}


// ------------------------- Avatar
void APlayerController_Base::OnRepNotify_CurrentSelectedAvatar_Implementation()
{
	AStarmark_PlayerState* PlayerStateReference = Cast<AStarmark_PlayerState>(PlayerState);

	// (Default) Player party initialization
	if (PlayerStateReference) {
		PlayerStateReference->PlayerState_BeginBattle();

		// Avatar initialization
		if (CurrentSelectedAvatar) {
			CurrentSelectedAvatar->BeginPlayWorkaroundFunction_Implementation(BattleWidgetReference);

			// Widget initialization
			if (!IsValid(BattleWidgetReference))
				CreateBattleWidget();

			if (IsValid(BattleWidgetReference))
				SetBattleWidgetVariables();

			if (!IsReadyToStartMultiplayerBattle)
				Server_SetReadyToStartMultiplayerBattle();
		} else {
			GetWorld()->GetTimerManager().SetTimer(PlayerStateTimerHandle, this, &APlayerController_Base::OnRepNotify_CurrentSelectedAvatar, 0.2f, false);
		}
	}
	else {
		GetWorld()->GetTimerManager().SetTimer(PlayerStateTimerHandle, this, &APlayerController_Base::OnRepNotify_CurrentSelectedAvatar, 0.2f, false);
	}
}


void APlayerController_Base::Server_SetReadyToStartMultiplayerBattle_Implementation()
{
	IsReadyToStartMultiplayerBattle = true;
}


// ------------------------- Battle
void APlayerController_Base::Server_GetDataFromProfile_Implementation()
{
	UStarmark_GameInstance* GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	PlayerProfileReference = GameInstanceReference->CurrentProfileReference;
	PlayerName = GameInstanceReference->PlayerName;
	PlayerParty = GameInstanceReference->CurrentProfileReference->CurrentAvatarTeam;

	UE_LOG(LogTemp, Warning, TEXT("Server_GetDataFromProfile / IsValid(PlayerProfileReference) returns: %s"), IsValid(PlayerProfileReference) ? TEXT("true") : TEXT("false"));
	UE_LOG(LogTemp, Warning, TEXT("Server_GetDataFromProfile / PlayerName is: %s"), *PlayerName);
}


void APlayerController_Base::OnPrimaryClick(AActor* ClickedActor)
{
	if (CurrentSelectedAvatar->CurrentSelectedAttack.Name != "Default" &&
		CurrentSelectedAvatar->ValidAttackTargetsArray.Num() > 0) {
		if (ClickedActor &&
			CurrentSelectedAvatar->CurrentSelectedAttack.AttackTargetsInRange == EBattle_AttackTargetsInRange::E_AttackClickedAvatar) {
			// If we're attacking, and we clicked on a valid target in-range, launch an attack
			UE_LOG(LogTemp, Warning, TEXT("OnPrimaryClick / Launch attack against ClickedActor"));
			UE_LOG(LogTemp, Warning, TEXT("OnPrimaryClick / ClickedActor is: %s"), *ClickedActor->GetFullName());

			if (Cast<ACharacter_Pathfinder>(ClickedActor)) {
				if (CurrentSelectedAvatar != ClickedActor &&
					CurrentSelectedAvatar->ValidAttackTargetsArray.Contains(ClickedActor)) {
					CurrentSelectedAvatar->LaunchAttack_Implementation(Cast<ACharacter_Pathfinder>(ClickedActor));
					Client_SendEndOfTurnCommandToServer();
				}
			}
		}
		else if (CurrentSelectedAvatar->CurrentSelectedAttack.AttackTargetsInRange == EBattle_AttackTargetsInRange::E_AttackAllTargets) {
			UE_LOG(LogTemp, Warning, TEXT("OnPrimaryClick / Launch attack against all valid targets in range: %d"), CurrentSelectedAvatar->ValidAttackTargetsArray.Num());

			for (int i = 0; i < CurrentSelectedAvatar->ValidAttackTargetsArray.Num(); i++) {
				if (Cast<ACharacter_Pathfinder>(CurrentSelectedAvatar->ValidAttackTargetsArray[i])) {
					UE_LOG(LogTemp, Warning, TEXT("OnPrimaryClick / AttackTarget is: %s"), *CurrentSelectedAvatar->ValidAttackTargetsArray[i]->GetFullName());
					CurrentSelectedAvatar->LaunchAttack_Implementation(Cast<ACharacter_Pathfinder>(CurrentSelectedAvatar->ValidAttackTargetsArray[i]));
				}
			}

			Client_SendEndOfTurnCommandToServer();
		}
		else if (CurrentSelectedAvatar->CurrentSelectedAttack.AttackTargetsInRange == EBattle_AttackTargetsInRange::SelectAllGridTiles) {
			UE_LOG(LogTemp, Warning, TEXT("OnPrimaryClick / Select all valid grid tiles in range: %d"), CurrentSelectedAvatar->ValidAttackTargetsArray.Num());

			for (int i = 0; i < CurrentSelectedAvatar->ValidAttackTargetsArray.Num(); i++) {
				if (Cast<AActor_GridTile>(CurrentSelectedAvatar->ValidAttackTargetsArray[i])) {
					UE_LOG(LogTemp, Warning, TEXT("OnPrimaryClick / GridTile is: %s"), *CurrentSelectedAvatar->ValidAttackTargetsArray[i]->GetFullName());
					CurrentSelectedAvatar->LaunchAttack_Implementation(Cast<AActor_GridTile>(CurrentSelectedAvatar->ValidAttackTargetsArray[i]));
				}
			}

			Client_SendEndOfTurnCommandToServer();
		}

		// Update HUD only if the player clicked on an actor
		if (BattleWidgetReference)
			BattleWidgetReference->OnPlayerClick();
	}
}


void APlayerController_Base::DelayedEndTurn()
{
	GetWorld()->GetTimerManager().SetTimer(PlayerStateTimerHandle, this, &APlayerController_Base::OnRepNotify_CurrentSelectedAvatar, 1.f, false);
}


void APlayerController_Base::SendMoveCommandToServer_Implementation(FVector MoveLocation)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Send Move Command To Server")));
}


void APlayerController_Base::Client_SendEndOfTurnCommandToServer_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Client_SendEndOfTurnCommandToServer / Call SendEndOfTurnCommandToServer()"));
	SendEndOfTurnCommandToServer();
}


void APlayerController_Base::SendEndOfTurnCommandToServer_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("SendEndOfTurnCommandToServer / Call AvatarEndTurn_Implementation()"));
	Cast<AStarmark_GameState>(GetWorld()->GetGameState())->AvatarEndTurn_Implementation();
}


void APlayerController_Base::Player_OnAvatarTurnChanged_Implementation()
{
	TArray<AActor*> GridTilesArray;

	if (IsValid(BattleWidgetReference)) {
		BattleWidgetReference->EndTurnCommandButton->SetIsEnabled(IsCurrentlyActingPlayer);

		//if (IsCurrentlyActingPlayer) {
		//	BattleWidgetReference->AvatarAttacksBox->SetVisibility(ESlateVisibility::Visible);
		//}
		//else {
		//	BattleWidgetReference->AvatarAttacksBox->SetVisibility(ESlateVisibility::Collapsed);
		//}
	}

	CurrentSelectedAvatar->CurrentSelectedAttack.Name = "Default";
	CurrentSelectedAvatar->ValidAttackTargetsArray.Empty();

	SetBattleWidgetVariables();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_GridTile::StaticClass(), GridTilesArray);
	for (int j = 0; j < GridTilesArray.Num(); j++) {
		Cast<AActor_GridTile>(GridTilesArray[j])->DynamicMaterial->SetVectorParameterValue("Colour", FLinearColor::White);
		Cast<AActor_GridTile>(GridTilesArray[j])->ChangeColourOnMouseHover = true;
	}
}


// ------------------------- Multiplayer Battle
void APlayerController_Base::GetAvatarUpdateFromServer_Implementation(ACharacter_Pathfinder* AvatarReference, int AvatarUniqueID, bool IsCurrentlyActing, bool IsCurrentlSelectedAvatar)
{
	if (IsValid(AvatarReference)) {
		LocalAvatarUpdate(AvatarReference, AvatarUniqueID, IsCurrentlyActing, IsCurrentlSelectedAvatar);
	}
}


void APlayerController_Base::LocalAvatarUpdate(ACharacter_Pathfinder* AvatarReference, int AvatarUniqueID, bool IsCurrentlyActing, bool IsCurrentlSelectedAvatar)
{
	ACharacter_Pathfinder* FoundActor = AvatarReference;

	if (AvatarUniqueID == MultiplayerUniqueID)
		FoundActor->ActorSelected_DynamicMaterial_Colour = FLinearColor::Green;
	else
		FoundActor->ActorSelected_DynamicMaterial_Colour = FLinearColor::Red;

	FoundActor->ActorSelectedPlane->SetHiddenInGame(!IsCurrentlyActing);
	FoundActor->ActorSelectedDynamicMaterialColourUpdate();

	SetBattleWidgetVariables();
}


void APlayerController_Base::Client_SendLaunchAttackToServer_Implementation(ACharacter_Pathfinder* Attacker, AActor* Target, const FString& AttackName)
{
	UE_LOG(LogTemp, Warning, TEXT("Client_SendLaunchAttackToServer / Attack chosen: %s"), *AttackName);
	Cast<AStarmark_GameMode>(GetWorld()->GetAuthGameMode())->Server_LaunchAttack(Attacker, Target, AttackName);
}