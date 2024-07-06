#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/UniformGridPanel.h"

#include "Widget_AvatarLibrary.generated.h"


// Forward Declarations
class UWidgetComponent_Avatar;
class UStarmark_GameInstance;


UCLASS()
class STARMARK_API UWidget_AvatarLibrary : public UUserWidget
{
	GENERATED_BODY()
	
public:

// Variables
// --------------------------------------------------

// ------------------------- Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UUniformGridPanel* AvatarLibraryUniformGridPanel;

// ------------------------- References
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UWidgetComponent_Avatar> AvatarWidgetComponent_Class;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent_Avatar* AvatarWidgetComponent_Reference;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStarmark_GameInstance* GameInstanceReference;

// ------------------------- Materials
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* QuestionMarkMaterial;


// Functions
// --------------------------------------------------

// ------------------------- Widget
	UFUNCTION(BlueprintCallable)
	void OnWidgetOpened();

	UFUNCTION()
	void UpdateAllAvatarsInTeam();

// ------------------------- Delegates
	UFUNCTION()
	void BindAvatarCreatedDelegate(const UWidgetComponent_Avatar* AvatarWidgetComponentReference);

	UFUNCTION()
	void OnAvatarCreatedDelegateBroadcast();

	UFUNCTION()
	void OnAvatarChangedSlotDelegateBroadcast();
};
