#include "Widget_ServerCreator.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Widget_MainMenu.h"


// ------------------------- Widget
void UWidget_ServerCreator::OnExitButtonPressed()
{
	if (!MainMenuWidget_Reference && MainMenuWidget_Class)
		MainMenuWidget_Reference = CreateWidget<UWidget_MainMenu>(this, MainMenuWidget_Class);

	if (MainMenuWidget_Reference) {
		MainMenuWidget_Reference->AddToViewport();
		this->RemoveFromParent();
	}
}