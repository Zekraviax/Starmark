#include "Starmark_GameInstance.h"

#include "Actor_GridTile.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "SaveData_PlayerProfilesList.h"
#include "WidgetComponent_FoundServer.h"
#include "Widget_ServerBrowser.h"


UStarmark_GameInstance::UStarmark_GameInstance(const FObjectInitializer& ObjectInitializer)
{
	/** Bind function for CREATING a Session */
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UStarmark_GameInstance::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UStarmark_GameInstance::OnStartOnlineGameComplete);

	/** Bind function for FINDING a Session */
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UStarmark_GameInstance::OnFindSessionsComplete);

	/** Bind function for JOINING a Session */
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UStarmark_GameInstance::OnJoinSessionComplete);

	/** Bind function for DESTROYING a Session */
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UStarmark_GameInstance::OnDestroySessionComplete);
}


// ------------------------- Player
void UStarmark_GameInstance::LoadProfile(FString ProfileName)
{
	USaveData_PlayerProfilesList* SaveGameObject = Cast<USaveData_PlayerProfilesList>(UGameplayStatics::LoadGameFromSlot("PlayerProfilesList", 0));

	for (int i = 0; i < SaveGameObject->PlayerProfileNames.Num(); i++) {
		if (SaveGameObject->PlayerProfileNames[i] == ProfileName) {
			CurrentProfileReference = Cast<UPlayer_SaveData>(UGameplayStatics::LoadGameFromSlot(ProfileName, 0));

			PlayerName = CurrentProfileReference->Name;
			CurrentProfileName = ProfileName;

			// Set data in the players' controller
			//GetPlayer
			break;
		}
	}
}


void UStarmark_GameInstance::DeleteProfile(FString ProfileName)
{
	USaveData_PlayerProfilesList* SaveGameObject = Cast<USaveData_PlayerProfilesList>(UGameplayStatics::LoadGameFromSlot("PlayerProfilesList", 0));

	for (int i = SaveGameObject->PlayerProfileNames.Num() - 1; i >= 0; i--) {
		if (SaveGameObject->PlayerProfileNames[i] == ProfileName) {
			UGameplayStatics::DeleteGameInSlot(ProfileName, 0);
			SaveGameObject->PlayerProfileNames.Remove(ProfileName);

			UGameplayStatics::SaveGameToSlot(SaveGameObject, "PlayerProfilesList", 0);
		}
	}
}


AActor_GridTile* UStarmark_GameInstance::FindTileByCoordinates(int x, int y)
{
	TArray<AActor*> GridTilesArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor_GridTile::StaticClass(), GridTilesArray);
	AActor_GridTile* GridTile = nullptr;

	for (AActor* Tile : GridTilesArray) {
		GridTile = Cast<AActor_GridTile>(Tile);
		if (Tile->GetActorLocation().X / 200 == x && Tile->GetActorLocation().Y / 200 == y) {
			return GridTile;
		}
	}

	return nullptr;
}



// Networking
// --------------------------------------------------
bool UStarmark_GameInstance::HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers, FString CustomLobbyName)
{
	// Get the Online Subsystem to work with
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub) {
		// Get the Session Interface, so we can call the "CreateSession" function on it
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid() && UserId.IsValid()) {
			/*
			Fill in all the Session Settings that we want to use.

			There are more with SessionSettings.Set(...);
			For example the Map or the GameMode/Type.
			*/
			SessionSettings = MakeShareable(new FOnlineSessionSettings());

			SessionSettings->bIsLANMatch = bIsLAN;
			SessionSettings->bUsesPresence = bIsPresence;
			SessionSettings->NumPublicConnections = 5;
			SessionSettings->NumPrivateConnections = 0;
			SessionSettings->bAllowInvites = true;
			SessionSettings->bAllowJoinInProgress = true;
			SessionSettings->bShouldAdvertise = true;
			SessionSettings->bAllowJoinViaPresence = true;
			SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;

			SessionSettings->Set(SETTING_MAPNAME, FString("NewMap"), EOnlineDataAdvertisementType::ViaOnlineService);

			// Add custom SessionDisplayName setting
			FString Name = CustomLobbyName;
			SessionSettings->Set(TEXT("CustomLobbyDisplayName"), Name, EOnlineDataAdvertisementType::ViaOnlineService);

			// Set the delegate to the Handle of the SessionInterface
			OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

			// Our delegate should get called when this is complete (doesn't need to be successful!)
			return Sessions->CreateSession(*UserId, SessionName, *SessionSettings);
		}
	} else {
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("No OnlineSubsytem found!"));
	}

	return false;
}


