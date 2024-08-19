#include "Widget_MultiplayerEndMenu.h"


void UWidget_MultiplayerEndMenu::AppendPlayerDataToResultsString(const FString& inString) const
{
	FString Text = ResultsText->Text.ToString() + inString + "\n";
	ResultsText->SetText(FText::FromString(Text));
}
