#include "Widget_HUD_Battle.h"


#include "Blueprint/WidgetTree.h"
#include "Character_Pathfinder.h"
#include "Containers/UnrealString.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController_Battle.h"
#include "Starmark_GameState.h"
#include "WidgetComponent_AvatarAttack.h"


// ------------------------- Widget
void UWidget_HUD_Battle::UpdateAvatarAttacksComponents(TArray<FAvatar_AttackStruct> Attacks)
{
	UE_LOG(LogTemp, Warning, TEXT("UWidget_HUD_Battle / UpdateAvatarAttacksComponents / AvatarAttacksBox is valid? %s"), IsValid(AvatarAttacksBox) ? TEXT("true") : TEXT("false"));
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("UWidget_HUD_Battle / UpdateAvatarAttacksComponents / AvatarAttacksBox is valid? %s"), IsValid(AvatarAttacksBox) ? TEXT("true") : TEXT("false")));

	if (AvatarAttacksBox) {
		int ValidButtonsCount = 0;
		UE_LOG(LogTemp, Warning, TEXT("UWidget_HUD_Battle / UpdateAvatarAttacksComponents / AvatarAttacksBox child count: %d"), AvatarAttacksBox->GetChildrenCount());
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("UWidget_HUD_Battle / UpdateAvatarAttacksComponents / AvatarAttacksBox child count: %d"), AvatarAttacksBox->GetChildrenCount()));

		for (int i = 0; i < AvatarAttacksBox->GetChildrenCount(); i++) {
			UWidgetComponent_AvatarAttack* Button = Cast<UWidgetComponent_AvatarAttack>(AvatarAttacksBox->GetChildAt(i));

			if (IsValid(Button)) {
				Button->PlayerControllerReference = PlayerControllerReference;
				Button->SetIsEnabled(true);
				Button->SetVisibility(ESlateVisibility::Visible);

				if (Attacks.IsValidIndex(ValidButtonsCount)) {
					if (Attacks[ValidButtonsCount].Name != "Default" && Attacks[ValidButtonsCount].Name != "None") {
						UE_LOG(LogTemp, Warning, TEXT("UWidget_HUD_Battle / UpdateAvatarAttacksComponents / Create UI button for current avatar's attack %s at index: %d"), *PlayerControllerReference->CurrentSelectedAvatar->CurrentKnownAttacks[ValidButtonsCount].Name, ValidButtonsCount);

						Button->AttackNameText->SetText(FText::FromString(Attacks[ValidButtonsCount].Name.ToUpper()));
						Button->AvatarAttackIndex = ValidButtonsCount;
					}
				} else if (ValidButtonsCount == 4) {
					Button->AttackNameText->SetText(FText::FromString("RESERVE"));
					Button->AvatarAttackIndex = -5;
				} else {
					UE_LOG(LogTemp, Warning, TEXT("UWidget_HUD_Battle / UpdateAvatarAttacksComponents / Reset avatar attack button at index: %d"), i);
					//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("UWidget_HUD_Battle / UpdateAvatarAttacksComponents /  Reset avatar attack button at index: %d"), i));

					Button->AttackNameText->SetText(FText::FromString("-"));
					Button->AvatarAttackIndex = -1;
					Button->SetIsEnabled(false);
				}

				ValidButtonsCount++;
			}
		}
	} else {
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("UWidget_HUD_Battle / UpdateAvatarAttacksComponents / AvatarAttacksBox is not valid!?")));
	}
}


void UWidget_HUD_Battle::SetCommandsToListOfReserveAvatars(TArray<FAvatar_Struct> ReserveAvatars) const
{
	int ValidButtonIndex = 0;
	
	for (int i = 0; i < AvatarAttacksBox->GetChildrenCount(); i++) {
		if (Cast<UWidgetComponent_AvatarAttack>(AvatarAttacksBox->GetChildAt(i))) {
			UWidgetComponent_AvatarAttack* Button = Cast<UWidgetComponent_AvatarAttack>(AvatarAttacksBox->GetChildAt(i));
			
			if (ReserveAvatars.IsValidIndex(ValidButtonIndex)) {
				Button->AttackNameText->SetText(FText::FromString(ReserveAvatars[ValidButtonIndex].Nickname));
				Button->AvatarAttackIndex = ReserveAvatars[ValidButtonIndex].IndexInPlayerLibrary;
				Button->SetVisibility(ESlateVisibility::Visible);
				Button->SetIsEnabled(true);
			} else {
				UE_LOG(LogTemp, Warning, TEXT("UWidget_HUD_Battle / SetCommandsToListOfReserveAvatars / Reset button at index: %d"), i);

				Button->AttackNameText->SetText(FText::FromString("-"));
				//Button->PlayerControllerReference = nullptr;
				Button->AvatarAttackIndex = -1;
				Button->SetVisibility(ESlateVisibility::Collapsed);
			}

			ValidButtonIndex++;
		}
	}
}


