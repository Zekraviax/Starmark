#include "Widget_AvatarLibrary.h"

#include "Blueprint/WidgetTree.h"
#include "Components/PanelWidget.h"
#include "Components/UniformGridSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "PlayerController_Lobby.h"
#include "Starmark_GameInstance.h"
#include "Starmark_PlayerState.h"
#include "WidgetBlueprintLibrary.h"
#include "WidgetComponent_Avatar.h"
#include "Widget_AvatarCreation.h"


// ------------------------- Widget
void UWidget_AvatarLibrary::OnWidgetOpened()
{
	UStarmark_GameInstance* GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	AStarmark_PlayerState* PlayerStateReference = Cast<AStarmark_PlayerState>(GetOwningPlayerState());
	UWidgetTree* LibraryWidgetTree = this->WidgetTree;
	TArray<UWidget*> FoundChildWidgetComponents;
	int Column = -1;
	int Row = 0;

	if (PlayerStateReference->PlayerProfileReference && AvatarWidgetComponent_Class) {
		// Populate Avatar Team Slots
		FoundChildWidgetComponents = Cast<UPanelWidget>(LibraryWidgetTree->RootWidget)->GetAllChildren();

		for (int i = 0; i < PlayerStateReference->PlayerProfileReference->CurrentAvatarTeam.Num(); i++) {
			for (int j = 0; j < FoundChildWidgetComponents.Num(); j++) {
				if (Cast<UWidgetComponent_Avatar>(FoundChildWidgetComponents[j]) ) {
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
		AvatarWidgetComponent_Reference = CreateWidget<UWidgetComponent_Avatar>(this, AvatarWidgetComponent_Class);
		AvatarWidgetComponent_Reference->PairedWidget = this;
		AvatarWidgetComponent_Reference->CurrentFunction = E_AvatarWidgetComponent_Function::E_CreateNewAvatarInLibrary;

		Column++;
		if (Column >= 4) {
			Column = 0;
			Row++;
		}

		AvatarWidgetComponent_Reference->UpdateWidgetMaterials();
		AvatarWidgetComponent_Reference->AvatarName->SetText(FText::FromString("Create New"));
		AvatarLibraryUniformGridPanel->AddChildToUniformGrid(AvatarWidgetComponent_Reference, Row, Column);
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
	for (int j = 0; j < AvatarLibraryUniformGridPanel->GetChildrenCount(); j++) {
		Cast<UUniformGridSlot>(AvatarLibraryUniformGridPanel->GetChildAt(j)->Slot)->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	}

	// Bind AvatarSlotChangedDelegate
	if (!Cast<APlayerController_Lobby>(GetWorld()->GetFirstPlayerController())->OnAvatarChangedSlotDelegate.Contains(this, FName("OnAvatarChangedSlotDelegateBroadcast")))
		Cast<APlayerController_Lobby>(GetWorld()->GetFirstPlayerController())->OnAvatarChangedSlotDelegate.AddDynamic(this, &UWidget_AvatarLibrary::OnAvatarChangedSlotDelegateBroadcast);
}


// ------------------------- Delegates
void UWidget_AvatarLibrary::BindAvatarCreatedDelegate(UWidgetComponent_Avatar* AvatarWidgetComponentReference)
{
	AvatarWidgetComponentReference->AvatarCreationWidget_Reference->OnAvatarCreatedDelegate.AddDynamic(this, &UWidget_AvatarLibrary::OnAvatarCreatedDelegateBroadcast);
}


void UWidget_AvatarLibrary::OnAvatarCreatedDelegateBroadcast()
{
	OnWidgetOpened();
}


void UWidget_AvatarLibrary::OnAvatarChangedSlotDelegateBroadcast()
{
	AStarmark_PlayerState* PlayerStateReference = Cast<AStarmark_PlayerState>(GetOwningPlayer()->PlayerState);
	UWidgetTree* LibraryWidgetTree = this->WidgetTree;
	TArray<UWidget*> FoundChildWidgetComponents;

	PlayerStateReference->PlayerProfileReference->CurrentAvatarTeam.Empty();

	FoundChildWidgetComponents = Cast<UPanelWidget>(LibraryWidgetTree->RootWidget)->GetAllChildren();

	for (int i = 0; i < FoundChildWidgetComponents.Num(); i++) {
		if (Cast<UWidgetComponent_Avatar>(FoundChildWidgetComponents[i])) {
			UWidgetComponent_Avatar* AvatarWidgetComponentReference = Cast<UWidgetComponent_Avatar>(FoundChildWidgetComponents[i]);
			if (AvatarWidgetComponentReference->IndexInPlayerTeam >= 0) {
				PlayerStateReference->PlayerProfileReference->CurrentAvatarTeam.Insert(AvatarWidgetComponentReference->AvatarData, AvatarWidgetComponentReference->IndexInPlayerTeam);
				PlayerStateReference->PlayerProfileReference->AvatarLibrary.Remove(AvatarWidgetComponentReference->AvatarData);
			}
		}
	}

	PlayerStateReference->SaveToCurrentProfile();
}