void UStarmark_GameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnCreateSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	// Get the OnlineSubsystem so we can get the Session Interface
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get the Session Interface to call the StartSession function
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			// Clear the SessionComplete delegate handle, since we finished this call
			Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
			if (bWasSuccessful)
			{
				// Set the StartSession delegate handle
				OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);

				// Our StartSessionComplete delegate should get called after this
				Sessions->StartSession(SessionName);
			}
		}

	}
}


void UStarmark_GameInstance::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnStartSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	// Get the Online Subsystem so we can get the Session Interface
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub) {
		// Get the Session Interface to clear the Delegate
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid()) {
			// Clear the delegate, since we are done with this call
			Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
		}
	}

	// If the start was successful, we can open a NewMap if we want. Make sure to use "listen" as a parameter!
	if (bWasSuccessful) {
		UGameplayStatics::OpenLevel(GetWorld(), "MultiplayerLobby", true, "listen");
		//GetWorld()->ServerTravel("/Game/Levels/MultiplayerLobby.MultiplayerLobby?listen");
		//World'/Game/Levels/MultiplayerLobby.MultiplayerLobby'
	}
}


void UStarmark_GameInstance::FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence)
{
	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("UStarmark_GameInstance / FindSessions /  if (OnlineSub) returns: %s"), OnlineSub ? TEXT("true") : TEXT("false")));
	UE_LOG(LogTemp, Warning, TEXT("UStarmark_GameInstance / FindSessions / if (OnlineSub) returns: %s"), OnlineSub ? TEXT("true") : TEXT("false"));

	if (OnlineSub)
	{
		// Get the SessionInterface from our OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("UStarmark_GameInstance / FindSessions / Sessions.IsValid returns: %s"), Sessions.IsValid() ? TEXT("true") : TEXT("false")));
		UE_LOG(LogTemp, Warning, TEXT("FindSessions / Sessions.IsValid returns: %s"), Sessions.IsValid() ? TEXT("true") : TEXT("false"));

		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("UStarmark_GameInstance / FindSessions / UserId.IsValid returns: %s"), UserId.IsValid() ? TEXT("true") : TEXT("false")));
		UE_LOG(LogTemp, Warning, TEXT("FindSessions / UserId.IsValid returns: %s"), UserId.IsValid() ? TEXT("true") : TEXT("false"));

		if (Sessions.IsValid() && UserId.IsValid()) {
			/*
			Fill in all the SearchSettings, like if we are searching for a LAN game and how many results we want to have!
			*/
			SessionSearch = MakeShareable(new FOnlineSessionSearch());

			// Steam setting?
			bIsPresence = true;

			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("UStarmark_GameInstance / FindSessions / bIsLAN is: %s"), bIsLAN ? TEXT("true") : TEXT("false")));
			UE_LOG(LogTemp, Warning, TEXT("UStarmark_GameInstance / FindSessions / bIsLAN is: %s"), bIsLAN ? TEXT("true") : TEXT("false"));

			SessionSearch->bIsLanQuery = bIsLAN;
			SessionSearch->MaxSearchResults = 100;
			SessionSearch->PingBucketSize = 50;
			//SessionSearch->TimeoutInSeconds = 30.f;

			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("UStarmark_GameInstance / FindSessions / bIsPresence is: %s"), bIsPresence ? TEXT("true") : TEXT("false")));
			UE_LOG(LogTemp, Warning, TEXT("UStarmark_GameInstance / FindSessions / bIsPresence is: %s"), bIsPresence ? TEXT("true") : TEXT("false"));

			// We only want to set this Query Setting if "bIsPresence" is true
			if (bIsPresence) {
				SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals);
			}

			TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SessionSearch.ToSharedRef();

			// Set the Delegate to the Delegate Handle of the FindSession function
			OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

			// Finally call the SessionInterface function. The Delegate gets called once this is finished
			Sessions->FindSessions(*UserId, SearchSettingsRef);
		}
	} else {
		// If something goes wrong, just call the Delegate Function directly with "false".
		OnFindSessionsComplete(false);
	}
}


