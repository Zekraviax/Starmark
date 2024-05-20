#include "PlayerController_Lobby.h"

#include "Kismet/GameplayStatics.h"
#include "Starmark_GameInstance.h"
#include "Starmark_GameState.h"
#include "Starmark_PlayerState.h"
#include "Player_SaveData.h"
#include "WidgetComponent_LobbyPlayerVals.h"
#include "WidgetComponent_Avatar.h"
#include "Widget_ServerHost.h"


void APlayerController_Lobby::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerController_Lobby, PlayerDataStruct);
}


// ------------------------- Lobby
void APlayerController_Lobby::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}


// ------------------------- Widgets
void APlayerController_Lobby::PlayerJoinedMultiplayerLobby_Implementation()
{
	AStarmark_GameState* GameStateReference = Cast<AStarmark_GameState>(GetWorld()->GetGameState());

	if (!LobbyWidget_Reference->IsValidLowLevel() && LobbyWidget_Class->IsValidLowLevel() && IsLocalPlayerController()) {
		// Must assign a local controller
		LobbyWidget_Reference = CreateWidget<UWidget_ServerHost>(this, LobbyWidget_Class);
		LobbyWidget_Reference->OnWidgetOpened();

		if (GetWorld()->IsServer()) {
			LobbyWidget_Reference->ReadyButtonText->SetText(FText::FromString("Begin Game"));
			LobbyWidget_Reference->ReadyButton->SetIsEnabled(true);

			Cast<AStarmark_PlayerState>(GetPawn()->GetPlayerState())->PlayerReadyStatus = "Host";
		}

		// Get the player's data from their GameInstance
		UStarmark_GameInstance* GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		PlayerDataStruct = GameInstanceReference->PlayerData;

		// To-Do: Figure out why this crashes clients
		// Can't get the PlayerState here?
		//PlayerDataStruct = Cast<AStarmark_PlayerState>(GetPawn()->GetPlayerState())->PlayerDataStruct;
		//Cast<AStarmark_PlayerState>(GetPawn()->GetPlayerState())->UpdatePlayerData();

		// Send the number of expected players to the GameState
		// // To-Do: Figure out how to do this
		//GameStateReference->MultiplayerBattleExpectedPlayers = 2;

		LobbyWidget_Reference->AddToViewport();

		UE_LOG(LogTemp, Warning, TEXT("APlayerController_Lobby / PlayerJoinedMultiplayerLobby / Player joined: %s"), *PlayerDataStruct.PlayerName);
	} else {
		UE_LOG(LogTemp, Warning, TEXT("APlayerController_Lobby / PlayerJoinedMultiplayerLobby / Error when player attempted to join the lobby."));
	}

	SetInputMode(FInputModeGameAndUI());
	bShowMouseCursor = true;
}


void APlayerController_Lobby::Client_UpdatePlayersInLobby_Implementation(const TArray<FString>& PlayerNames, const TArray<FString>& PlayerReadyStatuses, bool ClearCurrentPlayers)
{
	UpdatePlayersInLobby(PlayerNames, PlayerReadyStatuses, ClearCurrentPlayers);
}


void APlayerController_Lobby::UpdatePlayersInLobby(TArray<FString> PlayerNames, TArray<FString> PlayerReadyStatuses, bool ClearCurrentPlayers)
{
	if (LobbyWidget_Reference->IsValidLowLevel()) {
		LobbyWidget_Reference->PlayerListVerticalBox->ClearChildren();

		for (int i = 0; i < PlayerReadyStatuses.Num(); i++) {
			LobbyPlayerVals_Reference = CreateWidget<UWidgetComponent_LobbyPlayerVals>(this, LobbyPlayerVals_Class);

			LobbyPlayerVals_Reference->PlayerReadyStatus->SetText(FText::FromString(PlayerReadyStatuses[i]));

			if (PlayerNames.IsValidIndex(i)) 
				LobbyPlayerVals_Reference->PlayerNameText->SetText(FText::FromString(PlayerNames[i]));
			else
				LobbyPlayerVals_Reference->PlayerNameText->SetText(FText::FromString("None"));

			LobbyWidget_Reference->PlayerListVerticalBox->AddChild(LobbyPlayerVals_Reference);
		}
	}
}


void APlayerController_Lobby::OnAvatarWidgetComponentClicked(UWidgetComponent_Avatar* SecondClickedAvatarWidgetComponent)
{
	if (CurrentSelectedAvatarWidgetComponent->IsValidLowLevel()) {
		// Data that was stored on the first clicked AvatarWidgetComponent
		FAvatar_Struct TemporaryAvatarData = CurrentSelectedAvatarWidgetComponent->AvatarData;
		UMaterial* TemporaryAvatarMaterial = CurrentSelectedAvatarWidgetComponent->AvatarMaterial;
		FText TemporaryAvatarName = CurrentSelectedAvatarWidgetComponent->AvatarName->GetText();

		// Swap index in player library

		// First swap: From the Second WidgetComponent to the First
		CurrentSelectedAvatarWidgetComponent->AvatarData = SecondClickedAvatarWidgetComponent->AvatarData;
		CurrentSelectedAvatarWidgetComponent->AvatarMaterial = SecondClickedAvatarWidgetComponent->AvatarMaterial;
		CurrentSelectedAvatarWidgetComponent->AvatarName->SetText(SecondClickedAvatarWidgetComponent->AvatarName->GetText());

		// Second swap: From the First (temporary variables) to the Second
		SecondClickedAvatarWidgetComponent->AvatarData = TemporaryAvatarData;
		SecondClickedAvatarWidgetComponent->AvatarMaterial = TemporaryAvatarMaterial;
		SecondClickedAvatarWidgetComponent->AvatarName->SetText(TemporaryAvatarName);

		// Update widget materials
		CurrentSelectedAvatarWidgetComponent->UpdateWidgetMaterials();
		SecondClickedAvatarWidgetComponent->UpdateWidgetMaterials();

		CurrentSelectedAvatarWidgetComponent = NULL;

		if (OnAvatarChangedSlotDelegate.IsBound())
			OnAvatarChangedSlotDelegate.Broadcast();

	} else {
		CurrentSelectedAvatarWidgetComponent = SecondClickedAvatarWidgetComponent;
	}
}