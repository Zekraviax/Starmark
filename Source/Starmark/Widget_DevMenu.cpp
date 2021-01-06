#include "Widget_DevMenu.h"


#include "Materials/MaterialInstanceDynamic.h"
#include "Actor_CharacterTest.h"


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
				//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Change Colour1")));
				//break;
			}
			if (*Dye.Value() == ColourTwoDropDown->GetSelectedOption()) {
				Material->SetVectorParameterValue("Colour2", Dye.Key());
				//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Change Colour2")));
				//break;
			}
			if (*Dye.Value() == ColourThreeDropDown->GetSelectedOption()) {
				Material->SetVectorParameterValue("Colour3", Dye.Key());
				//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Change Colour3")));
				//break;
			}
		}

		DyeChangeCharacter->Mesh->SetMaterial(0, Material);
	}
}