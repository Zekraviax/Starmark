#include "Widget_AvatarLibrary.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/PanelWidget.h"
#include "Components/UniformGridSlot.h"
#include "Kismet/GameplayStatics.h"
#include "SaveData_PlayerProfile.h"
#include "Starmark_GameInstance.h"
#include "Starmark_PlayerState.h"
#include "Widget_AvatarCreation.h"
#include "WidgetComponent_Avatar.h"


// ------------------------- Widget
void UWidget_AvatarLibrary::OnWidgetOpened()
{
	UE_LOG(LogTemp, Warning, TEXT("UWidget_AvatarLibrary / OnWidgetOpened / Begin function"));

	if (!GameInstanceReference) {
		GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	}

	UWidgetComponent_Avatar* LocalAvatarWidgetComponent_Reference;
	UWidgetTree* LibraryWidgetTree = this->WidgetTree;
	int Column = -1;
	int Row = 0;

	if (GameInstanceReference->PlayerSaveGameReference && AvatarWidgetComponent_Class) {
		// Populate Avatar team slots.
		TArray<UWidget*> FoundChildWidgetComponents = Cast<UPanelWidget>(LibraryWidgetTree->RootWidget)->GetAllChildren();

		// Add Avatars to the team slots, making sure that the index in the players team is preserved.
		for (int i = 0; i < GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Team.Num(); i++) {
			for (int j = 0; j < FoundChildWidgetComponents.Num(); j++) {
				if (Cast<UWidgetComponent_Avatar>(FoundChildWidgetComponents[j]) ) {
					LocalAvatarWidgetComponent_Reference = Cast<UWidgetComponent_Avatar>(FoundChildWidgetComponents[j]);
					if (LocalAvatarWidgetComponent_Reference->IndexInPlayerTeam == i) {
						if (GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Team[i].IndexInPlayerLibrary != i) {
							GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Team[i].IndexInPlayerLibrary = i;
						}

						LocalAvatarWidgetComponent_Reference->PairedWidget = this;
						LocalAvatarWidgetComponent_Reference->ApplyNewAvatarData(GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Team[i]);

						LocalAvatarWidgetComponent_Reference->CurrentFunction = E_AvatarWidgetComponent_Function::E_AddAvatarToChosenSlot;
						LocalAvatarWidgetComponent_Reference->RightClickMenuCommands.Empty();

						if (LocalAvatarWidgetComponent_Reference->AvatarData.AvatarName != "None") {
							LocalAvatarWidgetComponent_Reference->RightClickMenuCommands.Add(E_RightClickMenu_Commands::EditAvatar);
							LocalAvatarWidgetComponent_Reference->RightClickMenuCommands.Add(E_RightClickMenu_Commands::UnequipAvatar);
							LocalAvatarWidgetComponent_Reference->RightClickMenuCommands.Add(E_RightClickMenu_Commands::DeleteAvatar);
						}
						
						LocalAvatarWidgetComponent_Reference->RightClickMenuCommands.Add(E_RightClickMenu_Commands::Cancel);

						break;
					}
				}
			}
		}

		// Clear old widgets
		AvatarLibraryUniformGridPanel->ClearChildren();

		// Populate the Avatar Library (Avatars not currently in the players' team)
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

			LocalAvatarWidgetComponent_Reference->RightClickMenuCommands.Add(E_RightClickMenu_Commands::EquipAvatar);
			LocalAvatarWidgetComponent_Reference->RightClickMenuCommands.Add(E_RightClickMenu_Commands::EditAvatar);
			LocalAvatarWidgetComponent_Reference->RightClickMenuCommands.Add(E_RightClickMenu_Commands::DeleteAvatar);
			LocalAvatarWidgetComponent_Reference->RightClickMenuCommands.Add(E_RightClickMenu_Commands::Cancel);

			AvatarLibraryUniformGridPanel->AddChildToUniformGrid(LocalAvatarWidgetComponent_Reference, Row, Column);
		}

		// Create one move Avatar WidgetComponent that's used to add new Avatars to the Library
		LocalAvatarWidgetComponent_Reference = CreateWidget<UWidgetComponent_Avatar>(this, AvatarWidgetComponent_Class);
		LocalAvatarWidgetComponent_Reference->PairedWidget = this;
		LocalAvatarWidgetComponent_Reference->CurrentFunction = E_AvatarWidgetComponent_Function::E_CreateNewAvatarInLibrary;

		Column++;
		if (Column >= 4) {
			Column = 0;
			Row++;
		}

		LocalAvatarWidgetComponent_Reference->UpdateWidgetMaterials();
		LocalAvatarWidgetComponent_Reference->AvatarName->SetText(FText::FromString("Create New"));
		AvatarLibraryUniformGridPanel->AddChildToUniformGrid(LocalAvatarWidgetComponent_Reference, Row, Column);
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
	for (int j = 0; j < AvatarLibraryUniformGridPanel->GetChildrenCount(); j++) {
		Cast<UUniformGridSlot>(AvatarLibraryUniformGridPanel->GetChildAt(j)->Slot)->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	}

	// Bind AvatarSlotChangedDelegate
	//if (!Cast<APlayerController_Lobby>(GetWorld()->GetFirstPlayerController())->OnAvatarChangedSlotDelegate.Contains(this, FName("OnAvatarChangedSlotDelegateBroadcast")))
	//	Cast<APlayerController_Lobby>(GetWorld()->GetFirstPlayerController())->OnAvatarChangedSlotDelegate.AddDynamic(this, &UWidget_AvatarLibrary::OnAvatarChangedSlotDelegateBroadcast);
}


