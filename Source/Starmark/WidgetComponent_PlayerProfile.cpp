#include "WidgetComponent_PlayerProfile.h"


// ------------------------- Components
void UWidgetComponent_PlayerProfile::OnSelectProfileButtonPressed()
{
	if (OnPlayerProfileLoadedDelegate.IsBound()) {
		OnPlayerProfileLoadedDelegate.Broadcast();
	}
}