#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/TextBlock.h"
#include "OnlineSessionSettings.h"

#include "WidgetComponent_FoundServer.generated.h"


UCLASS()
class STARMARK_API UWidgetComponent_FoundServer : public UUserWidget
{
	GENERATED_BODY()

public:
// Variables
// --------------------------------------------------

// ------------------------- Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ServerNameText;

// ------------------------- Server
	TSharedPtr<const FUniqueNetId> UserId;
	FName SessionName;
	FOnlineSessionSearchResult Session;

// ------------------------- Widget
	UPROPERTY()
	FString CustomLobbyName;

// Variables
// --------------------------------------------------

// ------------------------- Widget
	UFUNCTION(BlueprintCallable)
	void OnJoinButtonPressed();

	UFUNCTION()
	void SetLobbyName();
};
