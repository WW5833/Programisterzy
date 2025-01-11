#include "MainMenuPage.h"

#include <stdlib.h>
#include <stdio.h>

#include "AnsiHelper.h"
#include "PageUtils.h"
#include "IOHelper.h"

#include "QuizPage.h"
#include "AddQuestionPage.h"
#include "SettingsPage.h"
#include "InstructionPage.h"
#include "QuizQuestionPage.h"
#include "QuestionListPage.h"
#include "IOHelper.h"

#include "Utf8Symbols.h"

#include "DebugCheck.h"

#define OPTION_COUNT 6

#ifdef PROGRAMISTERZY_DEBUG
    #define DEBUG_OPTION_COUNT 2
    #include "DebugPage.h"
    #include "WelcomePage.h"
    #define TOTAL_OPTION_COUNT (OPTION_COUNT+DEBUG_OPTION_COUNT)
#else
    #define TOTAL_OPTION_COUNT (OPTION_COUNT)
#endif

typedef struct
{
    int terminalWidth;
    int terminalHeight;

    int selected;
} MainMenuPageData;

static const int LineTable[] = {4, 5, 6, 7, 8, 10, 11, 12};

void PrintMainMenu(MainMenuPageData* data)
{
    ResetColor();
    ClearScreen();

    int height = TOTAL_OPTION_COUNT + 3;
    for (int i = 0; i < height; i++)
    {
        PrintGenericBorderEdges(0, data->terminalWidth, i + 2, SINGLE_VERTICAL_LINE, false);
    }
    ResetCursor();

    PRINT_SINGLE_TOP_BORDER(data->terminalWidth);
    printf(CSR_MOVE_RIGHT(2));
    printf("Witaj w Programisterach!\n");
    
    PRINT_SINGLE_TJUNCTION_BORDER(data->terminalWidth);

    printf(CSR_MOVE_RIGHT(2));
    printf("[ ] Rozpocznij quiz\n" CSR_MOVE_RIGHT(2));
    printf("[ ] Dodaj pytanie\n" CSR_MOVE_RIGHT(2));
    printf("[ ] Ustawienia\n" CSR_MOVE_RIGHT(2));
    printf("[ ] Instrukcja Obsługi\n" CSR_MOVE_RIGHT(2));
    printf("[ ] Podgląd pytań\n" CSR_MOVE_RIGHT(2));
    printf("    (Zawiera zaznaczone odpowiedzi!!)\n" CSR_MOVE_RIGHT(2));
    printf("[ ] Wyjdź\n" CSR_MOVE_RIGHT(2));
#ifdef PROGRAMISTERZY_DEBUG
    printf("[ ] DEBUG\n" CSR_MOVE_RIGHT(2));
    printf("[ ] Strona powitalna\n"  CSR_MOVE_RIGHT(2));
#endif

    printf("\r");
    
    PRINT_SINGLE_BOTTOM_BORDER(data->terminalWidth);

    SetCursorPosition(4, LineTable[data->selected]);
    printf("*");
}

void OnArrowKeysPressed(MainMenuPageData* data, bool down) {
    SetCursorPosition(4, LineTable[data->selected]);
    printf(" ");

    if(down) {
        data->selected = (data->selected + 1) % TOTAL_OPTION_COUNT;
    } else {
        data->selected = (data->selected - 1 + TOTAL_OPTION_COUNT) % TOTAL_OPTION_COUNT;
    }
    
    SetCursorPosition(4, LineTable[data->selected]);
    printf("*");
}

void OnEnterPressed(MainMenuPageData* data) {
    switch (data->selected)
    {
        case 0:
            PageEnter_Quiz();
            break;
        case 1:
            PageEnter_AddQuestion();
            break;
        case 2:
            PageEnter_Settings();
            break;
        case 3:
            PageEnter_Instruction();
            break;
        case 4:
            PageEnter_QuestionList();  
            break;
        case 5:
            ExitApp(EXIT_SUCCESS);
#ifdef PROGRAMISTERZY_DEBUG
        case OPTION_COUNT:
            PageEnter_Debug();
            break;
        case OPTION_COUNT + 1:
            PageEnter_Welcome();  
            break;
#endif
    }

    PrintMainMenu(data);
}

extern int LatestTerminalWidth, LatestTerminalHeight;
void OnMainMenuPageReset(int width, int height, void* data);

void PageEnter_MainMenu()
{
    HideCursor();

    MainMenuPageData data;
    data.terminalWidth = LatestTerminalWidth;
    data.terminalHeight = LatestTerminalHeight;
    data.selected = 0;

    PrintMainMenu(&data);

    SetResizeHandler(OnMainMenuPageReset, &data);

    while (true)
    {
        KeyInputType key = HandleInteractions(true);
        if(key == KEY_ARROW_UP || key == KEY_ARROW_DOWN) {
            OnArrowKeysPressed(&data, key == KEY_ARROW_DOWN);
        }
        else if (key == KEY_ENTER) {
            UnsetResizeHandler();
            
            OnEnterPressed(&data);

            SetResizeHandler(OnMainMenuPageReset, &data);
        }
    }

    UnsetResizeHandler();
}

void OnMainMenuPageReset(int width, int height, void* data) {
    MainMenuPageData* mainMenuData = (MainMenuPageData*)data;
    mainMenuData->terminalWidth = width;
    mainMenuData->terminalHeight = height;
    PrintMainMenu(mainMenuData);
}
