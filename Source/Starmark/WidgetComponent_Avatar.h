#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Starmark_Variables.h"

#include "WidgetComponent_Avatar.generated.h"


// Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAvatarSelected);


// Unique Enums
UENUM(BlueprintType)
enum class E_AvatarWidgetComponent_Function : uint8
{
	E_Nothing,
	E_CreateNewAvatarInLibrary
};


UCLASS()
class STARMARK_API UWidgetComponent_Avatar : public UUserWidget
{
	GENERATED_BODY()

public:

// Variables
// --------------------------------------------------

// ------------------------- Avatars
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAvatar_Struct AvatarData;

// ------------------------- References
	UPROPERTY()
	UUserWidget* PairedWidget;

// ------------------------- Widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	E_AvatarWidgetComponent_Function CurrentFunction;

// ------------------------- Delegates
	UPROPERTY()
	FAvatarSelected OnAvatarSelectedDelegate;


// Functions
// --------------------------------------------------

// ------------------------- Widget
	UFUNCTION(BlueprintCallable)
	void OnInteractButtonPressed();

	UFUNCTION(BlueprintNativeEvent)
	void UpdateWidgetMaterials();
};
