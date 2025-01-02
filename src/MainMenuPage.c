#include "MainMenuPage.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "AnsiHelper.h"
#include "PageUtils.h"

#include "QuizPage.h"
#include "SettingsPage.h"
// #include "QuestionListPage.h"
#include "AddQuestionPage.h"
#include "QuizQuestionPage.h"
#include "DebugPage.h"

void PrintMainMenu(int selected)
{
    ResetColor();
    ClearScreen();
    printf("Witaj w Programisterach!\n");
    printf("[ ] Rozpocznij quiz\n");
    // printf("[ ] Zarządzaj pytaniami\n");
    printf("[ ] Dodaj pytanie\n");
    printf("[ ] Tablica wyników\n");
    printf("[ ] Ustawienia\n");
    printf("[ ] Wyjdź\n");
    printf("[ ] DEBUG\n");
    printf("[ ] Podgląd pytań (Zawiera zaznaczone odpowiedzi!!)\n");

    SetCursorPosition(2, 2 + selected);
    printf("*");
}

void PageEnter_MainMenu()
{
    HideCursor();

    const int optionCount = 7;
    int selected = 0;
    PrintMainMenu(selected);

    while (true)
    {
        KeyInputType key = HandleInteractions(true);
        if (key == KEY_ARROW_UP)
        {
            SetCursorPosition(2, 2 + selected);
            printf(" ");

            selected--;
            if (selected < 0) selected = optionCount - 1;
            
            SetCursorPosition(2, 2 + selected);
            printf("*");
        }
        else if (key == KEY_ARROW_DOWN)
        {
            SetCursorPosition(2, 2 + selected);
            printf(" ");

            selected++;
            if (selected >= optionCount) selected = 0;
            
            SetCursorPosition(2, 2 + selected);
            printf("*");
        }
        else if (key == KEY_ENTER)
        {
            switch (selected)
            {
                case 0:
                    PageEnter_Quiz();
                    break;
                case 1:
                    // PageEnter_QuestionList();
                    PageEnter_AddQuestion();
                    break;
                case 2:
                    break;
                case 3:
                    PageEnter_Settings();
                    break;
                case 4:
                    exit(EXIT_SUCCESS);
                case 5:
                    PageEnter_Debug();
                    break;
                case 6:
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
                    
                    break;
            }

            PrintMainMenu(selected);
        }
    }
}
