#include "WidgetComponent_RightClickMenu.h"

#include "WidgetComponent_Avatar.h"
#include "WidgetComponent_RightClickMenuButton.h"


void UWidgetComponent_RightClickMenu::OnWidgetCreated()
{
	if (!Commands.Contains(E_RightClickMenu_Commands::Cancel))
		Commands.Add(E_RightClickMenu_Commands::Cancel);

	if (IsValid(RightClickMenuButton_Class)) {
		for (int i = 0; i < Commands.Num(); i++) {
			RightClickMenuButton_Reference = CreateWidget<UWidgetComponent_RightClickMenuButton>(this, RightClickMenuButton_Class);
			RightClickMenuButton_Reference->ButtonLabel->SetText(UEnum::GetDisplayValueAsText(Commands[i]));
			ButtonsVerticalBox->AddChildToVerticalBox(RightClickMenuButton_Reference);
		}
	}

	//this->AddToViewport();

	// Set position in Viewport
	FGeometry OwnerGeometry = this->GetCachedGeometry();
	//FVector2D Position = OwnerGeometry.AbsoluteToLocal(this->GetCachedGeometry().GetAbsolutePosition() + this->GetCachedGeometry().GetLocalSize() / 2.f);
	FVector2D Position = GetCachedGeometry().AbsoluteToLocal(FVector2D(OwnerWidget->GetCachedGeometry().GetAbsolutePosition().X - (OwnerWidget->GetCachedGeometry().GetLocalSize().X / 2), (OwnerWidget->GetCachedGeometry().GetAbsolutePosition().Y)));
	this->SetPositionInViewport(Position);
}