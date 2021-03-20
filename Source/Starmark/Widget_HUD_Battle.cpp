#include "Widget_HUD_Battle.h"


#include "Kismet/GameplayStatics.h"
#include "PlayerController_Base.h"
#include "WidgetComponent_AvatarAttack.h"
#include "Starmark_GameState.h"


// ------------------------- Widget
void UWidget_HUD_Battle::UpdateAvatarAttacksComponents()
{
	if (AvatarAttacksBox) {
		for (int i = 0; i < AvatarAttacksBox->GetChildrenCount(); i++) {
			if (PlayerControllerReference->CurrentSelectedAvatar->AllKnownAttacks.IsValidIndex(i)) {
				Cast<UWidgetComponent_AvatarAttack>(AvatarAttacksBox->GetChildAt(i))->AttackNameText->SetText(FText::FromString(PlayerControllerReference->CurrentSelectedAvatar->AllKnownAttacks[i].Name));

				Cast<UWidgetComponent_AvatarAttack>(AvatarAttacksBox->GetChildAt(i))->PlayerControllerReference = PlayerControllerReference;
				Cast<UWidgetComponent_AvatarAttack>(AvatarAttacksBox->GetChildAt(i))->AvatarAttackIndex = i;

				AvatarAttacksBox->GetChildAt(i)->SetVisibility(ESlateVisibility::Visible);
			}
			else {
				AvatarAttacksBox->GetChildAt(i)->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Final Damage")));
	}
}


// ------------------------- Commands
void UWidget_HUD_Battle::MoveCommand()
{
	PlayerControllerReference->PlayerClickMode = E_PlayerCharacter_ClickModes::E_MoveCharacter;

	// Hide Avatar Attacks Box
	if (AvatarAttacksBox)
		AvatarAttacksBox->SetVisibility(ESlateVisibility::Hidden);
}


void UWidget_HUD_Battle::AttackCommand()
{
	// Show Avatar Attacks Box
	if (AvatarAttacksBox) {
		UpdateAvatarAttacksComponents();

		AvatarAttacksBox->SetVisibility(ESlateVisibility::Visible);
	}
}


void UWidget_HUD_Battle::SwitchCommand()
{
	if (!PlayerControllerReference)
		PlayerControllerReference = Cast<APlayerController_Base>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	PlayerControllerReference->PlayerClickMode = E_PlayerCharacter_ClickModes::E_SelectCharacterToControl;

	if (AvatarAttacksBox)
		AvatarAttacksBox->SetVisibility(ESlateVisibility::Hidden);
}


void UWidget_HUD_Battle::EndCommand()
{
	if (AvatarAttacksBox)
		AvatarAttacksBox->SetVisibility(ESlateVisibility::Hidden);

	PlayerControllerReference->SendEndOfTurnCommandToServer_Implementation();
}