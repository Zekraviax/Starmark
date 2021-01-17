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
	TArray<FName> CombinationTypesRowNames, MoveEffectiveness_RowNames, AvatarResistances_RowNames;
	TMap<FName, FRealCurve*> MoveEffectiveness_BaseTypeChartRowMap, AvatarResistances_BaseTypeChartRowMap;
	FCharacter_CombinationTypes* CombinationType;
	FRealCurve* Value;
	int CurrentTypeEffectivenessValue = 1;

	// Get Row Names
	if (CombinationTypesRowNames.Num() <= 0) {
		CombinationTypesRowNames = CombinationTypesDataTable->GetRowNames();
	}
	if (MoveEffectiveness_BaseTypeChartRowMap.Num() <= 0) {
		MoveEffectiveness_BaseTypeChartRowMap = MoveEffectiveness_NumberedBaseTypeChartDataTable->GetRowMap();
	}

	if (AvatarResistances_BaseTypeChartRowMap.Num() <= 0) {
		AvatarResistances_BaseTypeChartRowMap = AvatarResistances_NumberedBaseTypeChartDataTable->GetRowMap();
	}

	if (PrimaryTypeDropdown->GetSelectedOption() != "N/A" && SecondaryTypeDropdown->GetSelectedOption() != "N/A"
		&& PrimaryTypeDropdown->GetSelectedOption() != SecondaryTypeDropdown->GetSelectedOption()) {

		int FirstTypeIndex = PrimaryTypeDropdown->GetSelectedIndex() - 1;
		int SecondTypeIndex = SecondaryTypeDropdown->GetSelectedIndex() - 1;

		MoveEffectiveness_BaseTypeChartRowMap.GetKeys(MoveEffectiveness_RowNames);
		AvatarResistances_BaseTypeChartRowMap.GetKeys(AvatarResistances_RowNames);

		// Reset Text Boxes
		for (auto& TypeTextBox : TypeTextsGridPanel->GetAllChildren()) {
			for (int i = 0; i < MoveEffectiveness_RowNames.Num(); i++) {
				if (Cast<UTextBlock>(TypeTextBox)->GetName() == (MoveEffectiveness_RowNames[i].ToString() + "Move")) {
					Cast<UTextBlock>(TypeTextBox)->SetText(FText::FromString("1"));
					//break;
				}
			}

			for (int i = 0; i < AvatarResistances_RowNames.Num(); i++) {
				if (Cast<UTextBlock>(TypeTextBox)->GetName() == (AvatarResistances_RowNames[i].ToString() + "Avatar")) {
					Cast<UTextBlock>(TypeTextBox)->SetText(FText::FromString("1"));
					//break;
				}
			}
		}		
		
		// Get Combination Name
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
				}
			}
		}


		for (auto& TypeTextBox : TypeTextsGridPanel->GetAllChildren()) {

			if (TypeTextBox->GetName() == "MoveText") {
				Cast<UTextBlock>(TypeTextBox)->SetText((FText::FromString(CombinationTypeString + " Move Effectiveness:")));
			} else if (TypeTextBox->GetName() == "AvatarText") {
				Cast<UTextBlock>(TypeTextBox)->SetText((FText::FromString(CombinationTypeString + " Avatar Resistances:")));
			} else {
				for (int i = 0; i < MoveEffectiveness_RowNames.Num(); i++) {
					if (Cast<UTextBlock>(TypeTextBox)->GetName() == (MoveEffectiveness_RowNames[i].ToString() + "Move")) {
						// Move Effectiveness

						Value = *MoveEffectiveness_BaseTypeChartRowMap.Find(MoveEffectiveness_RowNames[i]);
						FText Text = Cast<UTextBlock>(TypeTextBox)->GetText();

						float FirstMultiplier = FCString::Atof(*Text.ToString()) * Value->Eval(FirstTypeIndex, i);
						float SecondMultiplier = FirstMultiplier * Value->Eval(SecondTypeIndex, i);

						//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("%s First TypeIndex: %d"), *MoveEffectiveness_RowNames[i].ToString(), FirstTypeIndex));
						//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("%s Second TypeIndex: %d"), *MoveEffectiveness_RowNames[i].ToString(), SecondTypeIndex));

						//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("%s First Multiplier: x%f"), *MoveEffectiveness_RowNames[i].ToString(), FirstMultiplier));
						//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("%s Second Multiplier: x%f"), *MoveEffectiveness_RowNames[i].ToString(), SecondMultiplier));

						if (SecondMultiplier != 0.5f) {
							Text = FText::FromString(FString::SanitizeFloat(SecondMultiplier, 0));
						} else {
							Text = FText::FromString(FString::SanitizeFloat(SecondMultiplier, 1));
						}

						Cast<UTextBlock>(TypeTextBox)->SetText(FText::FromString(MoveEffectiveness_RowNames[i].ToString() + ": x" + Text.ToString()));
						//break;
					} else if (Cast<UTextBlock>(TypeTextBox)->GetName() == (AvatarResistances_RowNames[i].ToString() + "Avatar")) {
						// Avatar Resistances

						Value = *AvatarResistances_BaseTypeChartRowMap.Find(AvatarResistances_RowNames[i]);
						FText Text = Cast<UTextBlock>(TypeTextBox)->GetText();

						float FirstMultiplier = FCString::Atof(*Text.ToString()) * Value->Eval(FirstTypeIndex, i);
						float SecondMultiplier = FirstMultiplier * Value->Eval(SecondTypeIndex, i);

						GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("%s First TypeIndex: %d"), *AvatarResistances_RowNames[i].ToString(), FirstTypeIndex));
						GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("%s Second TypeIndex: %d"), *AvatarResistances_RowNames[i].ToString(), SecondTypeIndex));

						GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("%s First Multiplier: x%f"), *AvatarResistances_RowNames[i].ToString(), FirstMultiplier));
						GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("%s Second Multiplier: x%f"), *AvatarResistances_RowNames[i].ToString(), SecondMultiplier));

						if (SecondMultiplier != 0.5f) {
							Text = FText::FromString(FString::SanitizeFloat(SecondMultiplier, 0));
						} else {
							Text = FText::FromString(FString::SanitizeFloat(SecondMultiplier, 1));
						}

						Cast<UTextBlock>(TypeTextBox)->SetText(FText::FromString(AvatarResistances_RowNames[i].ToString() + ": x" + Text.ToString()));
					}
				}
			}
		}

		// Avatar Resistances
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