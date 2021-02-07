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
		float HealthPercentValue = FMath::FInterpTo(HealthBar->Percent, (LinkedAvatar->CurrentHealthPoints / LinkedAvatar->AvatarData.BaseStats.HealthPoints), DeltaTime, 5.f);
		HealthBar->SetPercent(HealthPercentValue);
	}
}


// ------------------------- Avatar
void UWidgetComponent_AvatarBattleData::SetAvatarData()
{
	if (LinkedAvatar) {
		if (NicknameText->GetText().ToString() != LinkedAvatar->AvatarData.Nickname) {
			if (LinkedAvatar->AvatarData.Nickname != "" && LinkedAvatar->AvatarData.Nickname != LinkedAvatar->AvatarData.AvatarName) {
				NicknameText->SetText(FText::FromString(LinkedAvatar->AvatarData.Nickname));
			}else {
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
	}
}