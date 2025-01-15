#include "SettingsPage.h"
#include "AnsiHelper.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "IOHelper.h"
#include "RGBColors.h"
#include "TextHelper.h"
#include "Utf8Symbols.h"
#include <math.h>

extern Settings LoadedSettings;

#define COLOR_PALET_WIDTH (RGB_COLOR_COUNT * 2 + 1)

#define OPTION_COUNT 11

typedef struct {
    int terminalWidth;
    int terminalHeight;

    bool slimMode;

    int selected;

    int colorsX, colorsY;
    int utf8SupportX, utf8SupportY;
    int showCorrectWhenWrongX, showCorrectWhenWrongY;
    int enableMouseSupportX, enableMouseSupportY;
    int darkModeX, darkModeY;

    int lineIndexes[OPTION_COUNT];

    int descriptionHeight;
} SettingsPageData;

static void DrawUI_PrintColors(SettingsPageData* data, int color) {
    if(data->slimMode) printf("\n    ");
    ResetColor();
    printf("|");
    for (int i = 0; i < RGB_COLOR_COUNT; i++)
    {
        SetColorRGBPreset(i, true);
        printf((color == i) ? "*" : " ");
        ResetColor();
        printf("|");
    }
}

static void DrawUI_SettingValue(SettingsPageData* data, int index) {
    bool value;
    switch(index) {
        case 5:
            SetCursorPosition(data->utf8SupportX, data->utf8SupportY);
            value = LoadedSettings.FullUTF8Support;
            break;
        case 6:
            SetCursorPosition(data->showCorrectWhenWrongX, data->showCorrectWhenWrongY);
            value = LoadedSettings.ShowCorrectWhenWrong;
            SetColorRGBPreset(value ? LoadedSettings.CorrectAnswerColor : LoadedSettings.WrongAnswerColor, false);
            if(data->terminalWidth - data->showCorrectWhenWrongX < 4) {
                if(data->terminalWidth - data->showCorrectWhenWrongX < 2) {
                    printf(CSR_MOVE_LEFT(1));
                }
                printf(value ? "T" : "N");
            }
            else {
                printf(value ? "TAK" : "NIE");
            }
            ResetColor();
            return;
        case 7:
            SetCursorPosition(data->enableMouseSupportX, data->enableMouseSupportY);
            value = LoadedSettings.EnableMouseSupport;
            break;
        case 8:
            SetCursorPosition(data->darkModeX, data->darkModeY);
            value = LoadedSettings.DarkMode;
            break;

        default:
            ExitAppWithErrorFormat(EXIT_FAILURE, ERRMSG_INVALID_OPTION_INDEX(data->selected));
    }

    SetColorRGBPreset(value ? LoadedSettings.CorrectAnswerColor : LoadedSettings.WrongAnswerColor, false);
    printf(value ? "TAK" : "NIE");
    ResetColor();
}

static int* GetOptionColor(int selected) {
    switch (selected)
    {
        case 0:
            return &(LoadedSettings.CorrectAnswerColor);
        case 1:
            return &LoadedSettings.WrongAnswerColor;
        case 2:
            return &LoadedSettings.SelectedAnswerColor;
        case 3:
            return &LoadedSettings.ConfirmedAnswerColor;
        case 4:
            return &LoadedSettings.SupportColor;

        default:
            ExitAppWithErrorFormat(EXIT_FAILURE, ERRMSG_INVALID_OPTION_INDEX(selected));
    }
}

static const char CorrectColorDescription[] = 
"Ten kolor jest używany do oznaczenia:\n"
"- poprawnej odpowiedzi oraz sugesti przyjaciela\n"
"- dostępnych kół ratunkowych\n"
"- komunikatu o wygranej grze\n"
"- wartości wygranej w grze\n"
"- osiągniętych bezpiecznych progów\n"
"- włączonych opcji";

static const char WrongColorDescription[] =
"Ten kolor jest używany do oznaczenia:\n"
"- błędnej odpowiedzi\n"
"- zużytych kół ratunkowych\n"
"- błędnych odpowiedzi przy koła ratunkowego 50/50\n"
"- komunikatu o przegranej grze\n"
"- wyłączonych opcji";

