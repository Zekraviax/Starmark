#include "PlayerController_Lobby.h"

#include "Widget_ServerHost.h"
#include "WidgetComponent_LobbyPlayerVals.h"
#include "Starmark_GameState.h"
#include "Starmark_PlayerState.h"
#include "Player_SaveData.h"


// ------------------------- Lobby
void APlayerController_Lobby::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	//Cast<AStarmark_PlayerState>(this->PlayerState)->UpdatePlayerData();
}


// ------------------------- Widgets
void APlayerController_Lobby::PlayerJoinedMultiplayerLobby_Implementation()
{
	AStarmark_GameState* GameStateReference = Cast<AStarmark_GameState>(GetWorld()->GetGameState());

	if (!LobbyWidget_Reference->IsValidLowLevel() && LobbyWidget_Class->IsValidLowLevel() && IsLocalPlayerController()) {
		// Must assign a local controller
		LobbyWidget_Reference = CreateWidget<UWidget_ServerHost>(this, LobbyWidget_Class);

		if (GetWorld()->IsServer()) {
			LobbyWidget_Reference->ReadyButtonText->SetText(FText::FromString("Begin Game"));
			LobbyWidget_Reference->ReadyButton->SetIsEnabled(true);

			Cast<AStarmark_PlayerState>(GetPawn()->GetPlayerState())->PlayerReadyStatus = "Host";
		}

		LobbyWidget_Reference->AddToViewport();
	}

	SetInputMode(FInputModeGameAndUI());
	bShowMouseCursor = true;

	if (LobbyWidget_Reference->IsValidLowLevel())
		LobbyWidget_Reference->GetAllPlayerProfiles();

	//Cast<AStarmark_PlayerState>(this->PlayerState)->SendUpdateToMultiplayerLobby();
}


void APlayerController_Lobby::Client_UpdatePlayersInLobby_Implementation(const TArray<FString>& PlayerNames, const TArray<FString>& PlayerReadyStatuses, bool ClearCurrentPlayers)
{
	//for (int i = 0; i < PlayerNames.Num(); i++) {
	//	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Player Name: %s"), *PlayerNames[i]));
	//}

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
				LobbyPlayerVals_Reference->PlayerNameText->SetText(FText::FromString("Default"));

			LobbyWidget_Reference->PlayerListVerticalBox->AddChild(LobbyPlayerVals_Reference);
		}
	}
}