#include "WidgetComponent_AvatarBattleData.h"


#include "Character_Pathfinder.h"


void UWidgetComponent_AvatarBattleData::NativeConstruct()
{
	Super::NativeConstruct();

}


void UWidgetComponent_AvatarBattleData::NativeTick(const FGeometry & MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	if (LinkedAvatar) {
		// Interpolate Health and Mana bar values
		HealthBar->SetPercent(FMath::FInterpTo(HealthBar->Percent, (float(LinkedAvatar->CurrentHealthPoints) / float(LinkedAvatar->AvatarData.BaseStats.HealthPoints)), DeltaTime, 4.f));
		ManaBar->SetPercent(FMath::FInterpTo(ManaBar->Percent, (float(LinkedAvatar->CurrentManaPoints) / float(LinkedAvatar->AvatarData.BaseStats.ManaPoints)), DeltaTime, 4.f));

		// Interpolate Health and Mana text values
		HealthDisplayText->SetText(FText::FromString(FString::FromInt(FMath::TruncToInt(FMath::FInterpTo(FCString::Atof(*HealthDisplayText->GetText().ToString()), float(LinkedAvatar->CurrentHealthPoints), DeltaTime, 5.f))) + 
			" / " + FString::FromInt(LinkedAvatar->AvatarData.BaseStats.HealthPoints)));
		ManaDisplayText->SetText(FText::FromString(FString::FromInt(FMath::TruncToInt(FMath::FInterpTo(FCString::Atof(*ManaDisplayText->GetText().ToString()), float(LinkedAvatar->CurrentManaPoints), DeltaTime, 5.f))) +
			" / " + FString::FromInt(LinkedAvatar->AvatarData.BaseStats.ManaPoints)));
	}
}


// ------------------------- Avatar
void UWidgetComponent_AvatarBattleData::SetAvatarData()
{
	if (LinkedAvatar) {
		if (NicknameText->GetText().ToString() != LinkedAvatar->AvatarData.Nickname) {
			if (LinkedAvatar->AvatarData.Nickname != "" && LinkedAvatar->AvatarData.Nickname != LinkedAvatar->AvatarData.AvatarName) {
				NicknameText->SetText(FText::FromString(LinkedAvatar->AvatarData.Nickname));
			} else {
				NicknameText->SetText(FText::FromString(""));
			}
		}

		if (LevelText->GetText().ToString() != FString::FromInt(LinkedAvatar->CurrentLevel)) {
			LevelText->SetText(FText::FromString(FString::FromInt(LinkedAvatar->CurrentLevel)));
		}

		if (AvatarText->GetText().ToString() != LinkedAvatar->AvatarData.AvatarName) {
			AvatarText->SetText(FText::FromString(LinkedAvatar->AvatarData.AvatarName));
		}

		if (TypesText->GetText().ToString() != UEnum::GetDisplayValueAsText<EAvatar_Types>(LinkedAvatar->AvatarData.PrimaryType).ToString()) {
			TypesText->SetText(FText::FromString(UEnum::GetDisplayValueAsText<EAvatar_Types>(LinkedAvatar->AvatarData.PrimaryType).ToString()));
		}

		float Division = float(LinkedAvatar->CurrentHealthPoints) / float(LinkedAvatar->AvatarData.BaseStats.HealthPoints);
		HealthBar->SetPercent(Division);

		if (HealthDisplayText->IsValidLowLevel())
			HealthDisplayText->SetText(FText::FromString(FString::FromInt(LinkedAvatar->CurrentHealthPoints) + " / " + FString::FromInt(LinkedAvatar->AvatarData.BaseStats.HealthPoints)));

		if (ManaDisplayText->IsValidLowLevel())
			ManaDisplayText->SetText(FText::FromString(FString::FromInt(LinkedAvatar->CurrentManaPoints) + " / " + FString::FromInt(LinkedAvatar->AvatarData.BaseStats.ManaPoints)));
	}
}