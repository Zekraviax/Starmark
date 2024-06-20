#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_MainMenu.generated.h"

// Forward Declarations
class UWidget_ServerCreator;
class UWidget_ServerBrowser;


UCLASS()
class STARMARK_API UWidget_MainMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
// Variables
// --------------------------------------------------

// ------------------------- References
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UWidget_ServerCreator> ServerCreatorWidget_Class;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidget_ServerCreator* ServerCreatorWidget_Reference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UWidget_ServerBrowser> ServerBrowserWidget_Class;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidget_ServerBrowser* ServerBrowserWidget_Reference;


// Function
// --------------------------------------------------

// ------------------------- Widget
	UFUNCTION(BlueprintCallable)
	void OnHostGameButtonPressed();

	UFUNCTION(BlueprintCallable)
	void OnSearchGameButtonPressed();
};
