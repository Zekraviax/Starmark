#pragma once

#include "CoreMinimal.h"

#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Starmark_GameInstanceSubsystem.generated.h"


UCLASS()
class STARMARK_API UStarmark_GameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

//public:
//
//	UStarmark_GameInstanceSubsystem();
//
// Variables
// --------------------------------------------------
//
// ------------------------- Session
//	FOnCreateSessionCompleteDelegate CreateSessionComplete;
//	FDelegateHandle CreateSessionComplete;
//	FOnlineSessionSettings SessionSettings;
//
// Functions
// --------------------------------------------------
//
// ------------------------- Session
//	UFUNCTION()
//	virtual void BeginPlay();
//
//	void CreateSessionComplete(FName SessionName, bool bWasSuccessful);
};