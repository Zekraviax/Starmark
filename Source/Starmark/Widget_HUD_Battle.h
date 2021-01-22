#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/TextBlock.h"

#include "Widget_HUD_Battle.generated.h"

// Forward Declarations
class APlayerController_Base;


UCLASS()
class STARMARK_API UWidget_HUD_Battle : public UUserWidget
{
	GENERATED_BODY()

public:
	// Variables
	// --------------------------------------------------

	// ------------------------- Widget
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TurnOrderTextBlock;

	// ------------------------- References
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	APlayerController_Base* PlayerControllerReference;

// Functions
// --------------------------------------------------

// ------------------------- Commands
	UFUNCTION(BlueprintCallable)
	void MoveCommand();

	UFUNCTION(BlueprintCallable)
	void AttackCommand();

	UFUNCTION(BlueprintCallable)
	void SwitchCommand();
};
