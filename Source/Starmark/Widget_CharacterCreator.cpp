#include "Widget_CharacterCreator.h"

#include "Misc/DateTime.h"


// ------------------------- Widget
void UWidget_CharacterCreator::OnWidgetOpened()
{
	FDateTime* Date = new FDateTime;
	int CurrentYear = Date->FDateTime::Now().GetYear();

	if (Birthday_Day_DropDown->IsValidLowLevel()) {
		for (int d = 1; d <= 31; d++) {
			Birthday_Day_DropDown->AddOption(FString::FromInt(d));
		}
	}

	if (Birthday_Month_DropDown->IsValidLowLevel()) {
		for (int m = 1; m <= 12; m++) {
			Birthday_Month_DropDown->AddOption(FString::FromInt(m));
		}
	}

	if (Birthday_Year_DropDown->IsValidLowLevel()) {
		for (int y = CurrentYear; y >= 1900; y--) {
			Birthday_Year_DropDown->AddOption(FString::FromInt(y));
		}
	}
}