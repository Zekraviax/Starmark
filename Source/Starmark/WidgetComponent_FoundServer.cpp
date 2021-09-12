#include "WidgetComponent_FoundServer.h"

#include "Kismet/GameplayStatics.h"
#include "OnlineSessionSettings.h"
#include "Starmark_GameInstance.h"


void UWidgetComponent_FoundServer::OnJoinButtonPressed()
{
	UStarmark_GameInstance* GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	GameInstanceReference->JoinSession(UserId, SessionName, Session);
}