void UWidget_HUD_Battle::SetUiIconsInTurnOrder(TArray<UTexture2D*> InDynamicAvatarTurnOrderImages) const
{
	if (InDynamicAvatarTurnOrderImages.Num() > 0) {
		if (EntityIconsInTurnOrder) {
			UE_LOG(LogTemp, Warning, TEXT("UWidget_HUD_Battle / SetUiIconsInTurnOrder / Cleared EntityIconsInTurnOrder children"))
			UE_LOG(LogTemp, Warning, TEXT("UWidget_HUD_Battle / SetUiIconsInTurnOrder / InDynamicAvatarTurnOrderImages child count: %d"), InDynamicAvatarTurnOrderImages.Num());

			for (int i = 1; i < 6; i++) {
				UE_LOG(LogTemp, Warning, TEXT("UWidget_HUD_Battle / SetUiIconsInTurnOrder / Validating InDynamicAvatarTurnOrderImages child at index: %d"), i);
				UE_LOG(LogTemp, Warning, TEXT("UWidget_HUD_Battle / SetUiIconsInTurnOrder / EntityIconsInTurnOrder child count: %d"), EntityIconsInTurnOrder->GetChildrenCount());
				int EntityIconIndex = i - 1;

				if (InDynamicAvatarTurnOrderImages.IsValidIndex(i)) {
					if (EntityIconsInTurnOrder->GetChildrenCount() <= EntityIconIndex) {
						if (Cast<UImage>(EntityIconsInTurnOrder->GetChildAt(EntityIconIndex))) {
							Cast<UImage>(EntityIconsInTurnOrder->GetChildAt(EntityIconIndex))->SetBrushFromTexture(InDynamicAvatarTurnOrderImages[i]);
							Cast<UImage>(EntityIconsInTurnOrder->GetChildAt(EntityIconIndex))->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

							UE_LOG(LogTemp, Warning, TEXT("UWidget_HUD_Battle / SetUiIconsInTurnOrder / Apply avatar image to EntityIconsInTurnOrder child at index %d"), EntityIconIndex)
						} else {
							//Cast<UImage>(EntityIconsInTurnOrder->GetChildAt(EntityIconIndex))->SetVisibility(ESlateVisibility::Collapsed);
							UE_LOG(LogTemp, Warning, TEXT("UWidget_HUD_Battle / SetUiIconsInTurnOrder / EntityIconsInTurnOrder child at index %d is not a UImage"), EntityIconIndex)
						}
					} else {
						Cast<UImage>(EntityIconsInTurnOrder->GetChildAt(EntityIconIndex))->SetVisibility(ESlateVisibility::Collapsed);
						UE_LOG(LogTemp, Warning, TEXT("UWidget_HUD_Battle / SetUiIconsInTurnOrder / EntityIconsInTurnOrder child at index %d is not valid"), EntityIconIndex)
					}
					
					// To-Do: If no UI images are found, get the default image
				}
				else {
					UE_LOG(LogTemp, Warning, TEXT("UWidget_HUD_Battle / SetUiIconsInTurnOrder / InDynamicAvatarTurnOrderImages index %d is not valid."), i);

					//To-Do: Set images to be blank here
					if (Cast<UImage>(EntityIconsInTurnOrder->GetChildAt(EntityIconIndex))) {
						Cast<UImage>(EntityIconsInTurnOrder->GetChildAt(EntityIconIndex))->SetVisibility(ESlateVisibility::Collapsed);
						UE_LOG(LogTemp, Warning, TEXT("UWidget_HUD_Battle / SetUiIconsInTurnOrder / Apply blank image to EntityIconsInTurnOrder child at index %d"), EntityIconIndex)
					} else {
						UE_LOG(LogTemp, Warning, TEXT("UWidget_HUD_Battle / SetUiIconsInTurnOrder / EntityIconsInTurnOrder child at index %d is not a UImage"), EntityIconIndex)
					}
				}
			}
		} else {
			UE_LOG(LogTemp, Warning, TEXT("UWidget_HUD_Battle / SetUiIconsInTurnOrder / EntityIconsInTurnOrder is not valid"))
		}
	} else {
		UE_LOG(LogTemp, Warning, TEXT("UWidget_HUD_Battle / SetUiIconsInTurnOrder / No images in the InDynamicAvatarTurnOrderImages array"))
	}
}


