#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "WidgetComponent_RightClickMenuButton.generated.h"


// Forward Declarations
class UWidgetComponent_RightClickMenu;


UCLASS()
class STARMARK_API UWidgetComponent_RightClickMenuButton : public UUserWidget
{
	GENERATED_BODY()
	
public:
// Variables
// --------------------------------------------------

// ------------------------- Components
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UButton* RightClickMenuButton;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	UTextBlock* ButtonLabel;

// ------------------------- Function
	UPROPERTY()
	UWidgetComponent_RightClickMenu* OwnerWidget;
};