void UStarmark_GameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("UStarmark_GameInstance / OnFindSessionsComplete / bWasSuccessfull: %d"), bWasSuccessful));
	UE_LOG(LogTemp, Warning, TEXT("UStarmark_GameInstance / OnFindSessionsComplete / bWasSuccessfull is: %s"), bWasSuccessful ? TEXT("true") : TEXT("false"));

	// Get OnlineSubsystem we want to work with
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("UStarmark_GameInstance / OnFindSessionsComplete / if (OnlineSub) returns: %s"), OnlineSub ? TEXT("true") : TEXT("false")));
	UE_LOG(LogTemp, Warning, TEXT("UStarmark_GameInstance / OnFindSessionsComplete / if (OnlineSub) returns: %s"), OnlineSub ? TEXT("true") : TEXT("false"));

	if (OnlineSub) {
		// Get SessionInterface of the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("UStarmark_GameInstance / OnFindSessionsComplete / Sessions.IsValid returns: %s"), Sessions.IsValid() ? TEXT("true") : TEXT("false")));
		UE_LOG(LogTemp, Warning, TEXT("UStarmark_GameInstance / OnFindSessionsComplete / Sessions.IsValid returns: %s"), Sessions.IsValid() ? TEXT("true") : TEXT("false"));

		if (Sessions.IsValid()) {
			// Clear the Delegate handle, since we finished this call
			Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

			// Just debugging the Number of Search results. Can be displayed in UMG or something later on
			//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Num Search Results: %d"), SessionSearch->SearchResults.Num()));

			// If we have found at least 1 session, we just going to debug them. You could add them to a list of UMG Widgets, like it is done in the BP version!
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("UStarmark_GameInstance / OnFindSessionsComplete / Sessions found: %d"), SessionSearch->SearchResults.Num()));
			UE_LOG(LogTemp, Warning, TEXT("UStarmark_GameInstance / OnFindSessionsComplete / Sessions found: %d"), SessionSearch->SearchResults.Num());


			if (SessionSearch->SearchResults.Num() > 0)
			{
				FString ServerName;

				// "SessionSearch->SearchResults" is an Array that contains all the information. You can access the Session in this and get a lot of information.
				// This can be customized later on with your own classes to add more information that can be set and displayed
				for (int32 SearchIdx = 0; SearchIdx < SessionSearch->SearchResults.Num(); SearchIdx++)
				{
					// OwningUserName is just the SessionName for now. I guess you can create your own Host Settings class and GameSession Class and add a proper GameServer Name here.
					// This is something you can't do in Blueprint for example!
					//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Session Number: %d | Session Name: %s "), SearchIdx + 1, *(SessionSearch->SearchResults[SearchIdx].Session.OwningUserName)));

					SessionSearch->SearchResults[SearchIdx].Session.SessionSettings.Get("CustomLobbyDisplayName", ServerName);
				}

				// If there are any ServerBrowser widgets, call the FinishedFindingSessions function
				TArray<UUserWidget*> FoundServerBrowserWidgets;
				UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, FoundServerBrowserWidgets, UWidget_ServerBrowser::StaticClass(), true);
				for (int i = 0; i < FoundServerBrowserWidgets.Num(); i++) {
					Cast<UWidget_ServerBrowser>(FoundServerBrowserWidgets[i])->FinishedFindingSessions();

					for (int j = 0; j < SessionSearch->SearchResults.Num(); j++) {
						Cast<UWidget_ServerBrowser>(FoundServerBrowserWidgets[i])->PopulateServerBrowserList();
					}
				}
			}
		}
	}
}


