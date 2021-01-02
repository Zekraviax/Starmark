#include "Widget_CharacterCreator.h"

#include "Misc/DateTime.h"
#include "Misc/DefaultValueHelper.h"
#include "Kismet/GameplayStatics.h"
#include "Starmark_GameInstance.h"


// ------------------------- Widget
void UWidget_CharacterCreator::OnWidgetOpened()
{
	FDateTime* Date = new FDateTime;
	int CurrentYear = Date->FDateTime::Now().GetYear();

	if (Birthday_Day_DropDown->IsValidLowLevel()) {
		for (int d = 1; d <= 31; d++) {
			Birthday_Day_DropDown->AddOption(FString::FromInt(d));
		}

		Birthday_Day_DropDown->SetSelectedOption("1");
	}

	if (Birthday_Month_DropDown->IsValidLowLevel()) {
		for (int m = 1; m <= 12; m++) {
			Birthday_Month_DropDown->AddOption(FString::FromInt(m));
		}

		Birthday_Month_DropDown->SetSelectedOption("1");
	}

	if (Birthday_Year_DropDown->IsValidLowLevel()) {
		for (int y = CurrentYear; y >= 1900; y--) {
			Birthday_Year_DropDown->AddOption(FString::FromInt(y));
		}

		//Birthday_Year_DropDown->SetSelectedOption("2021");
	}
}


// ------------------------- Player
void UWidget_CharacterCreator::CalculateHoroscope()
{
	BirthDayAsNumber = FCString::Atoi(*Birthday_Day_DropDown->GetSelectedOption());
	BirthMonthAsNumber = FCString::Atoi(*Birthday_Month_DropDown->GetSelectedOption());
	BirthYearAsNumber = FCString::Atoi(*Birthday_Year_DropDown->GetSelectedOption());

	if (!GameInstanceReference) {
		GameInstanceReference = Cast<UStarmark_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	}

	switch (BirthMonthAsNumber)
	{
		case(1):
			if (BirthDayAsNumber <= 19) {
				GameInstanceReference->PlayerData.Horoscope = E_Player_Horoscopes::E_Capricorn;
				HoroscopeText->SetText(FText::FromString("Capricorn"));
				break;
			} else {
				GameInstanceReference->PlayerData.Horoscope = E_Player_Horoscopes::E_Aquarius;
				HoroscopeText->SetText(FText::FromString("Aquarius"));
				break;
			}
			break;
		case(2):
			if (BirthDayAsNumber <= 18) {
				GameInstanceReference->PlayerData.Horoscope = E_Player_Horoscopes::E_Aquarius;
				HoroscopeText->SetText(FText::FromString("Aquarius"));
				break;
			} else {
				GameInstanceReference->PlayerData.Horoscope = E_Player_Horoscopes::E_Pisces;
				HoroscopeText->SetText(FText::FromString("Pisces"));
				break;
			}
		case(3):
			if (BirthDayAsNumber <= 20) {
				GameInstanceReference->PlayerData.Horoscope = E_Player_Horoscopes::E_Pisces;
				HoroscopeText->SetText(FText::FromString("Pisces"));
				break;
			} else {
				GameInstanceReference->PlayerData.Horoscope = E_Player_Horoscopes::E_Aries;
				HoroscopeText->SetText(FText::FromString("Aries"));
				break;
			}
		case(4):
			if (BirthDayAsNumber <= 19) {
				GameInstanceReference->PlayerData.Horoscope = E_Player_Horoscopes::E_Aries;
				HoroscopeText->SetText(FText::FromString("Aries"));
				break;
			} else {
				GameInstanceReference->PlayerData.Horoscope = E_Player_Horoscopes::E_Taurus;
				HoroscopeText->SetText(FText::FromString("Taurus"));
				break;
			}
		case(5):
			if (BirthDayAsNumber <= 20) {
				GameInstanceReference->PlayerData.Horoscope = E_Player_Horoscopes::E_Taurus;
				HoroscopeText->SetText(FText::FromString("Taurus"));
				break;
			} else {
				GameInstanceReference->PlayerData.Horoscope = E_Player_Horoscopes::E_Gemini;
				HoroscopeText->SetText(FText::FromString("Gemini"));
				break;
			}
		case(6):
			if (BirthDayAsNumber <= 20) {
				GameInstanceReference->PlayerData.Horoscope = E_Player_Horoscopes::E_Gemini;
				HoroscopeText->SetText(FText::FromString("Gemini"));
				break;
			} else {
				GameInstanceReference->PlayerData.Horoscope = E_Player_Horoscopes::E_Cancer;
				HoroscopeText->SetText(FText::FromString("Cancer"));
				break;
			}
		case(7):
			if (BirthDayAsNumber <= 22) {
				GameInstanceReference->PlayerData.Horoscope = E_Player_Horoscopes::E_Cancer;
				HoroscopeText->SetText(FText::FromString("Cancer"));
				break;
			} else {
				GameInstanceReference->PlayerData.Horoscope = E_Player_Horoscopes::E_Leo;
				HoroscopeText->SetText(FText::FromString("Leo"));
				break;
			}
		case(8):
			if (BirthDayAsNumber <= 22) {
				GameInstanceReference->PlayerData.Horoscope = E_Player_Horoscopes::E_Leo;
				HoroscopeText->SetText(FText::FromString("Leo"));
				break;
			} else {
				GameInstanceReference->PlayerData.Horoscope = E_Player_Horoscopes::E_Virgo;
				HoroscopeText->SetText(FText::FromString("Virgo"));
				break;
			}
		case(9):
			if (BirthDayAsNumber <= 22) {
				GameInstanceReference->PlayerData.Horoscope = E_Player_Horoscopes::E_Virgo;
				HoroscopeText->SetText(FText::FromString("Virgo"));
				break;
			} else {
				GameInstanceReference->PlayerData.Horoscope = E_Player_Horoscopes::E_Libra;
				HoroscopeText->SetText(FText::FromString("Libra"));
				break;
			}
		case(10):
			if (BirthDayAsNumber <= 22) {
				GameInstanceReference->PlayerData.Horoscope = E_Player_Horoscopes::E_Libra;
				HoroscopeText->SetText(FText::FromString("Libra"));
				break;
			} else {
				GameInstanceReference->PlayerData.Horoscope = E_Player_Horoscopes::E_Scorpio;
				HoroscopeText->SetText(FText::FromString("Scorpio"));
				break;
			}
		case(11):
			if (BirthDayAsNumber <= 22) {
				GameInstanceReference->PlayerData.Horoscope = E_Player_Horoscopes::E_Scorpio;
				HoroscopeText->SetText(FText::FromString("Scorpio"));
				break;
			} else {
				GameInstanceReference->PlayerData.Horoscope = E_Player_Horoscopes::E_Sagittarius;
				HoroscopeText->SetText(FText::FromString("Sagittarius"));
				break;
			}
		case(12):
			if (BirthDayAsNumber <= 21) {
				GameInstanceReference->PlayerData.Horoscope = E_Player_Horoscopes::E_Sagittarius;
				HoroscopeText->SetText(FText::FromString("Sagittarius"));
				break;
			} else {
				GameInstanceReference->PlayerData.Horoscope = E_Player_Horoscopes::E_Capricorn;
				HoroscopeText->SetText(FText::FromString("Capricorn"));
				break;
			}
		default:
			// The default is already set to Capricorn
			break;
	}


}