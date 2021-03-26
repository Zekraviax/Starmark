#include "Widget_MainMenu.h"

#include "Starmark_GameInstance.h"
#include "Widget_ServerBrowser.h"
#include "Widget_PlayerProfileCreator.h"


// ------------------------- Widget
void UWidget_MainMenu::OnHostGameButtonPressed()
{
	RemoveFromParent();
	//Cast<AStarmark_GameInstance>(GetWorld()->GetGameInstance())->BeginHostingGame();
}


void UWidget_MainMenu::OnSearchGameButtonPressed()
{
	RemoveFromParent();
}