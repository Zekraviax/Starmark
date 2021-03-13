#include "Widget_HUD_Battle.h"


#include "Kismet/GameplayStatics.h"
#include "PlayerController_Base.h"
#include "WidgetComponent_AvatarAttack.h"
#include "Starmark_GameState.h"


// ------------------------- Widget
void UWidget_HUD_Battle::UpdateAvatarAttacksComponents()
{
	//if (!PlayerControllerReference)
	//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("PlayerController Not Valid")));
	//else
	//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("PlayerController Valid")));

	//if (!PlayerControllerReference->CurrentSelectedAvatar)
	//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("CurrentSelectedAvatar Not Valid")));
	//else
	//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("CurrentSelectedAvatar Valid")));

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


// ------------------------- Commands
void UWidget_HUD_Battle::MoveCommand()
{
	if (!PlayerControllerReference)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("PlayerController Not Valid")));

	PlayerControllerReference->PlayerClickMode = E_PlayerCharacter_ClickModes::E_MoveCharacter;

	// Hide Avatar Attacks Box
	if (AvatarAttacksBox)
		AvatarAttacksBox->SetVisibility(ESlateVisibility::Hidden);
}


void UWidget_HUD_Battle::AttackCommand()
{
	//if (!PlayerControllerReference)
	//	PlayerControllerReference = NewPlayerControllerReference;

	// Show Avatar Attacks Box
	if (AvatarAttacksBox) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Avatar Attacks Box Valid")));
		UpdateAvatarAttacksComponents();
		AvatarAttacksBox->SetVisibility(ESlateVisibility::Visible);
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Avatar Attacks Box Not Valid")));
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
	//GetWorld()->GetGameState<AStarmark_GameState>()->AvatarEndTurn_Implementation();

	if (AvatarAttacksBox)
		AvatarAttacksBox->SetVisibility(ESlateVisibility::Hidden);
}