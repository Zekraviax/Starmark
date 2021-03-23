#include "PlayerController_Lobby.h"

#include "Widget_ServerHost.h"


// ------------------------- Widgets
void APlayerController_Lobby::AddLobbyMenuToScreen()
{
	if (!LobbyWidget_Reference->IsValidLowLevel() && LobbyWidget_Class->IsValidLowLevel() && IsLocalPlayerController()) {
		// Must assign a local controller
		LobbyWidget_Reference = CreateWidget<UWidget_ServerHost>(this, LobbyWidget_Class);
	}

	if (LobbyWidget_Reference->IsValidLowLevel()) {
		LobbyWidget_Reference->AddToViewport();
	}

	SetInputMode(FInputModeGameAndUI());
	bShowMouseCursor = true;
}