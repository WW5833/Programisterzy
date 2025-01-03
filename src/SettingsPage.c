#include "SettingsPage.h"
#include "AnsiHelper.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "PageUtils.h"

extern Settings* LoadedSettings;

#define OPTION_COUNT 10

bool slimMode = false;

void PrintColors(int color) {
    if(slimMode) printf("\n    ");
    ResetColor();
    printf("|");
    for (int i = 1; i < 8; i++)
    {
        SetColors(COLOR_FG_BLACK, COLOR_BG_BLACK + i);
        printf((color == COLOR_FG_BLACK + i) ? "*" : " ");
        ResetColor();
        printf("|");
    }

    for (int i = 1; i < 8; i++)
    {
        SetColors(COLOR_FG_BLACK, COLOR_BG_BLACK + i + COLOR_BRIGHT_MOD);
        printf((color == COLOR_FG_BLACK + i + COLOR_BRIGHT_MOD) ? "*" : " ");
        ResetColor();
        printf("|");
    }
}

void PrintUTF8Support(bool support, int terminalWidth) {
    char buffer[128];
    sprintf(buffer, "Pełne wsparcie UTF-8 | Jeśli ten znak [%c%c%c] nie jest poprawnie wyświetlany, wyłącz tę opcję | Włączone: %s", 0xE2, 0x96, 0xB6, (support ? "TAK\0" : "NIE\0"));
    PrintWrappedLine(&buffer[0], terminalWidth - 4, 4, false);
}

