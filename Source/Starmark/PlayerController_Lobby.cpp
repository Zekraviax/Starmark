#include "PlayerController_Lobby.h"

#include "Widget_ServerHost.h"
#include "WidgetComponent_LobbyPlayerVals.h"
#include "Starmark_GameState.h"
#include "Starmark_PlayerState.h"
#include "Player_SaveData.h"


// ------------------------- Widgets
void APlayerController_Lobby::PlayerJoinedMultiplayerLobby_Implementation()
{
	//AStarmark_GameState* GameStateReference = Cast<AStarmark_GameState>(GetWorld()->GetGameState());

	if (!LobbyWidget_Reference->IsValidLowLevel() && LobbyWidget_Class->IsValidLowLevel() && IsLocalPlayerController()) {
		// Must assign a local controller
		LobbyWidget_Reference = CreateWidget<UWidget_ServerHost>(this, LobbyWidget_Class);

		if (GetWorld()->IsServer()) {
			LobbyWidget_Reference->ReadyButtonText->SetText(FText::FromString("Begin Game"));
			LobbyWidget_Reference->ReadyButton->SetIsEnabled(true);

			//if (CurrentPlayerCount <= 1)
			//	LobbyWidget_Reference->ReadyButton->SetIsEnabled(false);
			//else
			//	LobbyWidget_Reference->ReadyButton->SetIsEnabled(true);

		}
	}

	if (LobbyWidget_Reference->IsValidLowLevel()) {
		LobbyWidget_Reference->AddToViewport();
	}

	SetInputMode(FInputModeGameAndUI());
	bShowMouseCursor = true;
}


void APlayerController_Lobby::UpdatePlayersInLobby(TArray<FString> PlayerNames, TArray<FString> PlayerReadyStatuses)
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

		//LobbyPlayerVals_Reference = CreateWidget<UWidgetComponent_LobbyPlayerVals>(this, LobbyPlayerVals_Class);
		//LobbyPlayerVals_Reference->PlayerReadyStatus->SetText(FText::FromString("Ready"));

		//if (Cast<AStarmark_PlayerState>(PlayerState)->PlayerProfileReference->IsValidLowLevel())
		//	LobbyPlayerVals_Reference->PlayerNameText->SetText(FText::FromString(Cast<AStarmark_PlayerState>(PlayerState)->PlayerProfileReference->Name));
		//else
		//	LobbyPlayerVals_Reference->PlayerNameText->SetText(FText::FromString("Default"));

		//LobbyWidget_Reference->PlayerListVerticalBox->AddChild(LobbyPlayerVals_Reference);
	}
}