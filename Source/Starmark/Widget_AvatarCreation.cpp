#include "Widget_AvatarCreation.h"

#include "Player_SaveData.h"
#include "Starmark_PlayerState.h"


// ------------------------- Widget
void UWidget_AvatarCreation::PopulateDropDowns()
{
	FString ContextString;

	// Avatars
	if (AvatarDataTable) {
		AvatarRowNames = AvatarDataTable->GetRowNames();

		for (int i = 0; i < AvatarRowNames.Num(); i++) {
			SpeciesDropDown->AddOption(AvatarDataTable->FindRow<FAvatar_Struct>(FName(*AvatarRowNames[i].ToString()), ContextString)->AvatarName);

			if (i == 0) {
				SpeciesDropDown->SetSelectedOption(AvatarRowNames[i].ToString());
				SpeciesDropDown->SetIsEnabled(true);
				CurrentAvatar = *AvatarDataTable->FindRow<FAvatar_Struct>(FName(*AvatarRowNames[i].ToString()), ContextString);
			}
		}

		OnSpeciesDropDownChanged(SpeciesDropDown->GetSelectedOption());
		OnAvatarNicknameTextChanged(AvatarNameEditableText->GetText());
	}

	// Avatars' Attacks
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

		for (int i = 1; i < ComplexAttacksRowNames.Num(); i++) {
			MoveOneDropDown->AddOption(ComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(ComplexAttacksRowNames[i]), ContextString)->Name);
			MoveTwoDropDown->AddOption(ComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(ComplexAttacksRowNames[i]), ContextString)->Name);
			MoveThreeDropDown->AddOption(ComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(ComplexAttacksRowNames[i]), ContextString)->Name);
			MoveFourDropDown->AddOption(ComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(ComplexAttacksRowNames[i]), ContextString)->Name);

			ComplexAttacksDisplayNames.Add(FName(ComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(ComplexAttacksRowNames[i]), ContextString)->Name));
		}
	}

	// Abilities
	if (AbilitiesDataTable) {
		if (AbilityRowNames.Num() <= 0)
			AbilityRowNames = AbilitiesDataTable->GetRowNames();

		for (int i = 0; i < AbilityRowNames.Num(); i++) {
			AbilityDropDown->AddOption(AbilityRowNames[i].ToString());
		}
	}

	// Items
	if (ItemsDataTable) {
		if (ItemRowNames.Num() <= 0)
			ItemRowNames = ItemsDataTable->GetRowNames();

		for (int i = 0; i < ItemRowNames.Num(); i++) {
			AccessoryDropDown->AddOption(ItemRowNames[i].ToString());
		}
	}

	OnAbilityDropDownChanged(AbilityDropDown->GetSelectedOption());
	OnAccessoryDropDownChanged(AccessoryDropDown->GetSelectedOption());
	OnMoveOneDropDownChanged(MoveOneDropDown->GetSelectedOption());
	OnMoveTwoDropDownChanged(MoveTwoDropDown->GetSelectedOption());
	OnMoveThreeDropDownChanged(MoveThreeDropDown->GetSelectedOption());
	OnMoveFourDropDownChanged(MoveFourDropDown->GetSelectedOption());
}


