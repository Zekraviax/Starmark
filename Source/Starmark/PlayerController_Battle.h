#pragma once

#include "CoreMinimal.h"
#include "Actor_AttackEffectsLibrary.h"
#include "GameFramework/PlayerController.h"

#include "Character_Pathfinder.h"

#include "PlayerController_Battle.generated.h"


// Forward Declarations
class AAIController_Avatar;
class UPlayer_SaveData;
class UWidget_HUD_Battle;
class UWidget_MultiplayerEndMenu;


// Unique Enums
UENUM(BlueprintType)
enum class E_PlayerCharacter_ClickModes : uint8
{
	// Use this click mode for times when we don't want the player to be able to interact with anything at all.
	// This click mode can be used to prevent any input, not just mouse clicks.
	E_Nothing,
	E_SelectCharacterToControl,
	E_SelectCharacterToAttack,
	E_MoveCharacter,

	// This is the first of the two Avatar summoning click modes.
	// Use this one to have the player select an Avatar-in-reserve, and the location to summon it.
	SelectReserveAvatarToSummon,
	// The "SummonAvatar" click mode should only be used to finalize the Summon action.
	// Use this once the player has selected the Avatar they wish to summon, and the location for said avatar.
	SummonAvatar,
	
	// This is the first of the two Item use click modes.
	// Use this one to have the player select the item they wish to use, 
	// and any other necessary actions like selecting a location for said item.
	SelectItemToUse,
	// This click mode should only be used to finaluse the Use Item action.
	// Use this once the player has selected the item they wish to use, plus any other necessary actions.
	UseItem
};


UENUM(BlueprintType)
enum class E_PlayerCharacter_HighlightModes : uint8
{
	E_None,
	E_MovePath,
	E_AttackPattern
};


UCLASS()
class STARMARK_API APlayerController_Battle : public APlayerController
{
	GENERATED_BODY()

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;

public:
	APlayerController_Battle();

// Variables
// --------------------------------------------------

// ------------------------- Controller
	// To-Do: Figure out if this variable needs to be replicated. (Do other players need to know this?)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	E_PlayerCharacter_ClickModes PlayerClickMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	E_PlayerCharacter_HighlightModes TileHighlightMode = E_PlayerCharacter_HighlightModes::E_MovePath;

// ------------------------- Widgets
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UWidget_HUD_Battle> BattleWidgetChildClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	UWidget_HUD_Battle* BattleWidgetReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UWidget_MultiplayerEndMenu> EndOfBattleWidgetChildClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	UWidget_MultiplayerEndMenu* EndOfBattleWidgetReference;

// ------------------------- Avatar
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	ACharacter_Pathfinder* CurrentSelectedAvatar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTimerHandle PlayerStateTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int SelectedReserveAvatarBattleUniqueID;

// ------------------------- Player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool IsReadyToStartMultiplayerBattle = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	TArray<FAvatar_Struct> PlayerParty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool IsCurrentlyActingPlayer;

	// This should only be used to pass the player's data to the server's GameState.
	// For all other needs, get this data from the GameState TMap.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FPlayer_Data PlayerDataStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FString PlayerName;

	// Use this to find the player's data in the GameState's player data array.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	int MultiplayerUniqueID;

// ------------------------- Grid
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CursorLocationSnappedToGrid;

// ------------------------- Other
	UPROPERTY()
	FTimerHandle DelayedEndTurnTimerHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor_AttackEffectsLibrary> AttackEffectsLibrary_Class;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AActor_AttackEffectsLibrary* AttackEffectsLibrary_Reference;

// Functions
// --------------------------------------------------

// ------------------------- Helper Functions
	FPlayer_Data ReturnPlayerData() { return PlayerDataStruct; }

	// Handles all secondary functions when changing click modes (e.g. list of attacks vs list of reserve avatars)
	void SetPlayerClickMode(E_PlayerCharacter_ClickModes NewClickMode);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClientTellServerToDumpMultiplayerLogs();
	UFUNCTION(Server, Reliable)
	void ServerTellServerToDumpMultiplayerLogs();

// ------------------------- Widgets
	UFUNCTION(Client, Reliable)
	void Client_ClearLobbyFromScreen();

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void CreateBattleWidget();
	void Local_BattleWidget_AddToScreen() const;
	
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void CreateEndOfBattleWidget();

