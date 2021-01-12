#include "Widget_DevMenu.h"


#include "Materials/MaterialInstanceDynamic.h"
#include "Actor_CharacterTest.h"
#include "Starmark_GameMode.h"


// ------------------------- Widgets
void UWidget_DevMenu::OnWidgetOpened()
{
	for (auto Dye = DyeColorsMap.CreateConstIterator(); Dye; ++Dye) {
		ColourOneDropDown->AddOption(*Dye.Value());
		ColourTwoDropDown->AddOption(*Dye.Value());
		ColourThreeDropDown->AddOption(*Dye.Value());
	}
}


// ------------------------- Dye-Changing
void UWidget_DevMenu::OnColourDropdownChanged(E_DevMenu_ColourChangeDropdowns Dropdown)
{
	if (DyeChangeCharacter) {
		UMaterialInstanceDynamic* Material = DyeChangeCharacter->DynamicMaterial;

		for (auto Dye = DyeColorsMap.CreateConstIterator(); Dye; ++Dye) {
			if (*Dye.Value() == ColourOneDropDown->GetSelectedOption()) {
				Material->SetVectorParameterValue("Colour1", Dye.Key());

			}
			if (*Dye.Value() == ColourTwoDropDown->GetSelectedOption()) {
				Material->SetVectorParameterValue("Colour2", Dye.Key());
			}
			if (*Dye.Value() == ColourThreeDropDown->GetSelectedOption()) {
				Material->SetVectorParameterValue("Colour3", Dye.Key());
			}
		}

		//DyeChangeCharacter->Mesh->SetMaterial(0, Material);
	}
}


void UWidget_DevMenu::CalculateTypeStrengthsAndWeaknesses()
{
	FString ContextString, PrimaryTypeString, SecondaryTypeString, CombinationTypeString, TextBoxName;
	TArray<FName> CombinationTypesRowNames;
	FCharacter_CombinationTypes* CombinationType;
	int CurrentTypeEffectivenessValue = 1;

	if (CombinationTypesRowNames.Num() <= 0) {
		CombinationTypesRowNames = CombinationTypesDataTable->GetRowNames();
	}

	if (PrimaryTypeDropdown->GetSelectedOption() != "N/A" && SecondaryTypeDropdown->GetSelectedOption() != "N/A"
		&& PrimaryTypeDropdown->GetSelectedOption() != SecondaryTypeDropdown->GetSelectedOption()) {

		for (auto& Name : CombinationTypesRowNames) {
			CombinationType = CombinationTypesDataTable->FindRow<FCharacter_CombinationTypes>(Name, ContextString);

			PrimaryTypeString = UEnum::GetDisplayValueAsText<ECharacter_Types>(CombinationType->PrimaryType).ToString();
			SecondaryTypeString = UEnum::GetDisplayValueAsText<ECharacter_Types>(CombinationType->SecondaryType).ToString();

			PrimaryTypeString = PrimaryTypeString.Right(PrimaryTypeString.Len() - 2);
			SecondaryTypeString = SecondaryTypeString.Right(SecondaryTypeString.Len() - 2);

			if (PrimaryTypeString == PrimaryTypeDropdown->GetSelectedOption() ||
				PrimaryTypeString == SecondaryTypeDropdown->GetSelectedOption()) {
				if (SecondaryTypeString == PrimaryTypeDropdown->GetSelectedOption() ||
					SecondaryTypeString == SecondaryTypeDropdown->GetSelectedOption()) {
					CombinationTypeString = UEnum::GetDisplayValueAsText<ECharacter_Types>(CombinationType->CombinationType).ToString();
					CombinationTypeString = CombinationTypeString.Right(CombinationTypeString.Len() - 2);

					CombinationTypeText->SetText(FText::FromString(CombinationTypeString));
					break;
				}
			}
		}

		// Type Effectiveness Calculations
		for (auto& TypeTextBox : TypeTextsGridPanel->GetAllChildren()) {
			for (auto& Name : CombinationTypesRowNames) {
				CombinationType = CombinationTypesDataTable->FindRow<FCharacter_CombinationTypes>(Name, ContextString);
				TextBoxName = Cast<UTextBlock>(TypeTextBox)->GetName();

				if (TextBoxName == PrimaryTypeString || TextBoxName == SecondaryTypeString) {

				}

				//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, (Cast<UTextBlock>(TypeTextBox)->GetName()));
			}
		}
	}
}