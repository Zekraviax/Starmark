#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/TextBlock.h"

#include "WidgetComponent_PlayerProfile.generated.h"

// Forward Declarations
class UPlayer_SaveData;

// Delegates
//DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(UPlayer_SaveData*, );


UCLASS()
class STARMARK_API UWidgetComponent_PlayerProfile : public UUserWidget
{
	GENERATED_BODY()

// ------------------------- Delegate Parameters
	UPROPERTY()
	UPlayer_SaveData* PlayerProfileDelegate;


public:
// Variables
// --------------------------------------------------

// ------------------------- Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ProfileNameText;

// ------------------------- Save Data
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ProfileName;

// Functions
// --------------------------------------------------

// ------------------------- Components
	UFUNCTION(BlueprintCallable)
	UPlayer_SaveData* OnSelectProfileButtonPressed();
};
