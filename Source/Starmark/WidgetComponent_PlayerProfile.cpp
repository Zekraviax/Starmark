#include "WidgetComponent_PlayerProfile.h"

#include "Player_SaveData.h"
#include "Kismet/GameplayStatics.h"


// ------------------------- Components
UPlayer_SaveData* UWidgetComponent_PlayerProfile::OnSelectProfileButtonPressed()
{
	UPlayer_SaveData* ReturnProfile = Cast<UPlayer_SaveData>(UGameplayStatics::LoadGameFromSlot(ProfileNameText->GetText().ToString(), 0));

	//if (!ReturnProfile) {

	//}

	return ReturnProfile;
}