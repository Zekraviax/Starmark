#include "WidgetComponent_AvatarBattleData.h"


#include "Character_Pathfinder.h"


void UWidgetComponent_AvatarBattleData::NativeConstruct()
{
	Super::NativeConstruct();

}


void UWidgetComponent_AvatarBattleData::NativeTick(const FGeometry & MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	//if (LinkedAvatar) {
		// Interpolate Health and Mana bar values
		//HealthBar->SetPercent(FMath::FInterpTo(HealthBar->Percent, (float(LinkedAvatar.BaseStats.HealthPoints) / float(LinkedAvatar.BaseStats.HealthPoints)), DeltaTime, 4.f));
		//ManaBar->SetPercent(FMath::FInterpTo(ManaBar->Percent, (float(LinkedAvatar.BaseStats.ManaPoints) / float(LinkedAvatar.BaseStats.ManaPoints)), DeltaTime, 4.f));
		HealthBar->SetPercent(float(LinkedAvatar.CurrentHealthPoints) / float(LinkedAvatar.BaseStats.HealthPoints));
		ManaBar->SetPercent(float(LinkedAvatar.CurrentManaPoints) / float(LinkedAvatar.BaseStats.ManaPoints));	

		// Interpolate Health and Mana text values
		//HealthDisplayText->SetText(FText::FromString(FString::FromInt(FMath::TruncToInt(FMath::FInterpTo(FCString::Atof(*HealthDisplayText->GetText().ToString()), float(LinkedAvatar.BaseStats.HealthPoints), DeltaTime, 5.f))) +
		//	" / " + FString::FromInt(LinkedAvatar.BaseStats.HealthPoints)));
		//ManaDisplayText->SetText(FText::FromString(FString::FromInt(FMath::TruncToInt(FMath::FInterpTo(FCString::Atof(*ManaDisplayText->GetText().ToString()), float(LinkedAvatar.BaseStats.ManaPoints), DeltaTime, 5.f))) +
		//	" / " + FString::FromInt(LinkedAvatar.BaseStats.ManaPoints)));
		HealthDisplayText->SetText(FText::FromString(FString::FromInt(FMath::TruncToInt(float(LinkedAvatar.CurrentHealthPoints))) + " / " + FString::FromInt(LinkedAvatar.BaseStats.HealthPoints)));
		ManaDisplayText->SetText(FText::FromString(FString::FromInt(FMath::TruncToInt(float(LinkedAvatar.CurrentManaPoints))) + " / " + FString::FromInt(LinkedAvatar.BaseStats.ManaPoints)));
	//}
}


// ------------------------- Avatar
void UWidgetComponent_AvatarBattleData::UpdateAvatarData(FAvatar_Struct NewLinkedAvatar)
{
	//if (NewLinkedAvatar) {
	LinkedAvatar = NewLinkedAvatar;
	//}

	//if (LinkedAvatar) {
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("CurrentSelectedAvatar: %s"), *LinkedAvatar.AvatarName));

		//if (NicknameText->GetText().ToString() != LinkedAvatar.Nickname) {
			if (LinkedAvatar.Nickname != "" && LinkedAvatar.Nickname != LinkedAvatar.AvatarName) {
				NicknameText->SetText(FText::FromString(LinkedAvatar.Nickname));
			} else {
				NicknameText->SetText(FText::FromString(""));
			}
		//}

		//if (LevelText->GetText().ToString() != FString::FromInt(1)) {
			LevelText->SetText(FText::FromString(FString::FromInt(1)));
		//}

		//if (AvatarText->GetText().ToString() != LinkedAvatar.AvatarName) {
			AvatarText->SetText(FText::FromString(LinkedAvatar.AvatarName));
		//}

		//if (TypesText->GetText().ToString() != UEnum::GetDisplayValueAsText<EAvatar_Types>(LinkedAvatar.PrimaryType).ToString()) {
			TypesText->SetText(FText::FromString(UEnum::GetDisplayValueAsText<EAvatar_Types>(LinkedAvatar.PrimaryType).ToString()));
		//}

		float Division = float(LinkedAvatar.BaseStats.HealthPoints) / float(LinkedAvatar.BaseStats.HealthPoints);
		HealthBar->SetPercent(Division);

		if (HealthDisplayText->IsValidLowLevel())
			HealthDisplayText->SetText(FText::FromString(FString::FromInt(LinkedAvatar.BaseStats.HealthPoints) + " / " + FString::FromInt(LinkedAvatar.BaseStats.HealthPoints)));

		if (ManaDisplayText->IsValidLowLevel())
			ManaDisplayText->SetText(FText::FromString(FString::FromInt(LinkedAvatar.BaseStats.ManaPoints) + " / " + FString::FromInt(LinkedAvatar.BaseStats.ManaPoints)));
	//}
	//else {
	//	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("LinkedAvatar Not Valid")));
	//}
}