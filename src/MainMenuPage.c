#include "MainMenuPage.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "AnsiHelper.h"
#include "PageUtils.h"

#include "QuizPage.h"
#include "SettingsPage.h"
#include "QuestionListPage.h"

void PrintMainMenu(int selected)
{
    ResetColor();
    ClearScreen();
    printf("Witaj w Programisterach!\n");
    printf("[ ] Rozpocznij quiz\n");
    printf("[ ] Zarządzaj pytaniami\n");
    printf("[ ] Tablica wyników\n");
    printf("[ ] Ustawienia\n");
    printf("[ ] Wyjdź\n");

    SetCursorPosition(2, 2 + selected);
    printf("*");
}

void PageEnter_MainMenu()
{
    HideCursor();

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
            if (selected < 0) selected = 4;
            
            SetCursorPosition(2, 2 + selected);
            printf("*");
        }
        else if (key == KEY_ARROW_DOWN)
        {
            SetCursorPosition(2, 2 + selected);
            printf(" ");

            selected++;
            if (selected > 4) selected = 0;
            
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
                    PageEnter_QuestionList();
                    break;
                case 2:
                    break;
                case 3:
                    PageEnter_Settings();
                    break;
                case 4:
                    exit(EXIT_SUCCESS);
            }

            PrintMainMenu(selected);
        }
    }
}
