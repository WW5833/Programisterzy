#include "UIManager.h"

#include <stdlib.h>
#include <stdio.h>
#include "QuizManager.h"

void PrintMainMenu()
{
    printf("1. Rozpocznij quiz\n");
    printf("2. Dodaj pytanie\n");
    printf("3. Tablica wynikow\n");
    printf("0. Wyjdz\n");
}

void PrintQuestion(Question* question, int number, int offset)
{
    printf("Pytanie %d: %s\n", number, question->Content);
    printf("A: %s\n", question->Answer[(0 + offset) % 4]);
    printf("B: %s\n", question->Answer[(1 + offset) % 4]);
    printf("C: %s\n", question->Answer[(2 + offset) % 4]);
    printf("D: %s\n", question->Answer[(3 + offset) % 4]);
}

void UILoop_MainMenu();
void UILoop_Quiz();
bool UILoop_QuizQuestion(Question* question, int number, bool* abilities, bool* outCorrect, char* outAnswer);

void UILoop_MainMenu()
{
    int choice = -1;
    while (choice != 0)
    {
        PrintMainMenu();
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            UILoop_Quiz();
            break;
        case 2:
            break;
        case 3:
            break;
        case 0:
            exit(EXIT_SUCCESS);
        default:
            break;
        }
    }
}

void UILoop_Quiz()
{
    char username[30];

    printf("Podaj swoje imie: ");
    scanf("%30s", username);
    getchar();

    QuizData* quiz = GenerateQuiz(username);
    if(quiz == NULL) return;

    for(int i = 0; i < 10; i++) {
        Question* q = ListGetAt(quiz->questions, i);
        // PrintQuestion(q, i + 1, 0);
        // getchar();
        if(!UILoop_QuizQuestion(q, i + 1, &quiz->abulitiesUsed[0], &quiz->asnwersCorrect[i], &quiz->answers[i])) {
            printf("Przegrales :/\n");
            getchar();
            break;
        }
    }

    DestroyQuiz(quiz);

    UILoop_MainMenu();
}

bool UILoop_QuizQuestion(Question* question, int number, bool* abilities, bool* outCorrect, char* outAnswer) { 
    int offset = rand() % 4;

    printf("[DEBUG] Offset: %d\n", offset);

    PrintQuestion(question, number, offset);

    if(!abilities[0]) printf("X: Help1\n");
    if(!abilities[1]) printf("Y: Help2\n");
    if(!abilities[2]) printf("Z: Help3\n");

    
    while (true)
    {
        char answer;
        scanf(" %c", &answer);

        if(answer == 'X' && !abilities[0]) {
            printf("Help1: %s\n", question->Help);
            abilities[0] = true;
            getchar();
            continue;
        }

        if(answer == 'Y' && !abilities[1]) {
            printf("Help2: %s\n", question->Help);
            abilities[1] = true;
            getchar();
            continue;
        }

        if(answer == 'Z' && !abilities[2]) {
            printf("Help3: %s\n", question->Help);
            abilities[2] = true;
            getchar();
            continue;
        }

        if(answer != 'A' && answer != 'B' && answer != 'C' && answer != 'D') {
            getchar();
            continue;
        }

        int answerIndex = answer - 'A';
        answerIndex += offset;
        answerIndex = (answerIndex + 4) % 4;

        *outAnswer = (char)answerIndex;

        *outCorrect = answerIndex == 0;

        if(*outCorrect) {
            printf("Poprawna odpowiedz\n");
        } else {
            printf("Zla odpowiedz\n");
        }

        getchar();
        return *outCorrect;
    }
}
