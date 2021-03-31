#include "PlayerController_Lobby.h"

#include "Widget_ServerHost.h"


// ------------------------- Widgets
void APlayerController_Lobby::PlayerJoinedMultiplayerLobby_Implementation(int CurrentPlayerCount)
{
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