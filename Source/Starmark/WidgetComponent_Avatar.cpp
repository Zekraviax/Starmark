#include "WidgetComponent_Avatar.h"

#include "Components/CanvasPanelSlot.h"
#include "PlayerController_Lobby.h"
#include "Widget_AvatarCreation.h"
#include "Widget_AvatarLibrary.h"
#include "WidgetComponent_RightClickMenu.h"


// ------------------------- Widget
void UWidgetComponent_Avatar::OnInteractButtonPressed()
{
	//switch (CurrentFunction)
	//{
	//	// Open AvatarCreator
	//	case (E_AvatarWidgetComponent_Function::E_CreateNewAvatarInLibrary):
	//		if (AvatarCreationWidget_Class) {
	//			AvatarCreationWidget_Reference = CreateWidget<UWidget_AvatarCreation>(this, AvatarCreationWidget_Class);
	//			AvatarCreationWidget_Reference->PopulateDropDowns();
	//			AvatarCreationWidget_Reference->AddToViewport();

	//			// Bind delegate in the AvatarLibrary
	//			if (Cast<UWidget_AvatarLibrary>(PairedWidget))
	//				Cast<UWidget_AvatarLibrary>(PairedWidget)->BindAvatarCreatedDelegate(this);
	//		}
	//		break;
	//	case (E_AvatarWidgetComponent_Function::E_AddAvatarToChosenSlot):
	//		Cast<APlayerController_Lobby>(GetWorld()->GetFirstPlayerController())->OnAvatarWidgetComponentClicked(this);
	//		break;
	//	case (E_AvatarWidgetComponent_Function::E_Nothing):
	//		break;
	//	default:
	//		break;
	//}

	//if (OnAvatarSelectedDelegate.IsBound())
	//	OnAvatarSelectedDelegate.Broadcast();
}


void UWidgetComponent_Avatar::OnRightMouseButtonDown()
{
	// Open the right click menu
	if (IsValid(RightClickMenu_Class) &&
		!IsValid(RightClickMenu_Reference)) {
		RightClickMenu_Reference = CreateWidget<UWidgetComponent_RightClickMenu>(this, RightClickMenu_Class);
	}

	if (IsValid(RightClickMenu_Reference)) {
		RightClickMenu_Reference->Commands.Append(RightClickMenuCommands);
		RightClickMenu_Reference->OwnerWidget = this;
		RightClickMenu_Reference->OnWidgetCreated();

		if (!RightClickMenu_Reference->IsInViewport())
			RightClickMenu_Reference->AddToViewport();
	}

	//if (AvatarCreationWidget_Class) {
	//	AvatarCreationWidget_Reference = CreateWidget<UWidget_AvatarCreation>(this, AvatarCreationWidget_Class);
	//	AvatarCreationWidget_Reference->IsEditingExistingAvatar = true;
	//	AvatarCreationWidget_Reference->PopulateDropDownsWithAvatarData(AvatarData);
	//	AvatarCreationWidget_Reference->AddToViewport();

	//	// Bind delegate in the AvatarLibrary
	//	if (Cast<UWidget_AvatarLibrary>(PairedWidget))
	//		Cast<UWidget_AvatarLibrary>(PairedWidget)->BindAvatarCreatedDelegate(this);
	//}
}


void UWidgetComponent_Avatar::UpdateWidgetMaterials_Implementation()
{
	// Implemented in blueprints
}


void UWidgetComponent_Avatar::ApplyNewAvatarData(FAvatar_Struct NewAvatarData)
{
	// Empty Slot
	if (NewAvatarData.Nickname.Len() == 0 &&
		NewAvatarData.AvatarName == "Default") {
		AvatarName->SetText(FText::FromString("Empty Slot"));
	} else {
		AvatarData = NewAvatarData;
		AvatarMaterial = AvatarData.DefaultImage;
		AvatarName->SetText(FText::FromString(AvatarData.AvatarName));

		UpdateWidgetMaterials();
	}
}