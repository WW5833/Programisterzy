#include "MainMenuPage.h"

#include <stdlib.h>
#include <stdio.h>

#include "AnsiHelper.h"
#include "PageUtils.h"

#include "QuizPage.h"
#include "SettingsPage.h"
#include "AddQuestionPage.h"
#include "QuizQuestionPage.h"

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

void PageEnter_QuizPreview() {
    ClearScreen();
    printf("Podgląd pytań\n");
    printf("Podaj numer pytania do podglądu: ");
    int id;
    if(scanf("%d", &id) != 1) {
        id = -1;
    }

    QuestionListHeader *list = GetQuestionList();
    QuestionListItem* current = list->head;
    while (current != NULL)
    {
        if(id != -1 && current->data->Id != id) {
            current = current->next;
            continue;
        }
        
        PageEnter_QuizQuestionPreview(current->data);
        current = current->next;
    }
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
            exit(EXIT_SUCCESS);
#ifdef PROGRAMISTERZY_DEBUG
        case 4:
            PageEnter_Debug();
            break;
        case 5:
            PageEnter_QuizPreview();  
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
