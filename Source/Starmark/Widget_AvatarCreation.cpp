#include "Widget_AvatarCreation.h"

#include "Player_SaveData.h"
#include "Starmark_PlayerState.h"


// ------------------------- Widget
void UWidget_AvatarCreation::PopulateDropDowns()
{
	FString ContextString;

	if (AvatarDataTable) {
		AvatarRowNames = AvatarDataTable->GetRowNames();

		for (int i = 0; i < AvatarRowNames.Num(); i++) {
			SpeciesDropDown->AddOption(AvatarRowNames[i].ToString());

			if (i == 0)
				SpeciesDropDown->SetSelectedOption(AvatarRowNames[i].ToString());
		}

		CurrentAvatar = *AvatarDataTable->FindRow<FAvatar_Struct>(FName(*SpeciesDropDown->GetSelectedOption()), ContextString);

		OnSpeciesDropDownChanged(SpeciesDropDown->GetSelectedOption());
		OnAvatarNicknameTextChanged(AvatarNameEditableText->GetText());
	}

	if (SimpleAttacksDataTable) {
		SimpleAttacksRowNames = SimpleAttacksDataTable->GetRowNames();

		for (int i = 0; i < SimpleAttacksRowNames.Num(); i++) {
			MoveOneDropDown->AddOption(SimpleAttacksRowNames[i].ToString());
			MoveTwoDropDown->AddOption(SimpleAttacksRowNames[i].ToString());
			MoveThreeDropDown->AddOption(SimpleAttacksRowNames[i].ToString());
			MoveFourDropDown->AddOption(SimpleAttacksRowNames[i].ToString());
		}
	}

	if (ComplexAttacksDataTable) {
		ComplexAttacksRowNames = ComplexAttacksDataTable->GetRowNames();

		for (int i = 0; i < ComplexAttacksRowNames.Num(); i++) {
			MoveOneDropDown->AddOption(ComplexAttacksRowNames[i].ToString());
			MoveTwoDropDown->AddOption(ComplexAttacksRowNames[i].ToString());
			MoveThreeDropDown->AddOption(ComplexAttacksRowNames[i].ToString());
			MoveFourDropDown->AddOption(ComplexAttacksRowNames[i].ToString());
		}
	}

	OnAbilityDropDownChanged(AbilityDropDown->GetSelectedOption());
	OnAccessoryDropDownChanged(AccessoryDropDown->GetSelectedOption());
	OnMoveOneDropDownChanged(MoveOneDropDown->GetSelectedOption());
	OnMoveTwoDropDownChanged(MoveTwoDropDown->GetSelectedOption());
	OnMoveThreeDropDownChanged(MoveThreeDropDown->GetSelectedOption());
	OnMoveFourDropDownChanged(MoveFourDropDown->GetSelectedOption());
}


void UWidget_AvatarCreation::OnAvatarNicknameTextChanged(FText NewNickname)
{
	if (NewNickname.IsEmpty())
		AvatarNameText->SetText(FText::FromString("No Nickname"));
	else 
		AvatarNameText->SetText(NewNickname);
}


void UWidget_AvatarCreation::OnSpeciesDropDownChanged(FString Option)
{
	if (Option.Len() == 0 || Option == "") {
		SpeciesText->SetText(FText::FromString("Species: None"));
		ElementsText->SetText(FText::FromString("Elements: None"));
	} else {
		FString ContextString;
		CurrentAvatar = *AvatarDataTable->FindRow<FAvatar_Struct>(FName(*Option), ContextString);

		SpeciesText->SetText(FText::FromString("Species: " + Option));
		ElementsText->SetText(FText::FromString("Elements: " + UEnum::GetDisplayValueAsText(CurrentAvatar.PrimaryType).ToString()));
	}
}

void UWidget_AvatarCreation::OnAbilityDropDownChanged(FString Option)
{
	if (Option.Len() == 0 || Option == "")
		AbilityText->SetText(FText::FromString("Ability: None"));
	else
		AbilityText->SetText(FText::FromString("Ability: " + Option));
}

void UWidget_AvatarCreation::OnAccessoryDropDownChanged(FString Option)
{
	if (Option.Len() == 0 || Option == "" || Option == "None")
		AccessoryText->SetText(FText::FromString("No Accessory"));
	else
		AccessoryText->SetText(FText::FromString(Option));
}


