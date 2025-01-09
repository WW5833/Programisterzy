#include "WelcomePage.h"
#include "Settings.h"
#include "PageUtils.h"
#include "AnsiHelper.h"
#include "SettingsPage.h"
#include "InstructionPage.h"

extern Settings* LoadedSettings;

extern int LatestTerminalWidth, LatestTerminalHeight;

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

    printf("\n\nCzy chcesz zobaczyć instrukcję obsługi gry?");
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