#include "WidgetComponent_AvatarBattleData.h"


#include "Character_Pathfinder.h"


void UWidgetComponent_AvatarBattleData::NativeConstruct()
{
	Super::NativeConstruct();

	//SetDrawAtDesiredSize(true);
	//SetWidgetSpace(EWidgetSpace::Screen);
	//SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void UWidgetComponent_AvatarBattleData::NativeTick(const FGeometry & MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	if (LinkedAvatar) {
		float HealthPercentValue = FMath::FInterpTo(HealthBar->Percent, (LinkedAvatar->CurrentHealthPoints / LinkedAvatar->AvatarData.BaseStats.HealthPoints), DeltaTime, 5.f);
		HealthBar->SetPercent(HealthPercentValue);
	}
}