#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/EditableText.h"
#include "Components/ScrollBox.h"

#include "Widget_PlayerProfileCreator.generated.h"

// Forward Declarations
class UWidget_MainMenu;
class USaveGame;
class USaveData_PlayerProfilesList;
class UWidgetComponent_PlayerProfile;


UCLASS()
class STARMARK_API UWidget_PlayerProfileCreator : public UUserWidget
{
	GENERATED_BODY()

public:
// Variables
// --------------------------------------------------

// ------------------------- Components
	UPROPERTY(meta = (BindWidget))
	UEditableText* NameEntryField;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* PlayerProfilesScrollBox;

// ------------------------- References
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TSubclassOf<UWidget_MainMenu> MainMenuWidget_Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidget_MainMenu* MainMenuWidget_Reference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UWidgetComponent_PlayerProfile> PlayerProfileWidgetComponent_Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetComponent_PlayerProfile* PlayerProfileWidgetComponent_Reference;

// ------------------------- Save Data
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USaveData_PlayerProfilesList* ProfilesList;
	

// Functions
// --------------------------------------------------

// ------------------------- Widget
	UFUNCTION(BlueprintCallable)
	void OnWidgetOpened();

	UFUNCTION(BlueprintCallable)
	void OnSaveGameButtonPressed();

	UFUNCTION(BlueprintCallable)
	void OnExitButtonPressed();
};
