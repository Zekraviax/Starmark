#include "Widget_PartyScreen.h"


// ------------------------- Widget
void UWidget_PartyScreen::OnWidgetOpened()
{
	//this->SetInputActionPriority(1);
}


void UWidget_PartyScreen::OnPauseButtonPressed()
{
	this->RemoveFromParent();
}