static const char SelectedColorDescription[] =
"Ten kolor jest używany do oznaczenia:\n"
"- zaznaczonej odpowiedzi\n"
"- zaznaczonego koła ratunkowego przy użyciu myszki\n"
"- nagrody za obecne pytanie";

static const char ConfirmedColorDescription[] =
"Ten kolor jest używany do oznaczenia:\n"
"- zaznaczonej i oczekującej na potwierdzenie odpowiedzi\n"
"- zaznaczonego i oczekującego na potwierdzenie koła ratunkowego\n"
"- liter pytań";

static const char SupportColorDescription[] =
"Ten kolor jest używany do oznaczenia:\n"
"- aktywnych kół ratunkowych\n"
"- nieosiągniętych bezpiecznych progów\n"
"- do wskaźników procentów w kole ratunkowych: Głos publiczności\n"
"- komunikat o rezygnacji z gry";

static const char Utf8SupportDescription[] =
"Starsze terminale (np. Cmd, PowerShell na Windows 10) nie wspierają wszystkich znaków UTF-8,"
" co może prowadzić do nieprawidłowego wyświetlania niektórych znaków."
" Gdy ta opcja jest wyłączona, program używa znaków ASCII zamiast niedostepnych znaków UTF-8."
"\nPrecyzyjniej: Znak [>] jest używany zamiast [▶] (trójkąt), a w kole ratunknowym \"Głos publiczności\" wykres jest mniej dokładny.";

static const char ShowCorrectWhenWrongDescription[] =
"Po udzieleniu błędnej odpowiedzi program zaznaczy poprawną odpowiedź.";

static const char EnableMouseSupportDescription[] =
"Umożliwia korzystanie z myszki w grze.\n"
"Kliknięcie na odpowiedź zaznacza ją, a kliknięcie na koło ratunkowe je aktywuje. (Wymagane ponowne kliknięcie aby potwierdzić)\n"
"Pozwala na nawigację po liście pytań za pomocą myszki.";

static const char DarkModeDescription[] =
"Włącza tryb ciemny.\n\n"
"Zmienia kolor tła na czarny, a tekst na biały.\n"
"Zmienia odcienie kolorów na bardziej przyjazne dla oczu biorąc pod uwagę kolor tła.";

static const char SaveAndExitDescription[] =
"Zapisuje zmiany i wraca do menu głównego.";

static const char ExitWithoutSaveDescription[] =
"Anuluje zmiany i wraca do menu głównego.";

static const char* Descriptions[OPTION_COUNT] = {
    CorrectColorDescription,
    WrongColorDescription,
    SelectedColorDescription,
    ConfirmedColorDescription,
    SupportColorDescription,
    Utf8SupportDescription,
    ShowCorrectWhenWrongDescription,
    EnableMouseSupportDescription,
    DarkModeDescription,
    SaveAndExitDescription,
    ExitWithoutSaveDescription,
};

static void UpdateDescriptionHeight(SettingsPageData* data) {
    const int width = data->terminalWidth - 4;
    int maxLines = 0;

    for (int i = 0; i < OPTION_COUNT; i++)
    {
        int lineCount = GetWrappedLineCount(Descriptions[i], width);
        if(lineCount > maxLines) {
            maxLines = lineCount;
        }
    }

    data->descriptionHeight = maxLines + 1;
}

static void DrawUI_UpdateSettingDescription(SettingsPageData* data) {
    if(data->terminalHeight <= data->lineIndexes[OPTION_COUNT - 1] + data->descriptionHeight) {
        return;
    }

    const int maxTextLines = data->descriptionHeight - 1;
    for (int i = 0; i < data->descriptionHeight - 1; i++)
    {
        SetCursorPosition(2, data->terminalHeight - maxTextLines + i);
        printf(CLR_LINE_END);
        printf("\r" CSR_MOVE_RIGHT(data->terminalWidth - 1));
        printf(SINGLE_VERTICAL_LINE);
    }
    
    bool center = true;
    bool onTop = false;

    if(data->selected >= OPTION_COUNT) {
        ExitAppWithErrorFormat(EXIT_FAILURE, ERRMSG_INVALID_OPTION_INDEX(data->selected));
    }

    if(data->selected < 5) {
        onTop = true;
        center = false;
    }

    const int width = data->terminalWidth - 4;
    const char* text = Descriptions[data->selected];
    int lineCount = GetWrappedLineCount(text, width);

    double centerOffset = (double)(maxTextLines - lineCount) / 2.0;
    const int top = data->terminalHeight - maxTextLines;
    if(onTop) {
        centerOffset = 0;
    }

    centerOffset = floor(centerOffset);
    SetCursorPosition(3, top + (int)centerOffset);
    PrintWrappedLine(text, width, 2, center);
}

