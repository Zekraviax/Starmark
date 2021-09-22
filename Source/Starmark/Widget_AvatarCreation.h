#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/ComboBox.h"
#include "Engine/DataTable.h"

#include "Widget_AvatarCreation.generated.h"


UCLASS()
class STARMARK_API UWidget_AvatarCreation : public UUserWidget
{
	GENERATED_BODY()

public:

// Variables
// --------------------------------------------------

// ------------------------- Components
	UPROPERTY(meta = (BindWidgetOptional))
	UComboBox* SpeciesDropDown;

	UPROPERTY(meta = (BindWidgetOptional))
	UComboBox* AbilityDropDown;

	UPROPERTY(meta = (BindWidgetOptional))
	UComboBox* AccessoryDropDown;

	UPROPERTY(meta = (BindWidgetOptional))
	UComboBox* MoveOneDropDown;

	UPROPERTY(meta = (BindWidgetOptional))
	UComboBox* MoveTwoDropDown;

	UPROPERTY(meta = (BindWidgetOptional))
	UComboBox* MoveThreeDropDown;

	UPROPERTY(meta = (BindWidgetOptional))
	UComboBox* MoveFourDropDown;

// ------------------------- References
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* AvatarDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* SimpleAttacksDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* ComplexAttacksDataTable;

// Functions
// --------------------------------------------------

// ------------------------- Widget
	UFUNCTION(BlueprintCallable)
	void PopulateDropDowns();
};