void UWidget_AvatarLibrary::UpdateAllAvatarsInTeam()
{
	UWidgetTree* LibraryWidgetTree = this->WidgetTree;
	TArray<UTexture*> QuestionMarkMaterials;
	TArray<UWidget*> FoundChildWidgetComponents = Cast<UPanelWidget>(LibraryWidgetTree->RootWidget)->GetAllChildren();
	QuestionMarkMaterial->GetUsedTextures(QuestionMarkMaterials, EMaterialQualityLevel::Medium, true, ERHIFeatureLevel::ES3_1, true);

	if (!GameInstanceReference) {
		GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	}
	
	for (int j = 0; j < FoundChildWidgetComponents.Num(); j++) {
		if (Cast<UWidgetComponent_Avatar>(FoundChildWidgetComponents[j])) {
			UWidgetComponent_Avatar* LocalAvatarWidgetComponent_Reference = Cast<UWidgetComponent_Avatar>(FoundChildWidgetComponents[j]);

			if (LocalAvatarWidgetComponent_Reference->IndexInPlayerTeam >= 0) {
				bool FoundAvatarInSlot = false;
				for (int k = 0; k < GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Team.Num(); k++) {
					if (GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Team[k].IndexInPlayerLibrary == LocalAvatarWidgetComponent_Reference->IndexInPlayerTeam) {
						FoundAvatarInSlot = true;
						LocalAvatarWidgetComponent_Reference->AvatarData = GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Team[k];
						break;
					}
				}

				LocalAvatarWidgetComponent_Reference->RightClickMenuCommands.Empty();	

				if (FoundAvatarInSlot) {
					LocalAvatarWidgetComponent_Reference->RightClickMenuCommands.Add(E_RightClickMenu_Commands::EditAvatar);
					LocalAvatarWidgetComponent_Reference->RightClickMenuCommands.Add(E_RightClickMenu_Commands::UnequipAvatar);

					LocalAvatarWidgetComponent_Reference->UpdateWidgetMaterials();
				} else {
					LocalAvatarWidgetComponent_Reference->AvatarData = FAvatar_Struct();
					LocalAvatarWidgetComponent_Reference->AvatarName->SetText(FText::FromString("Empty Slot"));
					LocalAvatarWidgetComponent_Reference->AvatarImage->SetBrushFromTexture(Cast<UTexture2D>(QuestionMarkMaterials[0]), true);
				}

				LocalAvatarWidgetComponent_Reference->RightClickMenuCommands.Add(E_RightClickMenu_Commands::Cancel);
			}
		}
	}
}


// ------------------------- Delegates
void UWidget_AvatarLibrary::BindAvatarCreatedDelegate(const UWidgetComponent_Avatar* AvatarWidgetComponentReference)
{
	AvatarWidgetComponentReference->AvatarCreationWidget_Reference->OnAvatarCreatedDelegate.AddDynamic(this, &UWidget_AvatarLibrary::OnAvatarCreatedDelegateBroadcast);
}


void UWidget_AvatarLibrary::OnAvatarCreatedDelegateBroadcast()
{
	OnWidgetOpened();
}


void UWidget_AvatarLibrary::OnAvatarChangedSlotDelegateBroadcast()
{
	const UWidgetTree* LibraryWidgetTree = this->WidgetTree;
	TArray<UWidget*> FoundChildWidgetComponents = Cast<UPanelWidget>(LibraryWidgetTree->RootWidget)->GetAllChildren();

	if (!GameInstanceReference) {
		GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	}

	GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Team.Empty();
	
	for (int i = 0; i < FoundChildWidgetComponents.Num(); i++) {
		if (Cast<UWidgetComponent_Avatar>(FoundChildWidgetComponents[i])) {
			UWidgetComponent_Avatar* AvatarWidgetComponentReference = Cast<UWidgetComponent_Avatar>(FoundChildWidgetComponents[i]);
			if (AvatarWidgetComponentReference->IndexInPlayerTeam >= 0) {
				GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Team.Insert(AvatarWidgetComponentReference->AvatarData, AvatarWidgetComponentReference->IndexInPlayerTeam);
				GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Library.Remove(AvatarWidgetComponentReference->AvatarData);
			}
		}
	}

	Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->SaveToCurrentProfile();
}