static void DrawUI_SettingDescription(SettingsPageData* data) {
    if(data->terminalHeight <= data->lineIndexes[OPTION_COUNT - 1] + data->descriptionHeight) {
        return;
    }

    SetCursorPosition(0, data->terminalHeight - data->descriptionHeight);
    PRINT_SINGLE_TJUNCTION_BORDER(data->terminalWidth);

    DrawUI_UpdateSettingDescription(data);
}

static void DrawUI_Settings(SettingsPageData* data) {
    ResetColor();
    ClearScreen();

    PRINT_SINGLE_TOP_BORDER(data->terminalWidth);

    printf("  Ustawienia");
    printf("\n  [ ] Kolor poprawnej odpowiedzi     ");
    GetCursorPosition(&data->colorsX, &data->colorsY);

    data->slimMode = (data->terminalWidth - data->colorsX) < COLOR_PALET_WIDTH;

    DrawUI_PrintColors(data, LoadedSettings.CorrectAnswerColor);
    printf("\n  [ ] Kolor błędnej odpowiedzi       ");
    DrawUI_PrintColors(data, LoadedSettings.WrongAnswerColor);
    printf("\n  [ ] Kolor zaznaczonej odpowiedzi   ");
    DrawUI_PrintColors(data, LoadedSettings.SelectedAnswerColor);
    printf("\n  [ ] Kolor potwierdzonej odpowiedzi ");
    DrawUI_PrintColors(data, LoadedSettings.ConfirmedAnswerColor);
    printf("\n  [ ] Kolor wsparcia                 ");
    DrawUI_PrintColors(data, LoadedSettings.SupportColor);

    printf("\n  [ ] ");
    PrintWrappedLine("Pełne wsparcie UTF-8 | Jeśli ten znak [▶] (trójkąt) nie jest poprawnie wyświetlany, wyłącz tę opcję | Włączone: ___",
        data->terminalWidth - 8, 6, false);
    GetCursorPosition(&data->utf8SupportX, &data->utf8SupportY);
    data->utf8SupportX -= 3;
    DrawUI_SettingValue(data, 5);

    printf("\n  [ ] ");
    PrintWrappedLine("Pokaż poprawną odpowiedź po przegranej: ", data->terminalWidth - 8, 6, false);
    GetCursorPosition(&data->showCorrectWhenWrongX, &data->showCorrectWhenWrongY);
    DrawUI_SettingValue(data, 6);

    printf("\n  [ ] Włącz wsparcie myszki: ");
    GetCursorPosition(&data->enableMouseSupportX, &data->enableMouseSupportY);
    DrawUI_SettingValue(data, 7);

    printf("\n  [ ] Włącz tryb ciemny: ");
    GetCursorPosition(&data->darkModeX, &data->darkModeY);
    DrawUI_SettingValue(data, 8);

    printf("\n");

    printf("\n  [ ] Zapisz i wróć do menu głównego");
    printf("\n  [ ] Wróć do menu głównego bez zapisywania");

    data->lineIndexes[0] = 3;
    data->lineIndexes[1] = 4 + (data->slimMode ? 1 : 0);
    data->lineIndexes[2] = 5 + (data->slimMode ? 2 : 0);
    data->lineIndexes[3] = 6 + (data->slimMode ? 3 : 0);
    data->lineIndexes[4] = 7 + (data->slimMode ? 4 : 0);
    data->lineIndexes[5] = 8 + (data->slimMode ? 5 : 0);

    data->lineIndexes[6] = data->showCorrectWhenWrongY;
    data->lineIndexes[7] = data->enableMouseSupportY;
    data->lineIndexes[8] = data->darkModeY;
    data->lineIndexes[9] = data->darkModeY + 2;
    data->lineIndexes[10] = data->darkModeY + 3;

    SetCursorPosition(4, data->lineIndexes[data->selected]);
    printf("*");

    SetCursorPosition(0, data->terminalHeight);
    PRINT_SINGLE_BOTTOM_BORDER(data->terminalWidth);

    for (int i = 2; i < data->terminalHeight; i++)
    {
        SetCursorPosition(data->colorsX, i);
        PrintGenericBorderEdges(0, data->terminalWidth, i, SINGLE_VERTICAL_LINE, false);
    }

    UpdateDescriptionHeight(data);
    
    DrawUI_SettingDescription(data);
}