	UFUNCTION(BlueprintCallable)
	void SetBattleWidgetVariables();

// ------------------------- Avatar
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void OnRepNotify_CurrentSelectedAvatar();

// ------------------------- Battle
	// The client fetches their data from their GameInstance here,
	// and sends it to the server here.
	UFUNCTION(Client, Reliable)
	void ClientGetIdsAndSendDataToServer(int BattleUniqueIDCounter, int MultiplayerUniqueIDCounter);
	UFUNCTION(Client, Reliable)
	void ClientSendDataToServer();

	// The server receives the data from the client here.
	// And passes it on to the GameState.
	UFUNCTION(Server, Reliable)
	void ServerSendDataToServer(FPlayer_Data ReceivedPlayerDataStruct);

	UFUNCTION(Server, Reliable)
	void ServerSendDataToServerWithReturn();
	
	UFUNCTION()
	FPlayer_Data Server_GetDataFromProfile();

	UFUNCTION(Server, Reliable)
	void Server_SetReadyToStartMultiplayerBattle();

	UFUNCTION(Client, Reliable)
	void GetUpdatedCombatLogTextFromGameState(const FString& CombatLogText);

	UFUNCTION(BlueprintCallable)
	void OnMouseCursorBeginHover(ACharacter_Pathfinder* ActingAvatar, TArray<FVector> PathBetweenAvatars);

	UFUNCTION(BlueprintCallable)
	void OnPrimaryClick(AActor* ClickedActor, TArray<AActor*> ValidTargetsArray);

	UFUNCTION(BlueprintCallable)
	void SubtractAvatarMovementPoints(int Moves);

	// Use this function to highlight all avatars and entities passed,
	// and to un-highlight all avatars and tiles not passed.
	void HighlightSpecificAvatarsAndTiles(const TArray<ACharacter_Pathfinder*>& Avatars, const TArray< AActor_GridTile*>& Tiles) const;
	void BeginSelectingTileForReserveAvatar(int SelectedBattleUniqueID);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void SummonReserveAvatarAtSelectedTile(AActor_GridTile* SelectedTile, ACharacter_Pathfinder* SelectedAvatar);

	void DelayedEndTurn();

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void Client_SendEndOfTurnCommandToServer();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SendEndOfTurnCommandToServer();

	// -------- HUD functions that the GameMode can call for all players. -------- //
	// Called at the end of a turn, before the currently acting Avatar changes.
	UFUNCTION(Server, Unreliable)
	void Player_OnAvatarTurnChanged();

	UFUNCTION(Client, Reliable)
	void Client_UpdateAttacksInHud(const ACharacter_Pathfinder* ActingAvatar);

	UFUNCTION(Client, Reliable)
	void Client_UpdateCurrentAvatarInHud(ACharacter_Pathfinder* ActingAvatar);

	UFUNCTION(Client, Reliable)
	void Client_UpdateCurrentTurnOrderInHud(const TArray<UTexture2D*>& InDynamicAvatarTurnOrderImages);

	UFUNCTION(Client, Reliable)
	void Client_ShowHideHud(bool ShowHud);

// ------------------------- Multiplayer Battle
	UFUNCTION(Client, Reliable)
	void GetAvatarUpdateFromServer(ACharacter_Pathfinder* AvatarReference, int AvatarUniqueID, bool IsCurrentlyActing, bool IsCurrentlSelectedAvatar);
	void LocalAvatarUpdate(ACharacter_Pathfinder* AvatarReference, int AvatarUniqueID, bool IsCurrentlyActing, bool IsCurrentlSelectedAvatar);

	UFUNCTION(Server, Reliable)
	void Client_SendLaunchAttackToServer(ACharacter_Pathfinder* Attacker, AActor* Target, const FString& AttackName);
};