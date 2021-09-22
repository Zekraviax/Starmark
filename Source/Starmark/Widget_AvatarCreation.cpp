#include "Widget_AvatarCreation.h"


// ------------------------- Widget
void UWidget_AvatarCreation::PopulateDropDowns()
{
	if (AvatarDataTable) {
		TArray<FName> AvatarRowNames;
		AvatarRowNames = AvatarDataTable->GetRowNames();
	}

	if (SimpleAttacksDataTable) {
		TArray<FName> SimpleAttacksRowNames;
		SimpleAttacksRowNames = ComplexAttacksDataTable->GetRowNames();
	}

	if (ComplexAttacksDataTable) {
		TArray<FName> ComplexAttacksRowNames;
		ComplexAttacksRowNames = ComplexAttacksDataTable->GetRowNames();
	}
}