#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/TextBlock.h"

#include "WidgetComponent_LobbyPlayerVals.generated.h"


UCLASS()
class STARMARK_API UWidgetComponent_LobbyPlayerVals : public UUserWidget
{
	GENERATED_BODY()
	
public:
// Variables
// --------------------------------------------------

// ------------------------- Components
	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerReadyStatus;
};
