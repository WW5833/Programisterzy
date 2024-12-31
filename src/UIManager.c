#include "UIManager.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include "QuizManager.h"
#include "AnsiHelper.h"
#include "PageUtils.h"
#include "Settings.h"
#include "SettingsPage.h"
#include "Utf8Symbols.h"
#include "QuizQuestionPage.h"

extern Settings* LoadedSettings;

void PrintMainMenu()
{
    ResetColor();
    ClearScreen();
    printf("Witaj w Programisterach!\n");
    printf("1. Rozpocznij quiz\n");
    printf("2. Dodaj pytanie\n");
    printf("3. Tablica wyników\n");
    printf("4. Ustawienia\n");
    printf("Q. Wyjdź\n");
}

void UILoop_MainMenu();
void UILoop_Quiz();

void UILoop_MainMenu()
{
    char choice = '\0';
    while (choice != 'q')
    {
        PrintMainMenu();
        choice = (char)_getch();
        switch (choice)
        {
            case '1':
                UILoop_Quiz();
                break;
            case '2':
                break;
            case '3':
                break;
            case '4':
                EnterSettings();
                break;
            case 'Q':
            case 'q':
            case CTRL_C:
                exit(EXIT_SUCCESS);
            default:
                break;
        }
    }
}

void ReadUsername(char username[30]) {
    char c;
    for (int i = 0; i < 30; i++)
    {
        c = (char)_getch();
        if(c == '\0') break;
        if(c == CTRL_C) {
            ExitOnCtrlC();
        }
        if(c == 13) { // Enter
            username[i] = '\0';
            printf("\n");
            break;
        }

        if(c == ' ') {
            username[i] = '_';
            printf("_");
            continue;
        }

        if(c == '\b') { // Backspace
            if(i == 0) continue;
            i -= 2;
            printf(CSR_MOVE_LEFT(1));
            printf(" " );
            printf(CSR_MOVE_LEFT(1));
            continue;
        }

        if(c >= 'a' && c <= 'z') c -= 32;
        username[i] = c;
        printf("%c", c);
    }
}

void UILoop_Quiz()
{
    char username[30];

    printf("Podaj swoje imie: ");
    ReadUsername(username);

    QuizData* quiz = GenerateQuiz(username);
    if(quiz == NULL) return;

    bool correct = false;
    for(int i = 0; i < 10; i++) {
        Question* q = ListGetAt(quiz->questions, i);
        if(!EnterQuizQuestionPage(q, i + 1, &quiz->abulitiesUsed[0], &correct, &quiz->answers[i])) {
            printf("Przegrałeś :/\n");
            WaitForEnter();
            break;
        }
    }

    DestroyQuiz(quiz);

    UILoop_MainMenu();
}