void UWidget_AvatarCreation::PopulateDropDownsWithAvatarData(FAvatar_Struct Avatar)
{
	PopulateDropDowns();

	// Species
	for (int a = 0; a < SpeciesDropDown->GetOptionCount(); a++) {
		if (Avatar.AvatarName == SpeciesDropDown->GetOptionAtIndex(a)) {
			SpeciesDropDown->SetSelectedIndex(a);
			OnSpeciesDropDownChanged(SpeciesDropDown->GetSelectedOption());
			break;
		}
	}

	// To-Do: Nickname

	// Moves
	for (int b = 0; b < MoveOneDropDown->GetOptionCount(); b++) {
		if (Avatar.CurrentAttacks.IsValidIndex(0)) {
			if (Avatar.CurrentAttacks[0].Name == MoveOneDropDown->GetOptionAtIndex(b)) {
				MoveOneDropDown->SetSelectedIndex(b);
			}
		}

		if (Avatar.CurrentAttacks.IsValidIndex(1)) {
			if (Avatar.CurrentAttacks[1].Name == MoveTwoDropDown->GetOptionAtIndex(b)) {
				MoveTwoDropDown->SetSelectedIndex(b);
			}
		}


		if (Avatar.CurrentAttacks.IsValidIndex(2)) {
			if (Avatar.CurrentAttacks[2].Name == MoveThreeDropDown->GetOptionAtIndex(b)) {
				MoveThreeDropDown->SetSelectedIndex(b);
			}
		}

		if (Avatar.CurrentAttacks.IsValidIndex(3)) {
			if (Avatar.CurrentAttacks[3].Name == MoveFourDropDown->GetOptionAtIndex(b)) {
				MoveFourDropDown->SetSelectedIndex(b);
			}
		}
	}

	// Ability
	for (int c = 0; c < AbilityDropDown->GetOptionCount(); c++) {
		if (Avatar.Ability.Name == AbilityDropDown->GetOptionAtIndex(c)) {
			AbilityDropDown->SetSelectedIndex(c);
			OnAbilityDropDownChanged(SpeciesDropDown->GetSelectedOption());
			break;
		}
	}
	
	// To-Do: Item

	OnMoveOneDropDownChanged(MoveOneDropDown->GetSelectedOption());
	OnMoveTwoDropDownChanged(MoveTwoDropDown->GetSelectedOption());
	OnMoveThreeDropDownChanged(MoveThreeDropDown->GetSelectedOption());
	OnMoveFourDropDownChanged(MoveFourDropDown->GetSelectedOption());

	CurrentAvatar = Avatar;
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
		for (int i = 0; i < AvatarRowNames.Num(); i++) {
			if (AvatarDataTable->FindRow<FAvatar_Struct>(FName(*AvatarRowNames[i].ToString()), ContextString)->AvatarName == SpeciesDropDown->GetSelectedOption()) {
				CurrentAvatar = *AvatarDataTable->FindRow<FAvatar_Struct>(FName(*AvatarRowNames[i].ToString()), ContextString);
				break;
			}
		}

		SpeciesText->SetText(FText::FromString("Species: " + Option));
		ElementsText->SetText(FText::FromString("Elements: " + UEnum::GetDisplayValueAsText(CurrentAvatar.PrimaryType).ToString()));
	}
}

void UWidget_AvatarCreation::OnAbilityDropDownChanged(FString Option)
{
	if (Option.Len() == 0 || Option == "")
		AbilityText->SetText(FText::FromString("Ability: None"));
	else {
		AbilityText->SetText(FText::FromString("Ability: " + Option));

		for (int i = 0; i < AbilityRowNames.Num(); i++) {
			if (AbilitiesDataTable->FindRow<FAvatar_AbilityStruct>(FName(*AbilityRowNames[i].ToString()), ContextString)->Name == AbilityDropDown->GetSelectedOption()) {
				CurrentAvatar.Ability = *AbilitiesDataTable->FindRow<FAvatar_AbilityStruct>(FName(*AbilityRowNames[i].ToString()), ContextString);
				break;
			}
		}
	}
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
		MoveOneNameText->SetText(FText::FromString("None"));
	else {
		FString ContextString;

		if (Option == "None") {
			if (CurrentAvatar.CurrentAttacks.IsValidIndex(0))
				CurrentAvatar.CurrentAttacks.RemoveAt(0);
			
			MoveOneNameText->SetText(FText::FromString("None"));
		} else {
			for (int i = 0; i < SimpleAttacksRowNames.Num(); i++) {
				if (Option == SimpleAttacksRowNames[i].ToString()) {
					MoveOneNameText->SetText(FText::FromString(SimpleAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(*Option), ContextString)->Name));
					break;
				}
			}

			for (int i = 0; i < ComplexAttacksRowNames.Num(); i++) {
				if (Option == ComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(ComplexAttacksRowNames[i]), ContextString)->Name) {
					MoveOneNameText->SetText(FText::FromString(ComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(ComplexAttacksRowNames[i]), ContextString)->Name));
					break;
				}
			}
		}
	}
}


