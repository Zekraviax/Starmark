#include "Widget_AttackSelectMenu.h"

#include "Character_Pathfinder.h"
#include "Blueprint/WidgetTree.h"


// ------------------------- Widget
void UWidget_AttackSelectMenu::OnWidgetOpened()
{
	if (AvatarReference) {
		for (int i = 0; i < AvatarReference->AttacksArray.Num(); i++) {
			//UButton* AttackButton = CreateWidget<UButton>(GetWorld(), ButtonClass);
			WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
		}
	}
}