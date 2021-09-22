#include "WidgetComponent_Avatar.h"


// ------------------------- Widget
void UWidgetComponent_Avatar::OnInteractButtonPressed()
{
	switch (CurrentFunction)
	{
		case (E_AvatarWidgetComponent_Function::E_CreateNewAvatarInLibrary):
			break;
		case (E_AvatarWidgetComponent_Function::E_Nothing):
			break;
		default:
			break;
	}

	if (OnAvatarSelectedDelegate.IsBound())
		OnAvatarSelectedDelegate.Broadcast();
}


void UWidgetComponent_Avatar::UpdateWidgetMaterials_Implementation()
{
	// Implemented in blueprints
}