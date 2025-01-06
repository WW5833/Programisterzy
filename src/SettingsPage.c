#include "SettingsPage.h"
#include "AnsiHelper.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "PageUtils.h"
#include "IOHelper.h"

extern Settings* LoadedSettings;

#define OPTION_COUNT 9

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
    char buffer[256];
    sprintf(buffer, "Pełne wsparcie UTF-8 | Jeśli ten znak [%c%c%c] (trójkąt) nie jest poprawnie wyświetlany, wyłącz tę opcję | Włączone: %s", 0xE2, 0x96, 0xB6, (support ? "TAK\0" : "NIE\0"));
    PrintWrappedLine(&buffer[0], terminalWidth - 4, 4, false);
}

int* GetOptionColor(int selected) {
    switch (selected)
    {
        case 0:
            return &(LoadedSettings->CorrectAnswerColor);
        case 1:
            return &LoadedSettings->WrongAnswerColor;
        case 2:
            return &LoadedSettings->SelectedAnswerColor;
        case 3:
            return &LoadedSettings->ConfirmedAnswerColor;
        case 4:
            return &LoadedSettings->SupportColor;

        default:
            fprintf(stderr, "Invalid option index: %d\n", selected);
            ExitApp(EXIT_FAILURE);
    }
}

void PageEnter_Settings()
{
    HideCursor();

    int terminalWidth, terminalHeight;
    GetTerminalSize(&terminalWidth, &terminalHeight);

    int colorsX, colorsY;
    int utf8SupportX, utf8SupportY;
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
    
    printf("\n[ ] Pokaż poprawną odpowiedź po przegranej: ");
    GetCursorPosition(&showCorrectWhenWrongX, &showCorrectWhenWrongY);
    printf(LoadedSettings->ShowCorrectWhenWrong ? "TAK" : "NIE");
    printf("\n[ ] Zapisz i wróć do menu głównego");
    printf("\n[ ] Wróć do menu głównego bez zapisywania");

    int lineIndexes[OPTION_COUNT] = 
    { 
        2, 
        3 + (slimMode ? 1 : 0), 
        4 + (slimMode ? 2 : 0), 
        5 + (slimMode ? 3 : 0), 
        6 + (slimMode ? 4 : 0), 
        7 + (slimMode ? 5 : 0), 
        showCorrectWhenWrongY, 
        showCorrectWhenWrongY + 1,
        showCorrectWhenWrongY + 2
    };

    int selected = 0;

    while (true)
    {
        KeyInputType key = HandleInteractions(true);
        switch (key)
        {
            case KEY_ARROW_UP:
                SetCursorPosition(2, lineIndexes[selected]);
                printf(" ");

                selected--;
                if (selected < 0) selected = OPTION_COUNT - 1;
                
                SetCursorPosition(2, lineIndexes[selected]);
                printf("*");
                break;

            case KEY_ARROW_DOWN:
                SetCursorPosition(2, lineIndexes[selected]);
                printf(" ");

                selected++;
                if (selected >= OPTION_COUNT) selected = 0;
                
                SetCursorPosition(2, lineIndexes[selected]);
                printf("*");
                break;
                
            case KEY_ENTER:
                if(selected == 5) {
                    LoadedSettings->FullUTF8Support = !LoadedSettings->FullUTF8Support;
                    SetCursorPosition(utf8SupportX, utf8SupportY);
                    PrintUTF8Support(LoadedSettings->FullUTF8Support, terminalWidth);
                }
                else if(selected == 6) {
                    LoadedSettings->ShowCorrectWhenWrong = !LoadedSettings->ShowCorrectWhenWrong;
                    SetCursorPosition(showCorrectWhenWrongX, showCorrectWhenWrongY);
                    printf(LoadedSettings->ShowCorrectWhenWrong ? "TAK" : "NIE");
                }
                else if(selected == OPTION_COUNT - 2) {
                    SaveSettings(LoadedSettings);
                    return;
                }
                else if(selected == OPTION_COUNT - 1) {
                    LoadedSettings = LoadSettings();
                    return;
                }
                break;
                
            case KEY_ARROW_LEFT: {
                if(selected > 4) break;
                int* option = GetOptionColor(selected);
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
                break;
            }
                
            case KEY_ARROW_RIGHT: {
                if(selected > 4) break;
                int* option = GetOptionColor(selected);
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
                break;
            }

            case KEY_ESCAPE: 
                LoadedSettings = LoadSettings();
                return;
            
            default:
                break;
        }
    }
}