void UWidget_HUD_Battle::SetCurrentActingEntityInfo(ACharacter_Pathfinder* CurrentActingEntity)
{
	// To-Do: If no UI images are found, get the default image
	// To-Do: Don't let the player interact with the HUD when an enemy is acting.

	UE_LOG(LogTemp, Warning, TEXT("UWidget_HUD_Battle / SetCurrentActingEntityInfo / IsValid(CurrentActingEntity) returns: %s"), IsValid(CurrentActingEntity) ? TEXT("true") : TEXT("false"));

	if (CurrentActingEntity) {
		if (CurrentActingEntity->AvatarData.UiImages.Num() > 0) {
			CurrentEntityIcon->SetBrushFromTexture(CurrentActingEntity->AvatarData.UiImages[0]);
			UE_LOG(LogTemp, Warning, TEXT("UWidget_HUD_Battle / SetCurrentActingEntityInfo / Grabbing the avatar's first UiImage"));
		} else {
			UE_LOG(LogTemp, Warning, TEXT("UWidget_HUD_Battle / SetCurrentActingEntityInfo / This avatar doesn't have any UiImages!"));
		}

		if (CurrentActingEntity->AvatarData.Factions.Contains(EEntity_Factions::Player1)) {
			UE_LOG(LogTemp, Warning, TEXT("UWidget_HUD_Battle / SetCurrentActingEntityInfo / This avatar has the faction Player1"));
			HealthText->SetText(FText::FromString(FString::FromInt(CurrentActingEntity->AvatarData.CurrentHealthPoints) + " / " + FString::FromInt(CurrentActingEntity->AvatarData.BattleStats.MaximumHealthPoints)));
			ManaText->SetText(FText::FromString(FString::FromInt(CurrentActingEntity->AvatarData.CurrentManaPoints) + " / " + FString::FromInt(CurrentActingEntity->AvatarData.BattleStats.MaximumManaPoints)));

			HealthBar->SetPercent(float(CurrentActingEntity->AvatarData.CurrentHealthPoints) / float(CurrentActingEntity->AvatarData.BattleStats.MaximumHealthPoints));
			ManaBar->SetPercent(float(CurrentActingEntity->AvatarData.CurrentManaPoints) / float(CurrentActingEntity->AvatarData.BattleStats.MaximumManaPoints));

			CurrentEntityNameText->SetText(FText::FromString(CurrentActingEntity->AvatarData.Nickname.ToUpper() + "'S TURN"));
		} else {
			UE_LOG(LogTemp, Warning, TEXT("UWidget_HUD_Battle / SetCurrentActingEntityInfo / This avatar doesn't have the faction Player1. Limited UI elements should be shown"));
			// Show limited UI elements:
			// Name
			// Portrait

			ShowHideActingPlayerHudElements(false);
		}
	}
}


void UWidget_HUD_Battle::ResetBattleHud()
{
	UE_LOG(LogTemp, Warning, TEXT("UWidget_HUD_Battle / ResetBattleHud / Player's HUD is being reset."));
	UE_LOG(LogTemp, Warning, TEXT("UWidget_HUD_Battle / ResetBattleHud / Currently acting player's Multiplayer ID: %d"), Cast<AStarmark_GameState>(GetWorld()->GetGameState())->CurrentlyActingPlayer->MultiplayerUniqueID);
	UE_LOG(LogTemp, Warning, TEXT("UWidget_HUD_Battle / ResetBattleHud / This player's Multiplayer ID: %d"), PlayerControllerReference->MultiplayerUniqueID);
	UE_LOG(LogTemp, Warning, TEXT("UWidget_HUD_Battle / ResetBattleHud / This player's Name: %s"), *PlayerControllerReference->PlayerDataStruct.PlayerName);

	if (Cast<AStarmark_GameState>(GetWorld()->GetGameState())->CurrentlyActingPlayer->MultiplayerUniqueID == PlayerControllerReference->MultiplayerUniqueID) {
		UpdateAvatarAttacksComponents(Cast<AStarmark_GameState>(GetWorld()->GetGameState())->CurrentlyActingAvatar->CurrentKnownAttacks);
		ShowHideActingPlayerHudElements(true);
	} else {
		ShowHideActingPlayerHudElements(false);
	}
}


// Show or hide all the HUD elements that are only relevant to the player whose turn it is
void UWidget_HUD_Battle::ShowHideActingPlayerHudElements(bool ShowElements) const
{
	if (ShowElements) {
		AvatarAttacksBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		EndTurnCommand->SetVisibility(ESlateVisibility::Visible);
		CommandsBackgroundImage->SetVisibility(ESlateVisibility::Visible);

		HealthBar->SetVisibility(ESlateVisibility::Visible);
		HealthText->SetVisibility(ESlateVisibility::Visible);

		ManaBar->SetVisibility(ESlateVisibility::Visible);
		ManaText->SetVisibility(ESlateVisibility::Visible);
	} else {
		AvatarAttacksBox->SetVisibility(ESlateVisibility::Collapsed);
		EndTurnCommand->SetVisibility(ESlateVisibility::Collapsed);
		CommandsBackgroundImage->SetVisibility(ESlateVisibility::Collapsed);

		HealthBar->SetVisibility(ESlateVisibility::Collapsed);
		HealthText->SetVisibility(ESlateVisibility::Collapsed);

		ManaBar->SetVisibility(ESlateVisibility::Collapsed);
		ManaText->SetVisibility(ESlateVisibility::Collapsed);
	}
}


// ------------------------- Commands
void UWidget_HUD_Battle::MoveCommand()
{
	PlayerControllerReference->SetPlayerClickMode(E_PlayerCharacter_ClickModes::E_MoveCharacter);
}


void UWidget_HUD_Battle::EndCommand()
{
	PlayerControllerReference->SendEndOfTurnCommandToServer_Implementation();
}


void UWidget_HUD_Battle::AttackCommand()
{
	CommandsBox->SetVisibility(ESlateVisibility::Collapsed);
	AvatarAttacksBox->SetVisibility(ESlateVisibility::Visible);
}