void UWidget_AvatarCreation::OnMoveTwoDropDownChanged(FString Option)
{
	if (Option.Len() == 0 || Option == "")
		MoveTwoNameText->SetText(FText::FromString("None"));
	else {
		FString ContextString;

		for (int i = 0; i < SimpleAttacksRowNames.Num(); i++) {
			if (Option == SimpleAttacksRowNames[i].ToString()) {
				MoveTwoNameText->SetText(FText::FromString(SimpleAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(*Option), ContextString)->Name));
				break;
			}
		}

		for (int i = 0; i < ComplexAttacksRowNames.Num(); i++) {
			if (Option == ComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(ComplexAttacksRowNames[i]), ContextString)->Name) {
				MoveTwoNameText->SetText(FText::FromString(ComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(ComplexAttacksRowNames[i]), ContextString)->Name));
				break;
			}
		}
	}
}


void UWidget_AvatarCreation::OnMoveThreeDropDownChanged(FString Option)
{
	if (Option.Len() == 0 || Option == "")
		MoveThreeNameText->SetText(FText::FromString("None"));
	else {
		FString ContextString;

		for (int i = 0; i < SimpleAttacksRowNames.Num(); i++) {
			if (Option == SimpleAttacksRowNames[i].ToString()) {
				MoveThreeNameText->SetText(FText::FromString(SimpleAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(*Option), ContextString)->Name));
				break;
			}
		}

		for (int i = 0; i < ComplexAttacksRowNames.Num(); i++) {
			if (Option == ComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(ComplexAttacksRowNames[i]), ContextString)->Name) {
				MoveThreeNameText->SetText(FText::FromString(ComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(ComplexAttacksRowNames[i]), ContextString)->Name));
				break;
			}
		}
	}
}


void UWidget_AvatarCreation::OnMoveFourDropDownChanged(const FString Option)
{
	if (Option.Len() == 0 || Option == "")
		MoveFourNameText->SetText(FText::FromString("None"));
	else {
		const FString ContextString;
		for (int i = 0; i < SimpleAttacksRowNames.Num(); i++) {
			if (Option == SimpleAttacksRowNames[i].ToString()) {
				MoveFourNameText->SetText(FText::FromString(SimpleAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(*Option), ContextString)->Name));
				break;
			}
		}

		for (int i = 0; i < ComplexAttacksRowNames.Num(); i++) {
			if (Option == ComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(ComplexAttacksRowNames[i]), ContextString)->Name) {
				MoveFourNameText->SetText(FText::FromString(ComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(FName(ComplexAttacksRowNames[i]), ContextString)->Name));
				break;
			}
		}
	}
}


