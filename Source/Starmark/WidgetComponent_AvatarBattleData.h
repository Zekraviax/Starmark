#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Starmark_Variables.h"

#include "WidgetComponent_AvatarBattleData.generated.h"

// Forward Declarations
class ACharacter_Pathfinder;


UCLASS()
class STARMARK_API UWidgetComponent_AvatarBattleData : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
	
public:
// Variables
// --------------------------------------------------

// ------------------------- Components
	// Health Bar
	// Mana Bar
	// Name
	// Types
	// Level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* ManaBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* AvatarText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* NicknameText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* LevelText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TypesText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* HealthDisplayText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ManaDisplayText;

// ------------------------- Avatar
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAvatar_Struct LinkedAvatar;

// Functions
// --------------------------------------------------

// ------------------------- Avatar
	UFUNCTION(BlueprintCallable)
	void UpdateAvatarData(FAvatar_Struct NewLinkedAvatar);
};
