#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/TextBlock.h"
#include "Materials/Material.h"
#include "Starmark_Variables.h"

#include "WidgetComponent_Avatar.generated.h"


// Forward Declarations
class UWidget_AvatarCreation;


// Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAvatarSelected);


// Unique Enums
UENUM(BlueprintType)
enum class E_AvatarWidgetComponent_Function : uint8
{
	E_Nothing,
	E_CreateNewAvatarInLibrary,
	E_AddAvatarToChosenSlot,
};


UCLASS()
class STARMARK_API UWidgetComponent_Avatar : public UUserWidget
{
	GENERATED_BODY()

public:

// Variables
// --------------------------------------------------

// ------------------------- Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* AvatarName;

// ------------------------- Avatars
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAvatar_Struct AvatarData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterial* AvatarMaterial;

	// 0 - 3 for Active Avatars, 4 - 5 for Reserve Avatars
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int IndexInPlayerTeam = -1;

// ------------------------- References
	UPROPERTY()
	UUserWidget* PairedWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UWidget_AvatarCreation> AvatarCreationWidget_Class;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidget_AvatarCreation* AvatarCreationWidget_Reference;

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

	UFUNCTION()
	void ApplyNewAvatarData(FAvatar_Struct NewAvatarData);
};