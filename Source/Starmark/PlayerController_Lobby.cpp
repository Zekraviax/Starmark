#include "PlayerController_Lobby.h"

#include "Kismet/GameplayStatics.h"
#include "Starmark_GameState.h"
#include "Starmark_PlayerState.h"
#include "Player_SaveData.h"
#include "WidgetComponent_LobbyPlayerVals.h"
#include "WidgetComponent_Avatar.h"
#include "Widget_ServerHost.h"


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

		LobbyWidget_Reference->AddToViewport();
	}

	SetInputMode(FInputModeGameAndUI());
	bShowMouseCursor = true;

	//if (LobbyWidget_Reference->IsValidLowLevel())
	//	LobbyWidget_Reference->GetAllPlayerProfiles();

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
				LobbyPlayerVals_Reference->PlayerNameText->SetText(FText::FromString("Default"));

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

		// Update player team
		//TArray<UUserWidget*> FoundAvatarWidgetComponents;
		//UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, FoundAvatarWidgetComponents, UWidgetComponent_Avatar::StaticClass(), true);

		////UStarmark_GameInstance* GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		//Cast<AStarmark_PlayerState>(GetPawn()->GetPlayerState())->PlayerProfileReference->CurrentAvatarTeam.Empty();

		//for (int i = 0; i < FoundAvatarWidgetComponents.Num(); i++) {
		//	UWidgetComponent_Avatar* AvatarWidgetComponentReference = Cast<UWidgetComponent_Avatar>(FoundAvatarWidgetComponents[i]);
		//	if (AvatarWidgetComponentReference->IndexInPlayerTeam >= 0) {
		//		Cast<AStarmark_PlayerState>(GetPawn()->GetPlayerState())->PlayerProfileReference->CurrentAvatarTeam.Insert(AvatarWidgetComponentReference->AvatarData, AvatarWidgetComponentReference->IndexInPlayerTeam);
		//		Cast<AStarmark_PlayerState>(GetPawn()->GetPlayerState())->PlayerProfileReference->AvatarLibrary.Remove(AvatarWidgetComponentReference->AvatarData);
		//	}
		//}

		//Cast<AStarmark_PlayerState>(GetPawn()->GetPlayerState())->SaveToCurrentProfile();

		if (OnAvatarChangedSlotDelegate.IsBound())
			OnAvatarChangedSlotDelegate.Broadcast();

	} else {
		CurrentSelectedAvatarWidgetComponent = SecondClickedAvatarWidgetComponent;
	}
}