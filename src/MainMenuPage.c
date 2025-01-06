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

#include "Utf8Symbols.h"

#include "DebugCheck.h"

#define OPTION_COUNT 5

#ifdef PROGRAMISTERZY_DEBUG
#define DEBUG_OPTION_COUNT 2
#include "DebugPage.h"
#endif


void PrintMainMenu(int selected)
{
    int terminalWidth, terminalHeight;
    GetTerminalSize(&terminalWidth, &terminalHeight);

    ResetColor();
    ClearScreen();

    int height = OPTION_COUNT + 2;
#ifdef PROGRAMISTERZY_DEBUG
    height += DEBUG_OPTION_COUNT;
#endif
    for (int i = 0; i < height; i++)
    {
        PrintGenericBorderEdges(0, terminalWidth, i + 2, SINGLE_VERTICAL_LINE, false);
    }
    ResetCursor();

    PRINT_SINGLE_TOP_BORDER(terminalWidth);
    printf(CSR_MOVE_RIGHT(2));
    printf("Witaj w Programisterach!\n");
    
    PRINT_SINGLE_TJUNCTION_BORDER(terminalWidth);

    printf(CSR_MOVE_RIGHT(2));
    printf("[ ] Rozpocznij quiz\n" CSR_MOVE_RIGHT(2));
    printf("[ ] Dodaj pytanie\n" CSR_MOVE_RIGHT(2));
    printf("[ ] Ustawienia\n" CSR_MOVE_RIGHT(2));
    printf("[ ] Instrukcja Obsługi\n" CSR_MOVE_RIGHT(2));
    printf("[ ] Wyjdź\n" CSR_MOVE_RIGHT(2));
#ifdef PROGRAMISTERZY_DEBUG
    printf("[ ] DEBUG\n" CSR_MOVE_RIGHT(2));
    printf("[ ] Podgląd pytań (Zawiera zaznaczone odpowiedzi!!)\n");
#endif
    
    PRINT_SINGLE_BOTTOM_BORDER(terminalWidth);

    SetCursorPosition(4, 4 + selected);
    printf("*");
}

void OnArrowKeysPressed(int* selected, int optionCount, bool down) {
    SetCursorPosition(4, 4 + *selected);
    printf(" ");

    if(down) {
        *selected = (*selected + 1) % optionCount;
    } else {
        *selected = (*selected - 1 + optionCount) % optionCount;
    }
    
    SetCursorPosition(4, 4 + *selected);
    printf("*");
}

void OnEnterPressed(int selected) {
    switch (selected)
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
            ExitApp(EXIT_SUCCESS);
#ifdef PROGRAMISTERZY_DEBUG
        case 5:
            PageEnter_Debug();
            break;
        case 6:
            PageEnter_QuestionList();  
            break;
#endif
    }

    PrintMainMenu(selected);
}

void PageEnter_MainMenu()
{
    HideCursor();

    int optionCount = OPTION_COUNT;
#ifdef PROGRAMISTERZY_DEBUG
    optionCount += DEBUG_OPTION_COUNT;
#endif

    int selected = 0;
    PrintMainMenu(selected);

    while (true)
    {
        KeyInputType key = HandleInteractions(true);
        if(key == KEY_ARROW_UP || key == KEY_ARROW_DOWN) {
            OnArrowKeysPressed(&selected, optionCount, key == KEY_ARROW_DOWN);
        }
        else if (key == KEY_ENTER) {
            OnEnterPressed(selected);
        }
    }
}
