#include "Widget_AvatarLibrary.h"

#include "Components/UniformGridSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Starmark_GameInstance.h"
#include "Starmark_PlayerState.h"
#include "WidgetComponent_Avatar.h"
#include "Widget_AvatarCreation.h"


// ------------------------- Widget
void UWidget_AvatarLibrary::OnWidgetOpened()
{
	UStarmark_GameInstance* GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	AStarmark_PlayerState* PlayerStateReference = Cast<AStarmark_PlayerState>(GetOwningPlayerState());
	int Column = -1;
	int Row = 0;

	if (PlayerStateReference->PlayerProfileReference && AvatarWidgetComponent_Class) {
		// Clear old widgets
		AvatarLibraryUniformGridPanel->ClearChildren();

		for (int i = 0; i < PlayerStateReference->PlayerProfileReference->AvatarLibrary.Num(); i++) {
			AvatarWidgetComponent_Reference = CreateWidget<UWidgetComponent_Avatar>(this, AvatarWidgetComponent_Class);
			AvatarWidgetComponent_Reference->AvatarData = PlayerStateReference->PlayerProfileReference->AvatarLibrary[i];
			AvatarWidgetComponent_Reference->PairedWidget = this;

			if (AvatarWidgetComponent_Reference->AvatarData.DefaultImage)
				AvatarWidgetComponent_Reference->AvatarMaterial = AvatarWidgetComponent_Reference->AvatarData.DefaultImage;

			Column++;
			if (Column >= 4) {
				Column = 0;
				Row++;
			}

			AvatarWidgetComponent_Reference->UpdateWidgetMaterials();
			AvatarWidgetComponent_Reference->AvatarName->SetText(FText::FromString(AvatarWidgetComponent_Reference->AvatarData.AvatarName));
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