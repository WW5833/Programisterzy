#include "MainMenuPage.h"

#include <stdlib.h>
#include <stdio.h>

#include "AnsiHelper.h"
#include "PageUtils.h"
#include "IOHelper.h"

#include "QuizPage.h"
#include "SettingsPage.h"
#include "AddQuestionPage.h"
#include "QuizQuestionPage.h"
#include "QuestionListPage.h"

#include "DebugCheck.h"

#define OPTION_COUNT 4

#ifdef PROGRAMISTERZY_DEBUG
#define DEBUG_OPTION_COUNT 2
#include "DebugPage.h"
#endif


void PrintMainMenu(int selected)
{
    ResetColor();
    ClearScreen();
    printf("Witaj w Programisterach!\n");
    printf("[ ] Rozpocznij quiz\n");
    printf("[ ] Dodaj pytanie\n");
    printf("[ ] Ustawienia\n");
    printf("[ ] Wyjdź\n");
#ifdef PROGRAMISTERZY_DEBUG
    printf("[ ] DEBUG\n");
    printf("[ ] Podgląd pytań (Zawiera zaznaczone odpowiedzi!!)\n");
#endif

    SetCursorPosition(2, 2 + selected);
    printf("*");
}

void OnArrowKeysPressed(int* selected, int optionCount, bool down) {
    SetCursorPosition(2, 2 + *selected);
    printf(" ");

    if(down) {
        *selected = (*selected + 1) % optionCount;
    } else {
        *selected = (*selected - 1 + optionCount) % optionCount;
    }
    
    SetCursorPosition(2, 2 + *selected);
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
            ExitApp(EXIT_SUCCESS);
#ifdef PROGRAMISTERZY_DEBUG
        case 4:
            PageEnter_Debug();
            break;
        case 5:
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