void UWidget_AvatarCreation::OnSaveButtonPressed()
{
	if (!PlayerStateReference) {
		PlayerStateReference = Cast<AStarmark_PlayerState>(GetOwningPlayerState());
	}

	// Set Attacks arrays
	CurrentAvatar.Attacks.Empty();
	CurrentAvatar.CurrentAttacks.Empty();
	FAvatar_AttackStruct emptyAttack;
	CurrentAvatar.CurrentAttacks.Init(emptyAttack, 4);

	FString ContextString;
	FAvatar_AttackStruct CurrentAttack;

	ComplexAttacksRowNames.RemoveAt(0);

	for (int i = 0; i < ComplexAttacksRowNames.Num(); i++) {
		UE_LOG(LogTemp, Warning, TEXT("OnSaveButtonPressed / Attack Name at index %d: %s"), i, *ComplexAttacksRowNames[i].ToString());

		if (SimpleAttacksRowNames.IsValidIndex(i)) {
			if (SimpleAttacksRowNames[i].ToString() == MoveOneNameText->GetText().ToString()) {
				CurrentAvatar.CurrentAttacks[0] = *SimpleAttacksDataTable->FindRow<FAvatar_AttackStruct>(SimpleAttacksRowNames[i], ContextString);
			} else if (SimpleAttacksRowNames[i].ToString() == MoveTwoNameText->GetText().ToString()) {
				CurrentAvatar.CurrentAttacks[1] = *SimpleAttacksDataTable->FindRow<FAvatar_AttackStruct>(SimpleAttacksRowNames[i], ContextString);
			} else if (SimpleAttacksRowNames[i].ToString() == MoveThreeNameText->GetText().ToString()) {
				CurrentAvatar.CurrentAttacks[2] = *SimpleAttacksDataTable->FindRow<FAvatar_AttackStruct>(SimpleAttacksRowNames[i], ContextString);
			} else if (SimpleAttacksRowNames[i].ToString() == MoveFourNameText->GetText().ToString()) {
				CurrentAvatar.CurrentAttacks[3] = *SimpleAttacksDataTable->FindRow<FAvatar_AttackStruct>(SimpleAttacksRowNames[i], ContextString);
			}
		}

		if (ComplexAttacksDisplayNames.IsValidIndex(i)) {
			if (ComplexAttacksDisplayNames[i].ToString() == MoveOneNameText->GetText().ToString()) {
				CurrentAvatar.CurrentAttacks[0] = *ComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(ComplexAttacksRowNames[i], ContextString);
			} else if (ComplexAttacksDisplayNames[i].ToString() == MoveTwoNameText->GetText().ToString()) {
				CurrentAvatar.CurrentAttacks[1] = *ComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(ComplexAttacksRowNames[i], ContextString);
			} else if (ComplexAttacksDisplayNames[i].ToString() == MoveThreeNameText->GetText().ToString()) {
				CurrentAvatar.CurrentAttacks[2] = *ComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(ComplexAttacksRowNames[i], ContextString);
			} else if (ComplexAttacksDisplayNames[i].ToString() == MoveFourNameText->GetText().ToString()) {
				CurrentAvatar.CurrentAttacks[3] = *ComplexAttacksDataTable->FindRow<FAvatar_AttackStruct>(ComplexAttacksRowNames[i], ContextString);
			}
		}
	}

	if (!IsEditingExistingAvatar) {
		CurrentAvatar.IndexInPlayerLibrary = (PlayerStateReference->PlayerProfileReference->AvatarLibrary.Num() * -1) - 1;
		PlayerStateReference->PlayerProfileReference->AvatarLibrary.Add(CurrentAvatar);
	} else {
		// Update the avatar if it's in the player's team
		for (int i = 0; i < PlayerStateReference->PlayerProfileReference->CurrentAvatarTeam.Num(); i++) {
			if (PlayerStateReference->PlayerProfileReference->CurrentAvatarTeam[i].IndexInPlayerLibrary == CurrentAvatar.IndexInPlayerLibrary) {
				PlayerStateReference->PlayerProfileReference->CurrentAvatarTeam[i] = CurrentAvatar;
				break;
			}
		}

		// Update the avatar if it's in the player's library
		for (int i = 0; i < PlayerStateReference->PlayerProfileReference->AvatarLibrary.Num(); i++) {
			if (PlayerStateReference->PlayerProfileReference->AvatarLibrary[i].IndexInPlayerLibrary == CurrentAvatar.IndexInPlayerLibrary) {
				PlayerStateReference->PlayerProfileReference->AvatarLibrary[i] = CurrentAvatar;
				break;
			}
		}
	}

	PlayerStateReference->SaveToCurrentProfile();

	if (OnAvatarCreatedDelegate.IsBound()) {
		OnAvatarCreatedDelegate.Broadcast();
	}

	this->RemoveFromParent();
}


void UWidget_AvatarCreation::OnExitButtonPressed()
{
	this->RemoveFromParent();
}