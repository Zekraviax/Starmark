#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

#include "Engine/DataTable.h"
#include "Starmark_Variables.h"

#include "Starmark_GameMode.generated.h"


// Forward Declarations
class AActor_AttackEffectsLibrary;
class ACharacter_Pathfinder;
class APlayerController_Battle;
class APlayerPawn_Flying;
class APlayerPawn_Static;
class AStarmark_GameState;
class UStarmark_GameInstance;
class AStarmark_PlayerState;


// The GameMode that handles battles; both singleplayer and multiplayer.
// Instantiated when the level is first loaded, and destroyed when the level is destroyed.
UCLASS()
class STARMARK_API AStarmark_GameMode : public AGameMode
{
	GENERATED_BODY()

public:
// Variables
// --------------------------------------------------

// ------------------------- References
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AStarmark_GameState* GameStateReference;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStarmark_GameInstance* HostPlayerGameInstanceReference;

// ------------------------- Data Tables
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* AttacksDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* AvatarSimpleAttacksDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* AvatarComplexAttacksDataTable;

// ------------------------- Battle
	// To-Do: Deprecate this array. The GameState should be handling this.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<APlayerController_Battle*> PlayerControllerReferences;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APlayerPawn_Flying> PlayerPawnBlueprintClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ACharacter_Pathfinder> AvatarBlueprintClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ExpectedPlayers = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor_AttackEffectsLibrary> AttackEffectsLibrary_Class;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AActor_AttackEffectsLibrary* AttackEffectsLibrary_Reference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* StatusEffectsDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FString> CombatLogTextArray;

	// This should only be used to recalculate the AvatarTurnOrder.
	// It should be reset at the start of each Avatar's turn.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool AvatarDiedThisTurn = false;

// ------------------------- Multiplayer
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTimerHandle PlayerReadyCheckTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int MultiplayerUniqueIDCounter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int BattleUniqueIDCounter;	

// ------------------------- Other
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString GameModeContextString;

// Functions
// --------------------------------------------------

// ------------------------- Local Helper Functions
	void SetGameStateLocalReference();
	
	AStarmark_GameState* GetGameState();
	UStarmark_GameInstance* GetHostPlayerGameStateInstanceReference() const;
	TArray<APlayerController_Battle*> GetAllBattlePlayerControllers() const;
	TArray<ACharacter_Pathfinder*> GetAllAvatars() const;

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerDumpMultiplayerBattleToLogs();

// ------------------------- Battle
	// Pre-Battle Setup Functions
	virtual void HandleSeamlessTravelPlayer(AController*& C);

	UFUNCTION(BlueprintCallable)
	void OnPlayerPostLogin(APlayerController_Battle* NewPlayerController);

	UFUNCTION(Server, Reliable)
	void GetPreBattleChecks();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_BeginMultiplayerBattle();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_MultiplayerBattleCheckAllPlayersReady();

	// Battle Functions
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_AssembleTurnOrderText();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_SpawnAvatar(APlayerController_Battle* PlayerController, FPlayer_Data PlayerData, int IndexInPlayerParty, FAvatar_Struct AvatarData, FVector Location = FVector(0, 0, -9999));

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_UpdateAllAvatarDecals();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_AvatarBeginTurn(int CurrentAvatarTurnIndex);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_LaunchAttack(ACharacter_Pathfinder* Attacker, AActor* Target, const FString& AttackName);

	UFUNCTION(Server, Reliable)
	void Server_AvatarDefeated(ACharacter_Pathfinder* Avatar);

	UFUNCTION()
	void EndOfBattle() const;
};