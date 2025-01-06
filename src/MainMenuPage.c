#include "MainMenuPage.h"

#include <stdlib.h>
#include <stdio.h>

#include "AnsiHelper.h"
#include "PageUtils.h"

#include "QuizPage.h"
#include "AddQuestionPage.h"
#include "SettingsPage.h"
#include "InstructionPage.h"
#include "QuizQuestionPage.h"
#include "QuestionListPage.h"

#include "DebugCheck.h"

#define OPTION_COUNT 5

#ifdef PROGRAMISTERZY_DEBUG
#define DEBUG_OPTION_COUNT 2
#include "DebugPage.h"
#endif


void PrintMainMenu(int selected)
{
    ResetColor();
    ClearScreen();
    printf("Witaj w Programisterach!\n");
    for(int i = 0; i < 55; i++){
        printf("-");
    }
    printf("\n[ ] Rozpocznij quiz\n");
    printf("[ ] Dodaj pytanie\n");
    printf("[ ] Ustawienia\n");
    printf("[ ] Instrukcja Obsługi\n");
    printf("[ ] Wyjdź\n");
#ifdef PROGRAMISTERZY_DEBUG
    printf("[ ] DEBUG\n");
    printf("[ ] Podgląd pytań (Zawiera zaznaczone odpowiedzi!!)\n");
#endif
    
    for(int i = 0; i < 55; i++){
        printf("-");
    }

    SetCursorPosition(2, 3 + selected);
    printf("*");
}

void OnArrowKeysPressed(int* selected, int optionCount, bool down) {
    SetCursorPosition(2, 3 + *selected);
    printf(" ");

    if(down) {
        *selected = (*selected + 1) % optionCount;
    } else {
        *selected = (*selected - 1 + optionCount) % optionCount;
    }
    
    SetCursorPosition(2, 3 + *selected);
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
            exit(EXIT_SUCCESS);
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
