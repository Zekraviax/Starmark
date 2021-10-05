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
	// Avatar Name
	LinkedAvatar = NewLinkedAvatar;

	if (LinkedAvatar.Nickname != "" && LinkedAvatar.Nickname != LinkedAvatar.AvatarName)
		AvatarText->SetText(FText::FromString(LinkedAvatar.Nickname));
	else
		AvatarText->SetText(FText::FromString(LinkedAvatar.AvatarName));

	// Avatar Type(s)
	TypesText->SetText(FText::FromString(UEnum::GetDisplayValueAsText<EAvatar_Types>(LinkedAvatar.PrimaryType).ToString()));

	// Progress Bar variables
	float HealthDivision = float(LinkedAvatar.CurrentHealthPoints) / float(LinkedAvatar.BaseStats.HealthPoints);
	HealthBar->SetPercent(HealthDivision);
	float ManaDivision = float(LinkedAvatar.CurrentManaPoints) / float(LinkedAvatar.BaseStats.ManaPoints);
	ManaBar->SetPercent(ManaDivision);
	float TileMovesDivision = float(LinkedAvatar.CurrentTileMoves) / float(LinkedAvatar.MaximumTileMoves);
	TileMovesBar->SetPercent(TileMovesDivision);

	// Text
	if (HealthDisplayText->IsValidLowLevel())
		HealthDisplayText->SetText(FText::FromString(FString::FromInt(LinkedAvatar.CurrentHealthPoints) + " / " + FString::FromInt(LinkedAvatar.BaseStats.HealthPoints)));

	if (ManaDisplayText->IsValidLowLevel())
		ManaDisplayText->SetText(FText::FromString(FString::FromInt(LinkedAvatar.CurrentManaPoints) + " / " + FString::FromInt(LinkedAvatar.BaseStats.ManaPoints)));

	if (TileMovesDisplayText->IsValidLowLevel())
		TileMovesDisplayText->SetText(FText::FromString(FString::FromInt(LinkedAvatar.CurrentTileMoves) + " / " + FString::FromInt(LinkedAvatar.MaximumTileMoves)));
}