#include "WelcomePage.h"
#include "Settings.h"
#include "IOHelper.h"
#include "AnsiHelper.h"
#include "SettingsPage.h"
#include "InstructionPage.h"

extern Settings LoadedSettings;
extern int LatestTerminalWidth, LatestTerminalHeight;

void PageEnter_Welcome()
{
    HideCursor();

    ClearScreen();

    printf("Zanim zaczniesz grę ...\n");
    printf("\nCzy widzisz ten znak [▶] (trójkąt):");
    printf("\n1. TAK");
    printf("\n2. NIE");

    char c = WaitForKeys('1', '2');

    if(c == '2')
    {
        LoadedSettings.FullUTF8Support = 0;
        SaveSettingsToFile();
        printf("\n\nWyłączono pełne wsparcie UTF-8.");
    }

    printf("\n\nCzy chcesz zobaczyć instrukcję obsługi gry?");
    printf("\n1. TAK");
    printf("\n2. NIE");

    c = WaitForKeys('1', '2');

    LoadedSettings.WelcomePageShown = 1;
    SaveSettingsToFile();

    if(c == '1')
    {
        PageEnter_Instruction();
        return;
    }

    printf("\n\nTo tyle, powodzenia w grze :)");

    printf("\nNaciśnij ENTER, aby kontynuować.");
    WaitForKeys(ENTER);
}
