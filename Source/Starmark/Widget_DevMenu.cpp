#include "Widget_DevMenu.h"


#include "Materials/MaterialInstanceDynamic.h"
#include "Components/SkeletalMeshComponent.h"
#include "Actor_CharacterTest.h"
#include "Actor_MorphTargetTest.h"
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


// ------------------------- Type Chart Calculator
void UWidget_DevMenu::CalculateTypeStrengthsAndWeaknesses()
{
	FString ContextString, PrimaryTypeString, SecondaryTypeString, CombinationTypeString, TextBoxName;
	TArray<FName> CombinationTypesRowNames;
	TMap<FName, FRealCurve*> NumberedBaseTypeChartRowMap;
	FCharacter_CombinationTypes* CombinationType;
	int CurrentTypeEffectivenessValue = 1;

	// Get Row Names
	if (CombinationTypesRowNames.Num() <= 0) {
		CombinationTypesRowNames = CombinationTypesDataTable->GetRowNames();
	}
	if (NumberedBaseTypeChartRowMap.Num() <= 0) {
		NumberedBaseTypeChartRowMap = NumberedBaseTypeChartDataTable->GetRowMap();
	}

	if (PrimaryTypeDropdown->GetSelectedOption() != "N/A" && SecondaryTypeDropdown->GetSelectedOption() != "N/A"
		&& PrimaryTypeDropdown->GetSelectedOption() != SecondaryTypeDropdown->GetSelectedOption()) {

		// Reset Text Boxes
		for (auto& TypeTextBox : TypeTextsGridPanel->GetAllChildren()) {
			Cast<UTextBlock>(TypeTextBox)->SetText(FText::FromString("1"));
		}

		// Get all combination types
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

		//for (auto Entry = NumberedBaseTypeChartRowMap.CreateConstIterator(); Entry; ++Entry) {
		//	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, ("Type Chart Entry: %s"), *Entry.Key().ToString());
		//	//, TypeChartEntry.Value
		//}

		TArray<FName> RowNames;
		FRealCurve* Value;

		int FirstTypeIndex = PrimaryTypeDropdown->GetSelectedIndex();
		int SecondTypeIndex = SecondaryTypeDropdown->GetSelectedIndex();

		NumberedBaseTypeChartRowMap.GetKeys(RowNames);

		// Primary Type Multipliers
		for (int i = 0; i < RowNames.Num(); i++) {
			Value = *NumberedBaseTypeChartRowMap.Find(RowNames[i]);
			//CurrentTypeEffectivenessValue = 1;

			//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, (RowNames[i].ToString() + " / " + FString::SanitizeFloat(Value->Eval(FirstTypeIndex, i))));
			//CurrentTypeEffectivenessValue = CurrentTypeEffectivenessValue * Value->Eval(0, i);

			for (auto& TypeTextBox : TypeTextsGridPanel->GetAllChildren()) {
				if (Cast<UTextBlock>(TypeTextBox)->GetName() == RowNames[i].ToString()) {
					FText Text = Cast<UTextBlock>(TypeTextBox)->GetText();
					float FirstMultiplier = FCString::Atof(*Text.ToString()) * Value->Eval(FirstTypeIndex, i);
					float SecondMultiplier = FirstMultiplier * Value->Eval(SecondTypeIndex, i);

					Cast<UTextBlock>(TypeTextBox)->SetText(FText::FromString(FString::SanitizeFloat(SecondMultiplier)));
				}
			}
		}

		// Type Effectiveness Calculations
		//for (auto& TypeTextBox : TypeTextsGridPanel->GetAllChildren()) {
		//	for (auto& Name : CombinationTypesRowNames) {
		//		CombinationType = CombinationTypesDataTable->FindRow<FCharacter_CombinationTypes>(Name, ContextString);
		//		TextBoxName = Cast<UTextBlock>(TypeTextBox)->GetName();
		//		// Get the numbered type chart

		//		// For each type, add the combination type's strenghts and weaknesses to the multiplier

		//		//if (TextBoxName == PrimaryTypeString || TextBoxName == SecondaryTypeString) {

		//		//}

		//		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, (Cast<UTextBlock>(TypeTextBox)->GetName()));

		//		//break
		//	}
		//}
	}
}


// ------------------------- Face Customizer
void UWidget_DevMenu::OnFaceChanged(AActor_MorphTargetTest* MorphTargetTestCharacter)
{
	if (MorphTargetTestCharacter) {
		if (Face_DropDown->GetSelectedOption() == "0") {
			Cast<USkeletalMeshComponent>(MorphTargetTestCharacter->GetComponentByClass(USkeletalMeshComponent::StaticClass()))->SetMorphTarget("Key 1", 0.f);
			Cast<USkeletalMeshComponent>(MorphTargetTestCharacter->GetComponentByClass(USkeletalMeshComponent::StaticClass()))->SetMorphTarget("Key 2", 0.f);
		}
		else if (Face_DropDown->GetSelectedOption() == "1") {
			Cast<USkeletalMeshComponent>(MorphTargetTestCharacter->GetComponentByClass(USkeletalMeshComponent::StaticClass()))->SetMorphTarget("Key 1", -1.f);
			Cast<USkeletalMeshComponent>(MorphTargetTestCharacter->GetComponentByClass(USkeletalMeshComponent::StaticClass()))->SetMorphTarget("Key 2", 1.f);
		}
		else if (Face_DropDown->GetSelectedOption() == "2") {
			Cast<USkeletalMeshComponent>(MorphTargetTestCharacter->GetComponentByClass(USkeletalMeshComponent::StaticClass()))->SetMorphTarget("Key 1", 1.f);
			Cast<USkeletalMeshComponent>(MorphTargetTestCharacter->GetComponentByClass(USkeletalMeshComponent::StaticClass()))->SetMorphTarget("Key 2", -1.f);
		}
	}
}


void UWidget_DevMenu::OnEyesChanged(AActor_MorphTargetTest* MorphTargetTestCharacter)
{
	if (MorphTargetTestCharacter) {
		USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(MorphTargetTestCharacter->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
		UMaterialInstanceDynamic* PizzaEyes_DynamicMaterial = UMaterialInstanceDynamic::Create(PizzaEyes_Material, MorphTargetTestCharacter);
		UMaterialInstanceDynamic* RoundEyes_DynamicMaterial = UMaterialInstanceDynamic::Create(RoundEyes_Material, MorphTargetTestCharacter);

		if (Eyes_DropDown->GetSelectedOption() == "Pizza" && PizzaEyes_DynamicMaterial) {
			SkeletalMeshComponent->SetMaterial(0, PizzaEyes_DynamicMaterial);
		} else if (Eyes_DropDown->GetSelectedOption() == "Round" && RoundEyes_DynamicMaterial) {
			SkeletalMeshComponent->SetMaterial(0, RoundEyes_DynamicMaterial);
		}
	}
}


void UWidget_DevMenu::OnEyeColourChanged(AActor_MorphTargetTest* MorphTargetTestCharacter)
{

}