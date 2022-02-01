#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APlayerController_Base* PlayerControllerReference;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int AvatarAttackIndex;

// Functions
// --------------------------------------------------

// ------------------------- Widget
	UFUNCTION(BlueprintCallable)
	void OnButtonPressed();
};
