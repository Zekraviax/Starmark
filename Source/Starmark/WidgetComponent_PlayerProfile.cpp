#include "WidgetComponent_PlayerProfile.h"

#include "Kismet/GameplayStatics.h"
#include "Player_SaveData.h"
#include "SaveData_PlayerProfilesList.h"
#include "Starmark_PlayerState.h"
#include "Widget_PlayerProfileCreator.h"


// ------------------------- Components
void UWidgetComponent_PlayerProfile::OnSelectProfileButtonPressed()
{
	USaveData_PlayerProfilesList* PlayerProfilesList = Cast<USaveData_PlayerProfilesList>(UGameplayStatics::LoadGameFromSlot("PlayerProfilesList", 0));
	UPlayer_SaveData* FoundProfile;

	for (int i = 0; i < PlayerProfilesList->PlayerProfileNames.Num(); i++) {
		UE_LOG(LogTemp, Warning, TEXT("UWidgetComponent_PlayerProfile / OnSelectProfileButtonPressed / Profile found: %s"), *PlayerProfilesList->PlayerProfileNames[i]);

		if (PlayerProfilesList->PlayerProfileNames[i] == ProfileNameText->GetText().ToString()) {
			FoundProfile = Cast<UPlayer_SaveData>(UGameplayStatics::LoadGameFromSlot(PlayerProfilesList->PlayerProfileNames[i], 0));

			AStarmark_PlayerState* PlayerStateReference = Cast<AStarmark_PlayerState>(GetOwningPlayerState());
			PlayerStateReference->PlayerProfileReference = FoundProfile;

			break;
		}
	}
	
	//return ReturnProfile;
	if (OnPlayerProfileLoadedDelegate.IsBound()) {
		OnPlayerProfileLoadedDelegate.Broadcast();
	}
}