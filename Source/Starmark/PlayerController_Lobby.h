#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerController_Lobby.generated.h"


// Forward Declarations
class UWidget_ServerHost;


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

// Functions
// --------------------------------------------------

// ------------------------- Widgets
	UFUNCTION(BlueprintCallable)
	void AddLobbyMenuToScreen();
};
