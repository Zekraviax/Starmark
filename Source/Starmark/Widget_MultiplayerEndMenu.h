#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_MultiplayerEndMenu.generated.h"


UCLASS()
class STARMARK_API UWidget_MultiplayerEndMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	static void AppendPlayerDataToResultsString();
};