#include "Widget_AvatarSelection.h"

#include "Blueprint/WidgetTree.h"
#include "Components/UniformGridSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Starmark_GameInstance.h"
#include "Starmark_PlayerState.h"
#include "WidgetComponent_Avatar.h"


// ------------------------- Widget
void UWidget_AvatarSelection::OnWidgetOpened()
{
	UStarmark_GameInstance* GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	AStarmark_PlayerState* PlayerStateReference = Cast<AStarmark_PlayerState>(GetOwningPlayerState());
	UWidgetTree* SelectionWidgetTree = this->WidgetTree;
	TArray<UWidget*> FoundChildWidgetComponents;
	int Column = -1;
	int Row = 0;

	if (!PlayerStateReference->PlayerProfileReference) 
		PlayerStateReference->PlayerProfileReference = GameInstanceReference->CurrentProfileReference;

	if (PlayerStateReference->PlayerProfileReference && AvatarWidgetComponent_Class) {
		// Populate Avatar Team Slots
		FoundChildWidgetComponents = Cast<UPanelWidget>(SelectionWidgetTree->RootWidget)->GetAllChildren();

		for (int i = 0; i < PlayerStateReference->PlayerProfileReference->CurrentAvatarTeam.Num(); i++) {
			for (int j = 0; j < FoundChildWidgetComponents.Num(); j++) {
				if (Cast<UWidgetComponent_Avatar>(FoundChildWidgetComponents[j])) {
					UWidgetComponent_Avatar* AvatarWidgetComponent_Reference = Cast<UWidgetComponent_Avatar>(FoundChildWidgetComponents[j]);
					if (AvatarWidgetComponent_Reference->IndexInPlayerTeam == i) {
						AvatarWidgetComponent_Reference->ApplyNewAvatarData(PlayerStateReference->PlayerProfileReference->CurrentAvatarTeam[i]);
						AvatarWidgetComponent_Reference->CurrentFunction = E_AvatarWidgetComponent_Function::E_AddAvatarToChosenSlot;
						break;
					}
				}
			}
		}

		// Clear old widgets
		AvatarLibraryUniformGridPanel->ClearChildren();

		// Populate Avatar Library
		for (int i = 0; i < PlayerStateReference->PlayerProfileReference->AvatarLibrary.Num(); i++) {
			AvatarWidgetComponent_Reference = CreateWidget<UWidgetComponent_Avatar>(this, AvatarWidgetComponent_Class);
			AvatarWidgetComponent_Reference->ApplyNewAvatarData(PlayerStateReference->PlayerProfileReference->AvatarLibrary[i]);

			Column++;
			if (Column >= 4) {
				Column = 0;
				Row++;
			}

			AvatarWidgetComponent_Reference->PairedWidget = this;
			AvatarWidgetComponent_Reference->CurrentFunction = E_AvatarWidgetComponent_Function::E_AddAvatarToChosenSlot;
			AvatarLibraryUniformGridPanel->AddChildToUniformGrid(AvatarWidgetComponent_Reference, Row, Column);
		}

		// Create one move Avatar WidgetComponent that's used to add new Avatars to the Library
		//AvatarWidgetComponent_Reference = CreateWidget<UWidgetComponent_Avatar>(this, AvatarWidgetComponent_Class);
		//AvatarWidgetComponent_Reference->PairedWidget = this;
		//AvatarWidgetComponent_Reference->CurrentFunction = E_AvatarWidgetComponent_Function::E_CreateNewAvatarInLibrary;

		//Column++;
		//if (Column >= 4) {
		//	Column = 0;
		//	Row++;
		//}

		//AvatarWidgetComponent_Reference->UpdateWidgetMaterials();
		//AvatarWidgetComponent_Reference->AvatarName->SetText(FText::FromString("Create New"));
		//AvatarLibraryUniformGridPanel->AddChildToUniformGrid(AvatarWidgetComponent_Reference, Row, Column);
	}

	// Add hidden children to enforce the layout of the grid panel, if there are less than four children
	for (int k = 0; k < 3; k++) {
		Column++;
		if (Column >= 4) {
			Column = 0;
			Row++;
		}

		AvatarWidgetComponent_Reference = CreateWidget<UWidgetComponent_Avatar>(this, AvatarWidgetComponent_Class);
		AvatarWidgetComponent_Reference->SetVisibility(ESlateVisibility::Hidden);
		AvatarLibraryUniformGridPanel->AddChildToUniformGrid(AvatarWidgetComponent_Reference, Row, Column);
	}

	// Adjust all children in the Uniform Grid Panel
	//for (int j = 0; j < AvatarLibraryUniformGridPanel->GetChildrenCount(); j++) {
	//	Cast<UUniformGridSlot>(AvatarLibraryUniformGridPanel->GetChildAt(j)->Slot)->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	//}
}