void PageEnter_Settings()
{
    HideCursor();

    int terminalWidth, terminalHeight;
    GetTerminalSize(&terminalWidth, &terminalHeight);

    int colorsX, colorsY;
    int utf8SupportX, utf8SupportY;
    int turorialShownX, tutorialShownY;
    int autoResizeX, autoResizeY;
    int showCorrectWhenWrongX, showCorrectWhenWrongY;
    ClearScreen();
    printf("Ustawienia");
    printf("\n[*] Kolor poprawnej odpowiedzi     ");
    GetCursorPosition(&colorsX, &colorsY);

    const int colorPaletWidth = 30;

    slimMode = (terminalWidth - colorsX) < colorPaletWidth;

    PrintColors(LoadedSettings->CorrectAnswerColor);
    printf("\n[ ] Kolor błędnej odpowiedzi       ");
    PrintColors(LoadedSettings->WrongAnswerColor);
    printf("\n[ ] Kolor zaznaczonej odpowiedzi   ");
    PrintColors(LoadedSettings->SelectedAnswerColor);
    printf("\n[ ] Kolor potwierdzonej odpowiedzi ");
    PrintColors(LoadedSettings->ConfirmedAnswerColor);
    printf("\n[ ] Kolor wsparcia                 ");
    PrintColors(LoadedSettings->SupportColor);

    printf("\n[ ] ");
    GetCursorPosition(&utf8SupportX, &utf8SupportY);
    PrintUTF8Support(LoadedSettings->FullUTF8Support, terminalWidth);
    
    printf("\n[ ] Pokaż tutorial: ");
    GetCursorPosition(&turorialShownX, &tutorialShownY);
    printf(!LoadedSettings->TutorialShown ? "TAK" : "NIE");
    printf("\n[ ] Automatyczne reskalowanie konsoli: ");
    GetCursorPosition(&autoResizeX, &autoResizeY);
    printf(LoadedSettings->AutoResizeUI ? "TAK" : "NIE");
    printf("\n[ ] Pokaż poprawną odpowiedź po przegranej: ");
    GetCursorPosition(&showCorrectWhenWrongX, &showCorrectWhenWrongY);
    printf(LoadedSettings->ShowCorrectWhenWrong ? "TAK" : "NIE");
    printf("\n[ ] Zapisz i wróć do menu głównego");

    int lineIndexes[OPTION_COUNT] = 
    { 
        2, 
        3 + (slimMode ? 1 : 0), 
        4 + (slimMode ? 2 : 0), 
        5 + (slimMode ? 3 : 0), 
        6 + (slimMode ? 4 : 0), 
        7 + (slimMode ? 5 : 0), 
        tutorialShownY, 
        tutorialShownY + 1, 
        autoResizeY + 1,
        showCorrectWhenWrongY + 1
    };

    int selected = 0;

    while (true)
    {
        KeyInputType key = HandleInteractions(true);
        if (key == KEY_ARROW_UP)
        {
            SetCursorPosition(2, lineIndexes[selected]);
            printf(" ");

            selected--;
            if (selected < 0) selected = OPTION_COUNT - 1;
            
            SetCursorPosition(2, lineIndexes[selected]);
            printf("*");
        }
        else if (key == KEY_ARROW_DOWN)
        {
            SetCursorPosition(2, lineIndexes[selected]);
            printf(" ");

            selected++;
            if (selected >= OPTION_COUNT) selected = 0;
            
            SetCursorPosition(2, lineIndexes[selected]);
            printf("*");
        }
        else if (key == KEY_ENTER)
        {
            if(selected == 5) {
                LoadedSettings->FullUTF8Support = !LoadedSettings->FullUTF8Support;
                SetCursorPosition(utf8SupportX, utf8SupportY);
                PrintUTF8Support(LoadedSettings->FullUTF8Support, terminalWidth);
            }
            else if(selected == 6) {
                LoadedSettings->TutorialShown = !LoadedSettings->TutorialShown;
                SetCursorPosition(turorialShownX, tutorialShownY);
                printf(!LoadedSettings->TutorialShown ? "TAK" : "NIE");
            }
            else if(selected == 7) {
                LoadedSettings->AutoResizeUI = !LoadedSettings->AutoResizeUI;
                SetCursorPosition(autoResizeX, autoResizeY);
                printf(LoadedSettings->AutoResizeUI ? "TAK" : "NIE");
            }
            else if(selected == 8) {
                LoadedSettings->ShowCorrectWhenWrong = !LoadedSettings->ShowCorrectWhenWrong;
                SetCursorPosition(showCorrectWhenWrongX, showCorrectWhenWrongY);
                printf(LoadedSettings->ShowCorrectWhenWrong ? "TAK" : "NIE");
            }
            else if(selected == OPTION_COUNT - 1) {
                SaveSettings(LoadedSettings);
                return;
            }
        }
        else if (key == KEY_ARROW_LEFT)
        {
            int* option = NULL;
            switch (selected)
            {
                case 0:
                    option = &LoadedSettings->CorrectAnswerColor;
                    break;
                case 1:
                    option = &LoadedSettings->WrongAnswerColor;
                    break;
                case 2:
                    option = &LoadedSettings->SelectedAnswerColor;
                    break;
                case 3:
                    option = &LoadedSettings->ConfirmedAnswerColor;
                    break;
                case 4:
                    option = &LoadedSettings->SupportColor;
                    break;
            }
            *option = *option - 1;
            if (*option <= COLOR_FG_BLACK)
            {
                *option = COLOR_FG_WHITE + COLOR_BRIGHT_MOD;
            }
            else if (*option <= COLOR_FG_BLACK + COLOR_BRIGHT_MOD && *option > COLOR_FG_WHITE+1)
            {
                *option = COLOR_FG_WHITE;
            }

            SetCursorPosition(colorsX, lineIndexes[selected]);
            PrintColors(*option);
        }
        else if (key == KEY_ARROW_RIGHT)
        {
            int* option = NULL;
            switch (selected)
            {
                case 0:
                    option = &(LoadedSettings->CorrectAnswerColor);
                    break;
                case 1:
                    option = &LoadedSettings->WrongAnswerColor;
                    break;
                case 2:
                    option = &LoadedSettings->SelectedAnswerColor;
                    break;
                case 3:
                    option = &LoadedSettings->ConfirmedAnswerColor;
                    break;
                case 4:
                    option = &LoadedSettings->SupportColor;
                    break;
            }
            *option = *option + 1;
            if (*option > COLOR_FG_WHITE + COLOR_BRIGHT_MOD)
            {
                *option = COLOR_FG_BLACK+1;
            }
            else if (*option == COLOR_FG_WHITE+1)
            {
                *option = COLOR_FG_BLACK+1+COLOR_BRIGHT_MOD;
            }

            SetCursorPosition(colorsX, lineIndexes[selected]);
            PrintColors(*option);
        }
    }
}
