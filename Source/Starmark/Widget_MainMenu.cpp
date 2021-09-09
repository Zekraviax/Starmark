#include "Widget_MainMenu.h"

#include "Starmark_GameInstance.h"
#include "Widget_ServerBrowser.h"
#include "Widget_ServerCreator.h"
#include "Widget_PlayerProfileCreator.h"


// ------------------------- Widget
void UWidget_MainMenu::OnHostGameButtonPressed()
{
	if (!ServerCreatorWidget_Reference && ServerCreatorWidget_Class)
		ServerCreatorWidget_Reference = CreateWidget<UWidget_ServerCreator>(this, ServerCreatorWidget_Class);

	if (ServerCreatorWidget_Reference) {
		ServerCreatorWidget_Reference->AddToViewport();
		this->RemoveFromParent();
	}
}


void UWidget_MainMenu::OnSearchGameButtonPressed()
{
	RemoveFromParent();
}