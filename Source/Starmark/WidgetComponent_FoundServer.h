#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "OnlineSessionSettings.h"

#include "WidgetComponent_FoundServer.generated.h"


UCLASS()
class STARMARK_API UWidgetComponent_FoundServer : public UUserWidget
{
	GENERATED_BODY()

public:
// Variables
// --------------------------------------------------

// ------------------------- Server
	TSharedPtr<const FUniqueNetId> UserId;
	FName SessionName;
	FOnlineSessionSearchResult Session;

// Variables
// --------------------------------------------------

// ------------------------- Widget
	UFUNCTION(BlueprintCallable)
	void OnJoinButtonPressed();
};
