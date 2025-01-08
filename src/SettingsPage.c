#include "SettingsPage.h"
#include "AnsiHelper.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "PageUtils.h"
#include "IOHelper.h"

extern Settings* LoadedSettings;

#define COLOR_PALET_WIDTH 30

#define OPTION_COUNT 10

typedef struct {
    int terminalWidth;
    int terminalHeight;

    bool slimMode;

    int selected;

    int colorsX, colorsY;
    int utf8SupportX, utf8SupportY;
    int showCorrectWhenWrongX, showCorrectWhenWrongY;
    int enableMouseSupportX, enableMouseSupportY;

    int lineIndexes[OPTION_COUNT];
} SettingsPageData;

void PrintColors(SettingsPageData* data, int color) {
    if(data->slimMode) printf("\n    ");
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

void PrintUTF8Support(SettingsPageData* data, bool support) {
    char buffer[256];
    sprintf(buffer, "Pełne wsparcie UTF-8 | Jeśli ten znak [%c%c%c] (trójkąt) nie jest poprawnie wyświetlany, wyłącz tę opcję | Włączone: %s", 0xE2, 0x96, 0xB6, (support ? "TAK\0" : "NIE\0"));
    PrintWrappedLine(&buffer[0], data->terminalWidth - 4, 4, false);
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

void DrawSettingsUI(SettingsPageData* data) {
    ClearScreen();
    printf("Ustawienia");
    printf("\n[ ] Kolor poprawnej odpowiedzi     ");
    GetCursorPosition(&data->colorsX, &data->colorsY);

    data->slimMode = (data->terminalWidth - data->colorsX) < COLOR_PALET_WIDTH;

    PrintColors(data, LoadedSettings->CorrectAnswerColor);
    printf("\n[ ] Kolor błędnej odpowiedzi       ");
    PrintColors(data, LoadedSettings->WrongAnswerColor);
    printf("\n[ ] Kolor zaznaczonej odpowiedzi   ");
    PrintColors(data, LoadedSettings->SelectedAnswerColor);
    printf("\n[ ] Kolor potwierdzonej odpowiedzi ");
    PrintColors(data, LoadedSettings->ConfirmedAnswerColor);
    printf("\n[ ] Kolor wsparcia                 ");
    PrintColors(data, LoadedSettings->SupportColor);

    printf("\n[ ] ");
    GetCursorPosition(&data->utf8SupportX, &data->utf8SupportY);
    PrintUTF8Support(data, LoadedSettings->FullUTF8Support);
    
    printf("\n[ ] Pokaż poprawną odpowiedź po przegranej: ");
    GetCursorPosition(&data->showCorrectWhenWrongX, &data->showCorrectWhenWrongY);
    printf(LoadedSettings->ShowCorrectWhenWrong ? "TAK" : "NIE");
    
    printf("\n[ ] Włącz wsparcie myszki: ");
    GetCursorPosition(&data->enableMouseSupportX, &data->enableMouseSupportY);
    printf(LoadedSettings->EnableMouseSupport ? "TAK" : "NIE");

    printf("\n");

    printf("\n[ ] Zapisz i wróć do menu głównego");
    printf("\n[ ] Wróć do menu głównego bez zapisywania");

    data->lineIndexes[0] = 2;
    data->lineIndexes[1] = 3 + (data->slimMode ? 1 : 0);
    data->lineIndexes[2] = 4 + (data->slimMode ? 2 : 0);
    data->lineIndexes[3] = 5 + (data->slimMode ? 3 : 0);
    data->lineIndexes[4] = 6 + (data->slimMode ? 4 : 0);
    data->lineIndexes[5] = 7 + (data->slimMode ? 5 : 0);

    data->lineIndexes[6] = data->showCorrectWhenWrongY;
    data->lineIndexes[7] = data->enableMouseSupportY;
    data->lineIndexes[8] = data->enableMouseSupportY + 2;
    data->lineIndexes[9] = data->enableMouseSupportY + 3;

    SetCursorPosition(2, data->lineIndexes[data->selected]);
    printf("*");
}

void HandleArrowUpDownKeys(SettingsPageData* data, int direction) {
    SetCursorPosition(2, data->lineIndexes[data->selected]);
    printf(" ");

    data->selected += direction;
    if (data->selected < 0) data->selected = OPTION_COUNT - 1;
    if (data->selected >= OPTION_COUNT) data->selected = 0;

    SetCursorPosition(2, data->lineIndexes[data->selected]);
    printf("*");
}

bool HandleEnterKey(SettingsPageData* data) {
    switch(data->selected) {
        case 5:
            LoadedSettings->FullUTF8Support = !LoadedSettings->FullUTF8Support;
            SetCursorPosition(data->utf8SupportX, data->utf8SupportY);
            PrintUTF8Support(data, LoadedSettings->FullUTF8Support);
            break;
        case 6:
            LoadedSettings->ShowCorrectWhenWrong = !LoadedSettings->ShowCorrectWhenWrong;
            SetCursorPosition(data->showCorrectWhenWrongX, data->showCorrectWhenWrongY);
            printf(LoadedSettings->ShowCorrectWhenWrong ? "TAK" : "NIE");
            break;
        case 7:
            LoadedSettings->EnableMouseSupport = !LoadedSettings->EnableMouseSupport;
            SetCursorPosition(data->enableMouseSupportX, data->enableMouseSupportY);
            printf(LoadedSettings->EnableMouseSupport ? "TAK" : "NIE");

            if(!LoadedSettings->EnableMouseSupport) {
                EnableMouseInput(false);
            }
            
            break;

        case OPTION_COUNT - 2:
            SaveSettings(LoadedSettings);
            return true;

        case OPTION_COUNT - 1:
            LoadedSettings = LoadSettings(); // Reload settings
            return true;
    }
    
    return false;
}

void HandleArrowLeftRightKeys(SettingsPageData* data, int direction) {
    if(data->selected > 4) return;

    int* option = GetOptionColor(data->selected);
    *option = *option + direction;

    if(direction > 0) {
        if(*option > COLOR_FG_WHITE + COLOR_BRIGHT_MOD) {
            *option = COLOR_FG_RED;
        }
        else if(*option > COLOR_FG_WHITE && *option <= COLOR_FG_BLACK + COLOR_BRIGHT_MOD) {
            *option = COLOR_FG_RED + COLOR_BRIGHT_MOD;
        }
    }
    else {
        if (*option <= COLOR_FG_BLACK)
        {
            *option = COLOR_FG_WHITE + COLOR_BRIGHT_MOD;
        }
        else if(*option > COLOR_FG_WHITE && *option <= COLOR_FG_BLACK + COLOR_BRIGHT_MOD)
        {
            *option = COLOR_FG_WHITE;
        }
    }

    SetCursorPosition(data->colorsX, data->lineIndexes[data->selected]);
    PrintColors(data, *option);
}

void OnSettingsPageResize(int width, int height, void* data) {
    SettingsPageData* pageData = (SettingsPageData*)data;
    pageData->terminalWidth = width;
    pageData->terminalHeight = height;

    DrawSettingsUI(pageData);
}

extern int LatestTerminalWidth, LatestTerminalHeight;

void PageEnter_Settings()
{
    HideCursor();

    SettingsPageData* data = malloc(sizeof(SettingsPageData));
    data->terminalWidth = LatestTerminalWidth;
    data->terminalHeight = LatestTerminalHeight;
    data->selected = 0;

    DrawSettingsUI(data);

    SetResizeHandler(OnSettingsPageResize, data);

    bool continueLoop = true;
    while (continueLoop)
    {
        KeyInputType key = HandleInteractions(true);
        switch (key)
        {
            case KEY_ARROW_UP:
                HandleArrowUpDownKeys(data, -1);
                break;

            case KEY_ARROW_DOWN:
                HandleArrowUpDownKeys(data, 1);
                break;
                
            case KEY_ENTER:
                if(HandleEnterKey(data)) {
                    continueLoop = false; // Exit page
                }
                break;
                
            case KEY_ARROW_LEFT:
                HandleArrowLeftRightKeys(data, -1);
                break;
                
            case KEY_ARROW_RIGHT: 
                HandleArrowLeftRightKeys(data, 1);
                break;

            case KEY_ESCAPE: 
                LoadedSettings = LoadSettings(); // Reload settings
                continueLoop = false; // Exit page
                break;

            default:
                break;
        }
    }

    UnsetResizeHandler();
    free(data);
    data = NULL;
}
