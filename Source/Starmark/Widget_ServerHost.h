#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/ScrollBox.h"

#include "Widget_ServerHost.generated.h"

// Forward Declarations
class UWidgetComponent_PlayerProfile;
class UWidget_AvatarSelection;
class UWidget_MainMenu;


UCLASS()
class STARMARK_API UWidget_ServerHost : public UUserWidget
{
	GENERATED_BODY()
	
public:
// Variables
// --------------------------------------------------

// ------------------------- Components
	UPROPERTY(meta = (BindWidget))
	UButton* ReadyButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ReadyButtonText;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* PlayerListVerticalBox;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* PlayerProfilesScrollBox;

// ------------------------- References
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UWidget_MainMenu> MainMenuWidget_Class;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidget_MainMenu* MainMenuWidget_Reference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UWidgetComponent_PlayerProfile> PlayerProfile_Class;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent_PlayerProfile* PlayerProfile_Reference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UWidget_AvatarSelection> AvatarSelection_Class;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidget_AvatarSelection* AvatarSelection_Reference;

// Functions
// --------------------------------------------------

// ------------------------- Widget
	UFUNCTION(BlueprintCallable)
	void OnWidgetOpened();

	UFUNCTION(BlueprintCallable)
	void OnExitButtonPressed();

// ------------------------- Player
	UFUNCTION()
	void GetAllPlayerProfiles();
};