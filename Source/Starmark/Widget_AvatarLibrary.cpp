#include "Widget_AvatarLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "Starmark_GameInstance.h"
#include "Starmark_PlayerState.h"
#include "WidgetComponent_Avatar.h"


// ------------------------- Widget
void UWidget_AvatarLibrary::OnWidgetOpened()
{
	UStarmark_GameInstance* GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	AStarmark_PlayerState* PlayerStateReference = Cast<AStarmark_PlayerState>(GetOwningPlayerState());

	if (PlayerStateReference->PlayerProfileReference && AvatarWidgetComponent_Class) {
		int Column = -1;
		int Row = 0;

		for (int i = 0; i < PlayerStateReference->PlayerProfileReference->AvatarLibrary.Num(); i++) {
			AvatarWidgetComponent_Reference = CreateWidget<UWidgetComponent_Avatar>(this, AvatarWidgetComponent_Class);
			AvatarWidgetComponent_Reference->AvatarData = PlayerStateReference->PlayerProfileReference->AvatarLibrary[i];
			AvatarWidgetComponent_Reference->PairedWidget = this;

			Column++;
			if (Column > 4) {
				Column = 0;
				Row++;
			}

			AvatarWidgetComponent_Reference->UpdateWidgetMaterials();
			AvatarLibraryUniformGridPanel->AddChildToUniformGrid(AvatarWidgetComponent_Reference, Column, Row);
		}

		// Create one move Avatar WidgetComponent that's used to add new Avatars to the Library
		AvatarWidgetComponent_Reference = CreateWidget<UWidgetComponent_Avatar>(this, AvatarWidgetComponent_Class);
		AvatarWidgetComponent_Reference->PairedWidget = this;
		AvatarWidgetComponent_Reference->CurrentFunction = E_AvatarWidgetComponent_Function::E_CreateNewAvatarInLibrary;

		Column++;
		if (Column > 4) {
			Column = 0;
			Row++;
		}

		AvatarWidgetComponent_Reference->UpdateWidgetMaterials();
		AvatarLibraryUniformGridPanel->AddChildToUniformGrid(AvatarWidgetComponent_Reference, Column, Row);
	}
}