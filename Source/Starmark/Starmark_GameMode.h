#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

#include "Engine/DataTable.h"

#include "Starmark_GameMode.generated.h"


// Forward Declarations
class ACharacter_Pathfinder;
class APlayerPawn_Static;
class APlayerPawn_Flying;


UCLASS()
class STARMARK_API AStarmark_GameMode : public AGameMode
{
	GENERATED_BODY()

public:
// Variables
// --------------------------------------------------

// ------------------------- Avatar
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* AvatarDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* AvatarSimpleAttacksDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* AvatarComplexAttacksDataTable;

// ------------------------- Battle
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<APlayerController_Base*> PlayerControllerReferences;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APlayerPawn_Flying> PlayerPawnBlueprintClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ACharacter_Pathfinder> AvatarBlueprintClass;

// ------------------------- Multiplayer
	UPROPERTY()
	FTimerHandle PlayerReadyCheckTimerHandle;

	UPROPERTY()
	int MultiplayerUniqueIDCounter;

// Functions
// --------------------------------------------------

// ------------------------- Battle
	UFUNCTION(BlueprintCallable)
	void OnPlayerPostLogin(APlayerController_Base* NewPlayerController);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_BeginMultiplayerBattle();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_MultiplayerBattleCheckAllPlayersReady();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_SpawnAvatar(APlayerController_Base* PlayerController);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void Server_UpdateAllAvatarDecals();

	UFUNCTION(BlueprintNativeEvent)
	void EndOfBattle();
};