#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_ServerCreator.generated.h"

// Forward Declarations
class UWidget_MainMenu;


UCLASS()
class STARMARK_API UWidget_ServerCreator : public UUserWidget
{
	GENERATED_BODY()
	
public:
// Variables
// --------------------------------------------------

// ------------------------- References
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	TSubclassOf<UWidget_MainMenu> MainMenuWidget_Class;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidget_MainMenu* MainMenuWidget_Reference;

// Functions
// --------------------------------------------------

// ------------------------- Widget
	UFUNCTION(BlueprintCallable)
	void OnExitButtonPressed();
};
