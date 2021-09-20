#include "WidgetComponent_AvatarBattleData.h"


#include "Character_Pathfinder.h"


void UWidgetComponent_AvatarBattleData::NativeConstruct()
{
	Super::NativeConstruct();
}


void UWidgetComponent_AvatarBattleData::NativeTick(const FGeometry & MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	HealthBar->SetPercent(float(LinkedAvatar.CurrentHealthPoints) / float(LinkedAvatar.BaseStats.HealthPoints));
	ManaBar->SetPercent(float(LinkedAvatar.CurrentManaPoints) / float(LinkedAvatar.BaseStats.ManaPoints));	

	HealthDisplayText->SetText(FText::FromString(FString::FromInt(FMath::TruncToInt(float(LinkedAvatar.CurrentHealthPoints))) + " / " + FString::FromInt(LinkedAvatar.BaseStats.HealthPoints)));
	ManaDisplayText->SetText(FText::FromString(FString::FromInt(FMath::TruncToInt(float(LinkedAvatar.CurrentManaPoints))) + " / " + FString::FromInt(LinkedAvatar.BaseStats.ManaPoints)));
}


// ------------------------- Avatar
void UWidgetComponent_AvatarBattleData::UpdateAvatarData(FAvatar_Struct NewLinkedAvatar)
{
	if (LinkedAvatar.Nickname != "" && LinkedAvatar.Nickname != LinkedAvatar.AvatarName)
		AvatarText->SetText(FText::FromString(LinkedAvatar.Nickname));
	else
		AvatarText->SetText(FText::FromString(LinkedAvatar.AvatarName));

	//TierText->SetText(FText::FromString(FString::FromInt(1)));
	TypesText->SetText(FText::FromString(UEnum::GetDisplayValueAsText<EAvatar_Types>(LinkedAvatar.PrimaryType).ToString()));

	float Division = float(LinkedAvatar.CurrentHealthPoints) / float(LinkedAvatar.BaseStats.HealthPoints);
	HealthBar->SetPercent(Division);

	if (HealthDisplayText->IsValidLowLevel())
		HealthDisplayText->SetText(FText::FromString(FString::FromInt(LinkedAvatar.CurrentHealthPoints) + " / " + FString::FromInt(LinkedAvatar.BaseStats.HealthPoints)));

	if (ManaDisplayText->IsValidLowLevel())
		ManaDisplayText->SetText(FText::FromString(FString::FromInt(LinkedAvatar.CurrentManaPoints) + " / " + FString::FromInt(LinkedAvatar.BaseStats.ManaPoints)));
}