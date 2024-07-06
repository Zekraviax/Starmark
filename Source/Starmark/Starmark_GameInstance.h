#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Interfaces/OnlineSessionInterface.h"
#include "SaveData_DeveloperSettings.h"
#include "Starmark_Variables.h"

#include "Starmark_GameInstance.generated.h"


// Forward Declarations
class AActor_GridTile;
class USaveData_PlayerProfile;


// The GameInstance is created when the game is first launched, and is not destroyed until the game is closed.
// It persists from level to level, regardless of travel type (Seamless or Hard).
// That means this is a good place to store data that needs to be persistent, except in multiplayer scenarios.
// The GameInstance is not replicated at all. So for all replicated variables, use the PlayerState.
UCLASS()
class STARMARK_API UStarmark_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UStarmark_GameInstance(const FObjectInitializer& ObjectInitializer);

// Variables
// --------------------------------------------------

// ------------------------- Global Helper Variables
	UPROPERTY()
	USaveData_DeveloperSettings* DevSettingsSaveFile;

	// Putting DataTables here, so they're all in one place and easy to access..
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* AvatarsDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* SimpleAttacksDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* ComplexAttacksDataTable;

// ------------------------- Player
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USaveData_PlayerProfile* PlayerSaveGameReference;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPlayer_Data PlayerDataStruct;


// Functions
// --------------------------------------------------

// ------------------------- Global Helper Functions
	static bool DoesSessionExist();
	static FOnlineSessionSettings* GetCurrentSessionSettings();
	
	USaveData_PlayerProfile* ReturnPlayerSaveGameReference();
	
	UDataTable* GetAvatarsDataTable() const { return AvatarsDataTable; }
	
	UFUNCTION(BlueprintCallable)
	USaveData_DeveloperSettings* GetDevSettingsSaveFile();

	UFUNCTION(BlueprintCallable)
	FDeveloperSettingsAsStruct GetDevSettingsStruct();

// ------------------------- Player
	UFUNCTION(BlueprintCallable)
	void SaveToCurrentProfile();
	
	UFUNCTION(BlueprintCallable)
	void LoadProfile(FString ProfileName);

	UFUNCTION(BlueprintCallable)
	void DeleteProfile(FString ProfileName);

// ------------------------- Level
	UFUNCTION()
	AActor_GridTile* FindTileByCoordinates(int x, int y) const;


// Networking
// --------------------------------------------------
	/* Delegate called when session created */
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	/* Delegate called when session started */
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

	/** Handles to registered delegates for creating/starting a session */
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;

	TSharedPtr<class FOnlineSessionSettings> SessionSettings;

	/*
	*	Function to host a game!
	*
	*	@Param		UserID			User that started the request
	*	@Param		SessionName		Name of the Session
	*	@Param		bIsLAN			Is this is LAN Game ?
	*   @Param		bIsPresence		"Is the Session to create a presence Session"
	*	@Param		MaxNumPlayers	        Number of Maximum allowed players on this "Session" (Server)
	*/
	bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers, FString CustomLobbyName);

	/*
	*	Function fired when a session create request has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	/*
	*	Function fired when a session start request has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);

	/*
	*	Find an online session
	*
	*	@param UserId user that initiated the request
	*	@param bIsLAN are we searching LAN matches
	*	@param bIsPresence are we searching presence sessions
	*/
	void FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence);

	/* Delegate for searching for sessions */
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;

	/* Handle to registered delegate for searching a session */
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	/*
	*	Delegate fired when a session search query has completed
	*
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnFindSessionsComplete(bool bWasSuccessful);

	/*
	*	Joins a session via a search result
	*
	*	@param SessionName name of session
	*	@param SearchResult Session to join
	*
	*	@return bool true if successful, false otherwise
	*/
	bool JoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

	/* Delegate for joining a session */
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	/* Handle to registered delegate for joining a session */
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	/*
	*	Delegate fired when a session join request has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	/** Delegate for destroying a session */
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

	/** Handle to registered delegate for destroying a session */
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;

/**
*	Delegate fired when a destroying an online session has completed
*
*	@param SessionName the name of the session this callback is for
*	@param bWasSuccessful true if the async action completed without error, false if there was an error
*/
	virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);


	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	void StartOnlineGame(FString CustomLobbyName);

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	void FindOnlineGames(bool UseLAN);

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	void JoinOnlineGame();

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	void DestroySessionAndLeaveGame();
};