static void HandleArrowUpDownKeys(SettingsPageData* data, int direction) {
    SetCursorPosition(4, data->lineIndexes[data->selected]);
    printf(" ");

    data->selected += direction;
    if (data->selected < 0) data->selected = OPTION_COUNT - 1;
    if (data->selected >= OPTION_COUNT) data->selected = 0;

    SetCursorPosition(4, data->lineIndexes[data->selected]);
    printf("*");

    DrawUI_SettingDescription(data);
}

static bool HandleEnterKey(SettingsPageData* data) {
    switch(data->selected) {
        case 5:
            LoadedSettings.FullUTF8Support = !LoadedSettings.FullUTF8Support;
            break;
        case 6:
            LoadedSettings.ShowCorrectWhenWrong = !LoadedSettings.ShowCorrectWhenWrong;
            break;
        case 7:
            LoadedSettings.EnableMouseSupport = !LoadedSettings.EnableMouseSupport;

            if(!LoadedSettings.EnableMouseSupport) {
                EnableMouseInput(false);
            }

            break;
        case 8:
            LoadedSettings.DarkMode = !LoadedSettings.DarkMode;

            DrawUI_Settings(data);
            break;

        case OPTION_COUNT - 2:
            SaveSettingsToFile();
            return true;

        case OPTION_COUNT - 1:
            LoadSettingsFromFile(); // Reload settings
            return true;

        default: // Option does not support enter key
            return false;
    }

    DrawUI_SettingValue(data, data->selected);

    return false;
}

static void HandleArrowLeftRightKeys(SettingsPageData* data, int direction) {
    if(data->selected > 4) return;

    int* option = GetOptionColor(data->selected);
    *option = *option + direction;

    if(*option < 0)
        *option = RGB_COLOR_COUNT - 1;
    else if(*option >= RGB_COLOR_COUNT)
        *option = 0;

    if(data->selected == 0 || data->selected == 1) {
        for (int i = 5; i < 9; i++)
        {
            DrawUI_SettingValue(data, i); // Refresh for Yes/No colors
        }
    }

    SetCursorPosition(data->colorsX, data->lineIndexes[data->selected]);
    DrawUI_PrintColors(data, *option);
}

extern int LatestTerminalWidth, LatestTerminalHeight;

static void OnResize(void* data) {
    SettingsPageData* pageData = (SettingsPageData*)data;
    pageData->terminalWidth = LatestTerminalWidth;
    pageData->terminalHeight = LatestTerminalHeight;

    DrawUI_Settings(pageData);
}

void PageEnter_Settings()
{
    HideCursor();

    SettingsPageData data;
    data.terminalWidth = LatestTerminalWidth;
    data.terminalHeight = LatestTerminalHeight;
    data.selected = 0;
    data.descriptionHeight = 0;

    DrawUI_Settings(&data);

    SetResizeHandler(OnResize, &data);

    bool continueLoop = true;
    while (continueLoop)
    {
        KeyInputType key = HandleInteractions(true);
        switch (key)
        {
            case KEY_ARROW_UP:
                HandleArrowUpDownKeys(&data, -1);
                break;

            case KEY_ARROW_DOWN:
                HandleArrowUpDownKeys(&data, 1);
                break;

            case KEY_ENTER:
                if(HandleEnterKey(&data)) {
                    continueLoop = false; // Exit page
                }
                break;

            case KEY_ARROW_LEFT:
                HandleArrowLeftRightKeys(&data, -1);
                break;

            case KEY_ARROW_RIGHT:
                HandleArrowLeftRightKeys(&data, 1);
                break;

            case KEY_ESCAPE:
                LoadSettingsFromFile(); // Reload settings
                continueLoop = false; // Exit page
                break;

            default:
                break;
        }
    }

    UnsetResizeHandler();
}
