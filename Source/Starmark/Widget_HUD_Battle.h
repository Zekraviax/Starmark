#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/Button.h"

#include "Widget_HUD_Battle.generated.h"


// Forward Declarations
class APlayerController_Base;
class UWidgetComponent_AvatarBattleData;


UCLASS()
class STARMARK_API UWidget_HUD_Battle : public UUserWidget
{
	GENERATED_BODY()

public:
// Variables
// --------------------------------------------------

// ------------------------- Components
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TurnOrderTextBlock;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* AvatarAttacksBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UWidgetComponent_AvatarBattleData* AvatarBattleDataWidget;

	// Buttons
	UPROPERTY(meta = (BindWidget))
	UButton* MoveCommandButton;

	UPROPERTY(meta = (BindWidget))
	UButton* AttackCommandButton;

	UPROPERTY(meta = (BindWidget))
	UButton* SwitchCommandButton;

	UPROPERTY(meta = (BindWidget))
	UButton* EndTurnCommandButton;

// ------------------------- References
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APlayerController_Base* PlayerControllerReference;

// Functions
// --------------------------------------------------

// ------------------------- Widget
	UFUNCTION(BlueprintCallable)
	void UpdateAvatarAttacksComponents();

	UFUNCTION(BlueprintCallable)
	void UpdateTurnOrderText(FString NewText);

	UFUNCTION(BlueprintCallable)
	void OnPlayerClick();

	UFUNCTION(BlueprintCallable)
	void OnPlayerCurrentlyActingStateChanged();

// ------------------------- Commands
	UFUNCTION(BlueprintCallable)
	void MoveCommand();

	UFUNCTION(BlueprintCallable)
	void AttackCommand();

	UFUNCTION(BlueprintCallable)
	void SwitchCommand();

	UFUNCTION(BlueprintCallable)
	void EndCommand();
};
