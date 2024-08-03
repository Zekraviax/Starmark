#include "Widget_AvatarSelection.h"

#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"
#include "SaveData_PlayerProfile.h"
#include "Starmark_GameInstance.h"
#include "Starmark_PlayerState.h"
#include "WidgetComponent_Avatar.h"


// ------------------------- Widget
void UWidget_AvatarSelection::OnWidgetOpened()
{
	const UStarmark_GameInstance* GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	AStarmark_PlayerState* PlayerStateReference = Cast<AStarmark_PlayerState>(GetOwningPlayerState());
	UWidgetComponent_Avatar* LocalAvatarWidgetComponent_Reference;
	const UWidgetTree* SelectionWidgetTree = this->WidgetTree;
	int Column = -1;
	int Row = 0;
	
	if (GameInstanceReference->PlayerSaveGameReference && AvatarWidgetComponent_Class) {
		// Populate Avatar Team Slots
		TArray<UWidget*> FoundChildWidgetComponents = Cast<UPanelWidget>(SelectionWidgetTree->RootWidget)->GetAllChildren();

		for (int i = 0; i < GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Team.Num(); i++) {
			for (int j = 0; j < FoundChildWidgetComponents.Num(); j++) {
				if (Cast<UWidgetComponent_Avatar>(FoundChildWidgetComponents[j])) {
					LocalAvatarWidgetComponent_Reference = Cast<UWidgetComponent_Avatar>(FoundChildWidgetComponents[j]);
					if (LocalAvatarWidgetComponent_Reference->IndexInPlayerTeam == i) {
						LocalAvatarWidgetComponent_Reference->ApplyNewAvatarData(GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Team[i]);
						LocalAvatarWidgetComponent_Reference->CurrentFunction = E_AvatarWidgetComponent_Function::E_AddAvatarToChosenSlot;
						break;
					}
				}
			}
		}

		// Clear old widgets
		AvatarLibraryUniformGridPanel->ClearChildren();

		// Populate Avatar Library
		for (int i = 0; i < GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Library.Num(); i++) {
			LocalAvatarWidgetComponent_Reference = CreateWidget<UWidgetComponent_Avatar>(this, AvatarWidgetComponent_Class);
			LocalAvatarWidgetComponent_Reference->ApplyNewAvatarData(GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Library[i]);

			Column++;
			if (Column >= 4) {
				Column = 0;
				Row++;
			}

			LocalAvatarWidgetComponent_Reference->PairedWidget = this;
			LocalAvatarWidgetComponent_Reference->CurrentFunction = E_AvatarWidgetComponent_Function::E_AddAvatarToChosenSlot;
			AvatarLibraryUniformGridPanel->AddChildToUniformGrid(LocalAvatarWidgetComponent_Reference, Row, Column);
		}

		// Create one move Avatar WidgetComponent that's used to add new Avatars to the Library
		//LocalAvatarWidgetComponent_Reference = CreateWidget<UWidgetComponent_Avatar>(this, AvatarWidgetComponent_Class);
		//LocalAvatarWidgetComponent_Reference->PairedWidget = this;
		//LocalAvatarWidgetComponent_Reference->CurrentFunction = E_AvatarWidgetComponent_Function::E_CreateNewAvatarInLibrary;

		//Column++;
		//if (Column >= 4) {
		//	Column = 0;
		//	Row++;
		//}

		//LocalAvatarWidgetComponent_Reference->UpdateWidgetMaterials();
		//LocalAvatarWidgetComponent_Reference->AvatarName->SetText(FText::FromString("Create New"));
		//AvatarLibraryUniformGridPanel->AddChildToUniformGrid(LocalAvatarWidgetComponent_Reference, Row, Column);
	}

	// Add hidden children to enforce the layout of the grid panel, if there are less than four children
	for (int k = 0; k < 3; k++) {
		Column++;
		if (Column >= 4) {
			Column = 0;
			Row++;
		}

		LocalAvatarWidgetComponent_Reference = CreateWidget<UWidgetComponent_Avatar>(this, AvatarWidgetComponent_Class);
		LocalAvatarWidgetComponent_Reference->SetVisibility(ESlateVisibility::Hidden);
		AvatarLibraryUniformGridPanel->AddChildToUniformGrid(LocalAvatarWidgetComponent_Reference, Row, Column);
	}

	// Adjust all children in the Uniform Grid Panel
	//for (int j = 0; j < AvatarLibraryUniformGridPanel->GetChildrenCount(); j++) {
	//	Cast<UUniformGridSlot>(AvatarLibraryUniformGridPanel->GetChildAt(j)->Slot)->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	//}
}