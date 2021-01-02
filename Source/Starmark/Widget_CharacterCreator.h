#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/TextBlock.h"
#include "Components/ComboBoxString.h"

#include "Widget_CharacterCreator.generated.h"

// Forward Declarations
class UStarmark_GameInstance;


UCLASS()
class STARMARK_API UWidget_CharacterCreator : public UUserWidget
{
	GENERATED_BODY()

public:
// Base Variables
// --------------------------------------------------

// ------------------------- Components
	UPROPERTY(meta = (BindWidget))
	UComboBoxString* Birthday_Day_DropDown;

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* Birthday_Month_DropDown;

	UPROPERTY(meta = (BindWidget))
	UComboBoxString* Birthday_Year_DropDown;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HoroscopeText;

// ------------------------- References
	UPROPERTY()
	UStarmark_GameInstance* GameInstanceReference;

// ------------------------- Other
	UPROPERTY()
	int BirthDayAsNumber;

	UPROPERTY()
	int BirthMonthAsNumber;

	UPROPERTY()
	int BirthYearAsNumber;

// Function
// --------------------------------------------------

// ------------------------- Widget
	UFUNCTION(BlueprintCallable)
	void OnWidgetOpened();

// ------------------------- Player
	UFUNCTION(BlueprintCallable)
	void CalculateHoroscope();
};
