#include "QuizPage.h"
#include <stdbool.h>
#include "QuizManager.h"
#include <stdio.h>
#include <stdlib.h>
#include "PageUtils.h"
#include "QuizQuestionPage.h"
#include "AnsiHelper.h"
#include "Settings.h"
#include <conio.h>

extern Settings* LoadedSettings;

void ReadUsername(char username[30]) {
    char c;
    for (int i = 0; i < 30; i++)
    {
        c = (char)_getch();
        if(c == '\0') break;
        if(c == CTRL_C) {
            ExitOnCtrlC();
        }
        if(c == ENTER) { // Enter
            username[i] = '\0';
            printf("\n");
            break;
        }

        if(c == ' ' || c == '_') {
            username[i] = '_';
            printf("_");
            continue;
        }

        if(c == '\b') { // Backspace
            if(i == 0) continue;
            i -= 2;
            printf(CSR_MOVE_LEFT(1));
            printf(" ");
            printf(CSR_MOVE_LEFT(1));
            continue;
        }

        if(c >= 'a' && c <= 'z') c -= 32;
        username[i] = c;
        printf("%c", c);
    }
}

void PageEnter_Quiz()
{
    ClearScreen();

    ShowCursor();

    char username[30];
    printf("Podaj swoje imie: ");
    ReadUsername(username);

    HideCursor();

    QuizData* quiz = GenerateQuiz(username);
    if(quiz == NULL) return;

    bool correct = false;
    for(int i = 0; i < 10; i++) {
        Question* q = ListGetAt(quiz->questions, i);
        PageEnter_QuizQuestion(q, i + 1, &quiz->abulitiesUsed[0], &correct);
        if(!correct) break;
        quiz->currentQuestion = i;
    }

    if(correct) {
        SetColor(LoadedSettings->CorrectAnswerColor);
        printf("Wygrałeś! Gratulacje!\n");
    }
    else {
        SetColor(LoadedSettings->WrongAnswerColor);
        printf("Niestety, nie udało Ci się wygrać.\n");
    }

    DestroyQuiz(quiz);

    ResetColor();

    WaitForEnter();
}