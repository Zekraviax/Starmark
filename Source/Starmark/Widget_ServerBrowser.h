#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_ServerBrowser.generated.h"

// Forward Declarations
class UWidget_MainMenu;


UCLASS()
class STARMARK_API UWidget_ServerBrowser : public UUserWidget
{
	GENERATED_BODY()
	
public:
// Variables
// --------------------------------------------------

// ------------------------- References
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UWidget_MainMenu> MainMenuWidget_Class;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidget_MainMenu* MainMenuWidget_Reference;

// ------------------------- Multiplayer
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool SearchModeLAN = false;

// Functions
// --------------------------------------------------

// ------------------------- Widget
	UFUNCTION(BlueprintCallable)
	void OnExitButtonPressed();
};
