#include "Widget_ServerBrowser.h"

#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Starmark_GameInstance.h"
#include "WidgetComponent_FoundServer.h"
#include "Widget_MainMenu.h"


// ------------------------- Widget
void UWidget_ServerBrowser::OnExitButtonPressed()
{
	if (!MainMenuWidget_Reference && MainMenuWidget_Class)
		MainMenuWidget_Reference = CreateWidget<UWidget_MainMenu>(this, MainMenuWidget_Class);

	if (MainMenuWidget_Reference) {
		MainMenuWidget_Reference->AddToViewport();
		this->RemoveFromParent();
	}
}


void UWidget_ServerBrowser::PopulateServerBrowserList()
{
	UStarmark_GameInstance* GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (GameInstanceReference->SessionSearch->SearchResults.Num() > 0) {
		FString ServerName;

		for (int i = 0; i < GameInstanceReference->SessionSearch->SearchResults.Num(); i++) {
			FoundServerWidgetComponent_Reference = CreateWidget<UWidgetComponent_FoundServer>(GetWorld(), FoundServerWidgetComponent_Class);

			FoundServerWidgetComponent_Reference->Session = GameInstanceReference->SessionSearch->SearchResults[i];
			FoundServerWidgetComponent_Reference->UserId = GameInstanceReference->SessionSearch->SearchResults[i].Session.OwningUserId;
			FoundServerWidgetComponent_Reference->SessionName = FName(*GameInstanceReference->SessionSearch->SearchResults[i].Session.OwningUserName);

			GameInstanceReference->SessionSearch->SearchResults[i].Session.SessionSettings.Get("CustomLobbyDisplayName", ServerName);

			FoundServerWidgetComponent_Reference->CustomLobbyName = ServerName;
			FoundServerWidgetComponent_Reference->ServerNameText->SetText(FText::FromString(ServerName));

			FoundServersScrollBox->AddChild(FoundServerWidgetComponent_Reference);
			FoundServerWidgetComponent_Reference->SetLobbyName();
		}
	}
}


void UWidget_ServerBrowser::FinishedFindingSessions_Implementation()
{
	// Implemented in blueprints
}