#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerController_Lobby.generated.h"


// Forward Declarations
class UWidget_ServerHost;
class UWidgetComponent_LobbyPlayerVals;


UCLASS()
class STARMARK_API APlayerController_Lobby : public APlayerController
{
	GENERATED_BODY()

public:
// Variables
// --------------------------------------------------

// ------------------------- Widgets
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UWidget_ServerHost> LobbyWidget_Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidget_ServerHost* LobbyWidget_Reference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UWidgetComponent_LobbyPlayerVals> LobbyPlayerVals_Class;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent_LobbyPlayerVals* LobbyPlayerVals_Reference;

// ------------------------- Lobby


// Functions
// --------------------------------------------------

// ------------------------- Widgets
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void PlayerJoinedMultiplayerLobby();

	UFUNCTION(BlueprintCallable)
	void UpdatePlayersInLobby(TArray<FString> PlayerNames, TArray<FString> PlayerReadyStatuses, bool ClearCurrentPlayers);
};
