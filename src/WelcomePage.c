#include "WelcomePage.h"
#include "Settings.h"
#include "PageUtils.h"
#include "AnsiHelper.h"
#include "SettingsPage.h"
#include "InstructionPage.h"

extern Settings* LoadedSettings;

extern int LatestTerminalWidth, LatestTerminalHeight;

void PrintSettingsColors() {
    int bgOffset = 10;
    SetColor(LoadedSettings->CorrectAnswerColor + bgOffset);
    printf("W");
    SetColor(LoadedSettings->WrongAnswerColor + bgOffset);
    printf("i");
    SetColor(LoadedSettings->SelectedAnswerColor + bgOffset);
    printf("t");
    SetColor(LoadedSettings->ConfirmedAnswerColor + bgOffset);
    printf("a");
    SetColor(LoadedSettings->SupportColor + bgOffset);
    printf("j");
}

void PageEnter_Welcome()
{
    HideCursor();

    ClearScreen();

    printf("Zanim zaczniesz grę ...\n");
    printf("\nCzy widzisz ten znak [%c%c%c] (trójkąt):", 0xE2, 0x96, 0xB6);
    printf("\n1. TAK");
    printf("\n2. NIE");

    char c = WaitForKeys('1', '2');

    if(c == '2') {
        LoadedSettings->FullUTF8Support = 0;
        SaveSettings(LoadedSettings);
        printf("\n\nWyłączono pełne wsparcie UTF-8.");
    }

    printf("\n\nCzy możesz łatwo odczytać ten tekst: ");
    PrintSettingsColors();
    ResetColor();
    PrintWrappedLine(
        "\nChodzi o to abyś mógł odróżnić te kolory od domyślnego koloru tekstu ponieważ niektóre z tych kolorów mogą być użyte do odróżnienia zanaczonych odpowiedzi.", 
        LatestTerminalWidth, 0, false);
    printf("\n1. TAK");
    printf("\n2. NIE, przejdź do ustawień abym mógł zmienić kolory");

    c = WaitForKeys('1', '2');

    printf("\n\n");

    if(c == '2') {
        PageEnter_Settings();
        ClearScreen();
    }

    printf("Czy chcesz zobaczyć instrukcję obsługi gry?");
    printf("\n1. TAK");
    printf("\n2. NIE");

    c = WaitForKeys('1', '2');

    LoadedSettings->TutorialShown = 1;
    SaveSettings(LoadedSettings);

    if(c == '1') {
        PageEnter_Instruction();
        return;
    }

    printf("\n\nTo tyle, powodzenia w grze :)");

    printf("\nNaciśnij ENTER, aby kontynuować.");
    WaitForKeys(ENTER);
}