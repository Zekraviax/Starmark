#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/ComboBoxString.h"
#include "Math/Color.h"

#include "Widget_DevMenu.generated.h"

// Forward Declarations
class AActor_CharacterTest;

// Unique Variables
UENUM(BlueprintType)
enum class E_DevMenu_ColourChangeDropdowns : uint8
{
	E_Colour1,
	E_Colour2,
	E_Colour3,
};


UCLASS()
class STARMARK_API UWidget_DevMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
// Base Variables
// --------------------------------------------------

// ------------------------- Components
	UPROPERTY(meta = (BindWidget))
	UComboBoxString* ColourOneDropDown;
	UPROPERTY(meta = (BindWidget))
	UComboBoxString* ColourTwoDropDown;
	UPROPERTY(meta = (BindWidget))
	UComboBoxString* ColourThreeDropDown;

// ------------------------- Dye-Changing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dye-Changing")
	TMap<FLinearColor, FString> DyeColorsMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dye-Changing")
	TSoftObjectPtr<AActor> DyeChangeActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dye-Changing")
	TSoftObjectPtr<AActor_CharacterTest> DyeChangeCharacter;


// Function
// --------------------------------------------------

// ------------------------- Widgets
	UFUNCTION(BlueprintCallable)
	void OnWidgetOpened();

// ------------------------- Dye-Changing
	UFUNCTION(BlueprintCallable)
	void OnColourDropdownChanged(E_DevMenu_ColourChangeDropdowns Dropdown);
};
