#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"

#include "Widget_MultiplayerEndMenu.generated.h"


UCLASS()
class STARMARK_API UWidget_MultiplayerEndMenu : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ResultsText;
public:
	void AppendPlayerDataToResultsString(const FString& inString) const;
};