void UWidget_AvatarCreation::OnMoveOneDropDownChanged(FString Option)
{
	if (Option.Len() == 0 || Option == "")
		MoveOneNameText->SetText(FText::FromString("Move One: None"));
	else {
		FString ContextString;

		for (int i = 0; i < SimpleAttacksRowNames.Num(); i++) {
			if (Option == SimpleAttacksRowNames[i].ToString()) {
				CurrentAvatar.CurrentAttacks.Insert(*SimpleAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(*Option), ContextString), 0);
				MoveOneNameText->SetText(FText::FromString(SimpleAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(*Option), ContextString)->Name));
				break;
			}
		}

		for (int i = 0; i < ComplexAttacksRowNames.Num(); i++) {
			if (Option == ComplexAttacksRowNames[i].ToString()) {
				CurrentAvatar.CurrentAttacks.Insert(*ComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(*Option), ContextString), 0);
				MoveOneNameText->SetText(FText::FromString(ComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(*Option), ContextString)->Name));
				break;
			}
		}
	}
}


void UWidget_AvatarCreation::OnMoveTwoDropDownChanged(FString Option)
{
	if (Option.Len() == 0 || Option == "")
		MoveTwoNameText->SetText(FText::FromString("Move Two: None"));
	else {
		FString ContextString;

		for (int i = 0; i < SimpleAttacksRowNames.Num(); i++) {
			if (Option == SimpleAttacksRowNames[i].ToString()) {
				CurrentAvatar.CurrentAttacks.Insert(*SimpleAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(*Option), ContextString), 0);
				MoveTwoNameText->SetText(FText::FromString(SimpleAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(*Option), ContextString)->Name));
				break;
			}
		}

		for (int i = 0; i < ComplexAttacksRowNames.Num(); i++) {
			if (Option == ComplexAttacksRowNames[i].ToString()) {
				CurrentAvatar.CurrentAttacks.Insert(*ComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(*Option), ContextString), 0);
				MoveTwoNameText->SetText(FText::FromString(ComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(*Option), ContextString)->Name));
				break;
			}
		}
	}
}


void UWidget_AvatarCreation::OnMoveThreeDropDownChanged(FString Option)
{
	if (Option.Len() == 0 || Option == "")
		MoveThreeNameText->SetText(FText::FromString("Move Three: None"));
	else {
		FString ContextString;

		for (int i = 0; i < SimpleAttacksRowNames.Num(); i++) {
			if (Option == SimpleAttacksRowNames[i].ToString()) {
				CurrentAvatar.CurrentAttacks.Insert(*SimpleAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(*Option), ContextString), 0);
				MoveThreeNameText->SetText(FText::FromString(SimpleAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(*Option), ContextString)->Name));
				break;
			}
		}

		for (int i = 0; i < ComplexAttacksRowNames.Num(); i++) {
			if (Option == ComplexAttacksRowNames[i].ToString()) {
				CurrentAvatar.CurrentAttacks.Insert(*ComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(*Option), ContextString), 0);
				MoveThreeNameText->SetText(FText::FromString(ComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(*Option), ContextString)->Name));
				break;
			}
		}
	}
}


void UWidget_AvatarCreation::OnMoveFourDropDownChanged(FString Option)
{
	if (Option.Len() == 0 || Option == "")
		MoveFourNameText->SetText(FText::FromString("Move Four: None"));
	else {
		FString ContextString;

		for (int i = 0; i < SimpleAttacksRowNames.Num(); i++) {
			if (Option == SimpleAttacksRowNames[i].ToString()) {
				CurrentAvatar.CurrentAttacks.Insert(*SimpleAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(*Option), ContextString), 0);
				MoveFourNameText->SetText(FText::FromString(SimpleAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(*Option), ContextString)->Name));
				break;
			}
		}

		for (int i = 0; i < ComplexAttacksRowNames.Num(); i++) {
			if (Option == ComplexAttacksRowNames[i].ToString()) {
				CurrentAvatar.CurrentAttacks.Insert(*ComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(*Option), ContextString), 0);
				MoveFourNameText->SetText(FText::FromString(ComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(*Option), ContextString)->Name));
				break;
			}
		}
	}
}


void UWidget_AvatarCreation::OnSaveButtonPressed()
{
	if (!PlayerStateReference)
		PlayerStateReference = Cast<AStarmark_PlayerState>(GetOwningPlayerState());

	PlayerStateReference->PlayerProfileReference->AvatarLibrary.Add(CurrentAvatar);

	if (OnAvatarCreatedDelegate.IsBound())
		OnAvatarCreatedDelegate.Broadcast();

	PlayerStateReference->SaveToCurrentProfile();

	this->RemoveFromParent();
}


void UWidget_AvatarCreation::OnExitButtonPressed()
{
	this->RemoveFromParent();
}