#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/ScrollBox.h"

#include "Widget_ServerBrowser.generated.h"

// Forward Declarations
class UWidget_MainMenu;
class UWidgetComponent_FoundServer;


UCLASS()
class STARMARK_API UWidget_ServerBrowser : public UUserWidget
{
	GENERATED_BODY()
	
public:
// Variables
// --------------------------------------------------

// ------------------------- Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UScrollBox* FoundServersScrollBox;

// ------------------------- References
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UWidget_MainMenu> MainMenuWidget_Class;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidget_MainMenu* MainMenuWidget_Reference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UWidgetComponent_FoundServer> FoundServerWidgetComponent_Class;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent_FoundServer* FoundServerWidgetComponent_Reference;

// ------------------------- Multiplayer
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool SearchModeLAN = false;

// Functions
// --------------------------------------------------

// ------------------------- Widget
	UFUNCTION(BlueprintCallable)
	void OnExitButtonPressed();

	UFUNCTION(BlueprintCallable)
	void PopulateServerBrowserList();

	UFUNCTION(BlueprintNativeEvent)
	void FinishedFindingSessions();
};
