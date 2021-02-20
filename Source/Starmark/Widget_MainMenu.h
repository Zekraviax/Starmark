#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_MainMenu.generated.h"


UCLASS()
class STARMARK_API UWidget_MainMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
// Variables
// --------------------------------------------------

// ------------------------- Components

// Function
// --------------------------------------------------

// ------------------------- Widget
	UFUNCTION(BlueprintCallable)
	void OnHostGameButtonPressed();

	UFUNCTION(BlueprintCallable)
	void OnSearchGameButtonPressed();
};
