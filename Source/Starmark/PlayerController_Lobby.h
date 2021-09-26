#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerController_Lobby.generated.h"


// Forward Declarations
class UWidget_ServerHost;
class UWidgetComponent_Avatar;
class UWidgetComponent_LobbyPlayerVals;


// Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAvatarChangedSlot);


UCLASS()
class STARMARK_API APlayerController_Lobby : public APlayerController
{
	GENERATED_BODY()

public:
// Variables
// --------------------------------------------------

// ------------------------- References
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UWidget_ServerHost> LobbyWidget_Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidget_ServerHost* LobbyWidget_Reference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UWidgetComponent_LobbyPlayerVals> LobbyPlayerVals_Class;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent_LobbyPlayerVals* LobbyPlayerVals_Reference;

// ------------------------- Widgets
	UPROPERTY()
	UWidgetComponent_Avatar* CurrentSelectedAvatarWidgetComponent;

// ------------------------- Delegates
	UPROPERTY()
	FAvatarChangedSlot OnAvatarChangedSlotDelegate;

// Functions
// --------------------------------------------------

// ------------------------- Lobby
	virtual void OnRep_PlayerState() override;

// ------------------------- Widgets
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void PlayerJoinedMultiplayerLobby();

	UFUNCTION(BlueprintCallable, Client, Unreliable)
	void Client_UpdatePlayersInLobby(const TArray<FString>& PlayerNames, const TArray<FString>& PlayerReadyStatuses, bool ClearCurrentPlayers);

	UFUNCTION(BlueprintCallable)
	void UpdatePlayersInLobby(TArray<FString> PlayerNames, TArray<FString> PlayerReadyStatuses, bool ClearCurrentPlayers);

	UFUNCTION()
	void OnAvatarWidgetComponentClicked(UWidgetComponent_Avatar* SecondClickedAvatarWidgetComponent);
};