bool UStarmark_GameInstance::JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult & SearchResult)
{
	// Return bool
	bool bSuccessful = false;

	// Get OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		// Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid() && UserId.IsValid())
		{
			// Set the Handle again
			OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

			// Call the "JoinSession" Function with the passed "SearchResult". The "SessionSearch->SearchResults" can be used to get such a
			// "FOnlineSessionSearchResult" and pass it. Pretty straight forward!
			bSuccessful = Sessions->JoinSession(*UserId, SessionName, SearchResult);
		}
	}

	return bSuccessful;
}


void UStarmark_GameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnJoinSessionComplete %s, %d"), *SessionName.ToString(), static_cast<int32>(Result)));

	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			// Clear the Delegate again
			Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);

			// Get the first local PlayerController, so we can call "ClientTravel" to get to the Server Map
			// This is something the Blueprint Node "Join Session" does automatically!
			APlayerController * const PlayerController = GetFirstLocalPlayerController();

			// We need a FString to use ClientTravel and we can let the SessionInterface contruct such a
			// String for us by giving him the SessionName and an empty String. We want to do this, because
			// Every OnlineSubsystem uses different TravelURLs
			FString TravelURL;

			if (PlayerController && Sessions->GetResolvedConnectString(SessionName, TravelURL))
			{
				// Finally call the ClienTravel. If you want, you could print the TravelURL to see
				// how it really looks like
				PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}


void UStarmark_GameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("UStarmark_GameInstance / OnDestroySessionComplete / Destroy session complete? %s, %d"), *SessionName.ToString(), bWasSuccessful));

	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		// Get the SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			// Clear the Delegate
			Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);

			// If it was successful, we just load another level (could be a MainMenu!)
			if (bWasSuccessful)
			{
				UGameplayStatics::OpenLevel(GetWorld(), "ThirdPersonExampleMap", true);
			}
		}
	}
}


void UStarmark_GameInstance::StartOnlineGame(FString CustomLobbyName)
{
	// Creating a local player where we can get the UserID from
	ULocalPlayer* const Player = GetFirstGamePlayer();

	// Call our custom HostSession function. GameSessionName is a GameInstance variable
	HostSession(Player->GetPreferredUniqueNetId().GetUniqueNetId(), GameSessionName, false, true, 4, CustomLobbyName);
}


void UStarmark_GameInstance::FindOnlineGames(bool UseLAN)
{
	ULocalPlayer* const Player = GetFirstGamePlayer();

	FindSessions(Player->GetPreferredUniqueNetId().GetUniqueNetId(), UseLAN, true);
}


void UStarmark_GameInstance::JoinOnlineGame()
{
	ULocalPlayer* const Player = GetFirstGamePlayer();

	// Just a SearchResult where we can save the one we want to use, for the case we find more than one!
	FOnlineSessionSearchResult SearchResult;

	// If the Array is not empty, we can go through it
	if (SessionSearch->SearchResults.Num() > 0)
	{
		for (int32 i = 0; i < SessionSearch->SearchResults.Num(); i++)
		{
			// To avoid something crazy, we filter sessions from ourself
			if (SessionSearch->SearchResults[i].Session.OwningUserId != Player->GetPreferredUniqueNetId())
			{
				SearchResult = SessionSearch->SearchResults[i];

				// Once we found sounce a Session that is not ours, just join it. Instead of using a for loop, you could
				// use a widget where you click on and have a reference for the GameSession it represents which you can use
				// here
				JoinSession(Player->GetPreferredUniqueNetId().GetUniqueNetId(), GameSessionName, SearchResult);
				break;
			}
		}
	}
}


void UStarmark_GameInstance::DestroySessionAndLeaveGame()
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);

			Sessions->DestroySession(GameSessionName);
		}
	}
}
