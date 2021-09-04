#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

#include "Engine/DataTable.h"

#include "Starmark_GameMode.generated.h"


// Forward Declarations
class ACharacter_Pathfinder;
class APlayerPawn_Static;


UCLASS()
class STARMARK_API AStarmark_GameMode : public AGameMode
{
	GENERATED_BODY()

public:
// Variables
// --------------------------------------------------

// ------------------------- Battle
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<APlayerController_Base*> PlayerControllerReferences;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APlayerPawn_Static> PlayerPawnBlueprintClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ACharacter_Pathfinder> AvatarBlueprintClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int UniquePlayerIDCounter;

	// How many players are expected to join before a multiplayer match can begin
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MultiplayerExpectedPlayerCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle FirstUpdateTimerHandle;

// Functions
// --------------------------------------------------

// ------------------------- Battle
	UFUNCTION(BlueprintCallable)
	void OnPlayerPostLogin(APlayerController_Base* NewPlayerController);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_BeginMultiplayerBattle();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void Multicast_SendUpdateToAllPlayers();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Server_SpawnAvatar(APlayerController_Base* PlayerController);

	UFUNCTION(BlueprintNativeEvent)
	void EndOfBattle();
};