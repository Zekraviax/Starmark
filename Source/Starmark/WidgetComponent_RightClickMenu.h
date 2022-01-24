#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/VerticalBox.h"
#include "Starmark_Variables.h"

#include "WidgetComponent_RightClickMenu.generated.h"


// Forward Declarations
class UWidgetComponent_RightClickMenuButton;


UCLASS()
class STARMARK_API UWidgetComponent_RightClickMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
// Variables
// --------------------------------------------------

// ------------------------- Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UVerticalBox* ButtonsVerticalBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UWidgetComponent_RightClickMenuButton> RightClickMenuButton_Class;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent_RightClickMenuButton* RightClickMenuButton_Reference;

// ------------------------- Functions
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UUserWidget* OwnerWidget;

	UPROPERTY()
	TArray<E_RightClickMenu_Commands> Commands;

// Functions
// --------------------------------------------------

// ------------------------- Widget
	UFUNCTION()
	void OnWidgetCreated();
};