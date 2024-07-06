#include "WidgetComponent_RightClickMenuButton.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Player_SaveData.h"
#include "SaveData_PlayerProfile.h"
#include "Starmark_GameInstance.h"
#include "Widget_AvatarLibrary.h"
#include "WidgetComponent_Avatar.h"


void UWidgetComponent_RightClickMenuButton::OnButtonClicked()
{
	RightClickMenuWidget->RemoveFromParent();

	switch (Command)
	{
	case (E_RightClickMenu_Commands::EditAvatar):
		Cast<UWidgetComponent_Avatar>(RightClickMenuWidget->OwnerWidget)->RightClickMenuFunction_EditAvatar();
		break;
	case (E_RightClickMenu_Commands::EquipAvatar):
		EquipAvatar();
		break;
	case (E_RightClickMenu_Commands::UnequipAvatar):
		UnequipAvatar();
		break;
	case (E_RightClickMenu_Commands::DeleteAvatar):
		DeleteAvatar();
		break;
	case (E_RightClickMenu_Commands::Cancel):
		// do nothing except remove this widget.
		break;
	default:
		break;
	}
}


void UWidgetComponent_RightClickMenuButton::EquipAvatar()
{
	int FirstEmptyIndex = 6;
	TArray<int> OccupiedIndices;
	UStarmark_GameInstance* GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	for (int i = 0; i < GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Library.Num(); i++) {
		if (GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Library[i].AvatarName != "None" &&
			GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Library[i].AvatarName != "Default")
			OccupiedIndices.Add(GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Library[i].IndexInPlayerLibrary);
	}

	for (int j = 5; j >= 0; j--) {
		if (!OccupiedIndices.Contains(j))
			FirstEmptyIndex = j;
	}

	if (FirstEmptyIndex < 6) {
		// Get the avatar
		FAvatar_Struct ChosenAvatar = Cast<UWidgetComponent_Avatar>(RightClickMenuWidget->OwnerWidget)->AvatarData;
		ChosenAvatar.IndexInPlayerLibrary = FirstEmptyIndex;

		GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Team.Insert(ChosenAvatar, FirstEmptyIndex);
		GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Library.Remove(ChosenAvatar);

		TArray<UUserWidget*> FoundAvatarLibraryWidgets, FoundAvatarComponents;
		UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, FoundAvatarLibraryWidgets, UWidget_AvatarLibrary::StaticClass(), true);
		UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, FoundAvatarComponents, UWidgetComponent_Avatar::StaticClass(), true);

		// Update all Avatar widgets
		for (int i = 0; i < FoundAvatarLibraryWidgets.Num(); i++) {
			Cast<UWidget_AvatarLibrary>(FoundAvatarLibraryWidgets[i])->OnWidgetOpened();
			Cast<UWidget_AvatarLibrary>(FoundAvatarLibraryWidgets[i])->UpdateAllAvatarsInTeam();
		}

		GameInstanceReference->SaveToCurrentProfile();
	}
}


void UWidgetComponent_RightClickMenuButton::UnequipAvatar()
{
	int FirstEmptyIndex = 0;
	TArray<int> OccupiedIndices;
	UStarmark_GameInstance* GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	for (int i = 0; i < GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Library.Num(); i++) {
		if (GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Library[i].IndexInPlayerLibrary < FirstEmptyIndex) {
			GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Library[i].IndexInPlayerLibrary = FirstEmptyIndex;
		}
	}

	FAvatar_Struct ChosenAvatar = Cast<UWidgetComponent_Avatar>(RightClickMenuWidget->OwnerWidget)->AvatarData;
	ChosenAvatar.IndexInPlayerLibrary = FirstEmptyIndex;

	GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Library.Remove(ChosenAvatar);
	GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Team.Add(ChosenAvatar);

	TArray<UUserWidget*> FoundAvatarLibraryWidgets, FoundAvatarComponents;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, FoundAvatarLibraryWidgets, UWidget_AvatarLibrary::StaticClass(), true);
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, FoundAvatarComponents, UWidgetComponent_Avatar::StaticClass(), true);

	// Update all Avatar widgets
	for (int i = 0; i < FoundAvatarLibraryWidgets.Num(); i++) {
		Cast<UWidget_AvatarLibrary>(FoundAvatarLibraryWidgets[i])->OnWidgetOpened();
		Cast<UWidget_AvatarLibrary>(FoundAvatarLibraryWidgets[i])->UpdateAllAvatarsInTeam();
	}

	GameInstanceReference->SaveToCurrentProfile();
}


void UWidgetComponent_RightClickMenuButton::DeleteAvatar()
{
	UStarmark_GameInstance* GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	TArray<UUserWidget*> FoundAvatarLibraryWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, FoundAvatarLibraryWidgets, UWidget_AvatarLibrary::StaticClass(), true);
	UWidgetComponent_Avatar* AvatarSlot = Cast<UWidgetComponent_Avatar>(RightClickMenuWidget->OwnerWidget);

	if (AvatarSlot->IndexInPlayerTeam >= 0) {
		GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Team.Remove(AvatarSlot->AvatarData);
	} else {
		GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Team.Remove(AvatarSlot->AvatarData);
	}

	AvatarSlot->AvatarData = FAvatar_Struct();

	// Re-assign all indices in the players' library and team
	for (int i = 0; i < GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Team.Num(); i++) {
		GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Team[i].IndexInPlayerLibrary = i;
	}

	for (int i = 0; i < GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Team.Num(); i++) {
		GameInstanceReference->PlayerSaveGameReference->PlayerProfileStruct.Team[i].IndexInPlayerLibrary = (i * -1) - 1;
	}

	// Update all Avatar widgets
	for (int i = 0; i < FoundAvatarLibraryWidgets.Num(); i++) {
		Cast<UWidget_AvatarLibrary>(FoundAvatarLibraryWidgets[i])->OnWidgetOpened();
		Cast<UWidget_AvatarLibrary>(FoundAvatarLibraryWidgets[i])->UpdateAllAvatarsInTeam();
	}

	GameInstanceReference->SaveToCurrentProfile();
}