#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/EditableText.h"

#include "Widget_PlayerProfileCreator.generated.h"

// Forward Declarations
class UWidget_MainMenu;
class USaveGame;


UCLASS()
class STARMARK_API UWidget_PlayerProfileCreator : public UUserWidget
{
	GENERATED_BODY()

public:
// Variables
// --------------------------------------------------

// ------------------------- Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UEditableText* NameEntryField;

// ------------------------- References
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UWidget_MainMenu> MainMenuWidget_Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidget_MainMenu* MainMenuWidget_Reference;
	
// Functions
// --------------------------------------------------

// ------------------------- Widget
	UFUNCTION(BlueprintCallable)
	void OnWidgetOpened(USaveGame* PlayerProfile);

	UFUNCTION(BlueprintCallable)
	void OnSaveGameButtonPressed(USaveGame* PlayerProfile);

	UFUNCTION(BlueprintCallable)
	void OnExitButtonPressed();
};
