#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

#include "Engine/DataTable.h"
#include "Starmark_Variables.h"

#include "Starmark_GameMode.generated.h"


// Forward Declarations
class AActor_AttackEffectsLibrary;
class ACharacter_Pathfinder;
class AStarmark_GameState;
class APlayerController_Battle;
class APlayerPawn_Static;
class APlayerPawn_Flying;
class AStarmark_PlayerState;


UCLASS()
class STARMARK_API AStarmark_GameMode : public AGameMode
{
	GENERATED_BODY()

public:
// Variables
// --------------------------------------------------

// ------------------------- References
	UPROPERTY()
	AStarmark_GameState* GameStateReference;

// ------------------------- Data Tables
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* AvatarDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* AttacksDataTable;

// ------------------------- Battle
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* AvatarSimpleAttacksDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* AvatarComplexAttacksDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<APlayerController_Battle*> PlayerControllerReferences;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APlayerPawn_Flying> PlayerPawnBlueprintClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ACharacter_Pathfinder> AvatarBlueprintClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* UltimateTypeChartDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ExpectedPlayers = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor_AttackEffectsLibrary> AttackEffectsLibrary_Class;

	UPROPERTY()
	AActor_AttackEffectsLibrary* AttackEffectsLibrary_Reference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* StatusEffectsDataTable;

	UPROPERTY()
	TArray<FString> CombatLogTextArray;

// ------------------------- Multiplayer
	UPROPERTY()
	FTimerHandle PlayerReadyCheckTimerHandle;

	UPROPERTY()
	int MultiplayerUniqueIDCounter;

// ------------------------- Other
	UPROPERTY()
	FString GameModeContextString;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AStarmark_PlayerState> StarmarkPlayerStateBlueprintClass;

// Functions
// --------------------------------------------------

// ------------------------- Battle
	UFUNCTION(BlueprintCallable)
	void OnPlayerPostLogin(APlayerController_Battle* NewPlayerController);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_BeginMultiplayerBattle();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_SinglePlayerBeginMultiplayerBattle(APlayerController_Battle* PlayerControllerReference);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_MultiplayerBattleCheckAllPlayersReady();

	UFUNCTION(BlueprintCallable, Server, Unreliable)
	void Server_AssembleTurnOrderText();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_SpawnAvatar(APlayerController_Battle* PlayerController, int IndexInPlayerParty, FAvatar_Struct AvatarData);

	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable)
	void Server_UpdateAllAvatarDecals();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void Server_LaunchAttack(ACharacter_Pathfinder* Attacker, AActor* Target, const FString& AttackName);

	UFUNCTION(BlueprintNativeEvent)
	void EndOfBattle();
};