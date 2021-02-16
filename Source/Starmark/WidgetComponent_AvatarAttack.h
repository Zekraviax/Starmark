#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Starmark_GameMode.h"

#include "WidgetComponent_AvatarAttack.generated.h"

// Forward Declarations
class ACharacter_Pathfinder;
class APlayerController_Base;


UCLASS()
class STARMARK_API UWidgetComponent_AvatarAttack : public UUserWidget
{
	GENERATED_BODY()

public:
// Variables
// --------------------------------------------------

// ------------------------- Components
	UPROPERTY(meta = (BindWidget))
	UButton* SelectButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AttackNameText;

// ------------------------- References
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//UWidget_HUD_Battle* BattleHUDReference;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//FAvatar_AttackStruct BoundAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APlayerController_Base* PlayerControllerReference;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int AvatarAttackIndex;


// Functions
// --------------------------------------------------

// ------------------------- Widget
	//UFUNCTION()
	//void OnWidgetBegin();

	UFUNCTION(BlueprintCallable)
	void OnButtonPressed();
};
