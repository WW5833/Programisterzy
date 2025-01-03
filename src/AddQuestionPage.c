#include "AddQuestionPage.h"

#include "PageUtils.h"
#include "AnsiHelper.h"
#include <string.h>

#define MAX_QUESTION_LENGTH 255

bool CopyBuffer(char** dest, const char* src, int length) {
    if(length == 0) {
        printf("\nTo pole jest wymagane !!\n");
        return false;
    }

    *dest = malloc((size_t)(length + 1));
    for (int i = 0; i < length; i++) {
        if(src[i] == ';') {
            printf("\nZnaleziono niedozwolony znak: ';' - spróbuj ponownie\n");
            free(*dest);
            *dest = NULL;
            return false;
        }
        (*dest)[i] = src[i];
    }

    (*dest)[length] = '\0';
    return true;
}

Question *PageEnter_AddQuestion()
{
    ClearScreen();

    Question *question = malloc(sizeof(Question));

    question->Id = GetMaxQuestionId() + 1;

    ShowCursor();

    char buffer[MAX_QUESTION_LENGTH];
    printf("Dodaj pytanie:\n");

    load_content:
    printf("Podaj treść pytania: ");
    fgets(buffer, MAX_QUESTION_LENGTH, stdin);
    question->ContentLength = (int)strlen(buffer) - 1;
    if(!CopyBuffer(&question->Content, buffer, question->ContentLength))
        goto load_content;
    
    for (int j = 0; j < 4; j++)
    {
        load_answer:
        if(j == 0)
            printf("Podaj poprawnej Odpowiedzi: ");
        else
            printf("Podaj odpowiedź %c: ", 'A' + j);

        fgets(buffer, MAX_QUESTION_LENGTH, stdin);
        question->AnswerLength[j] = (int)strlen(buffer) - 1;
        if(!CopyBuffer(&question->Answer[j], buffer, question->AnswerLength[j]))
            goto load_answer;
    }

    load_help:
    printf("Podaj treść podpowiedzi (zostaw puste jeśli brak): ");
    fgets(buffer, MAX_QUESTION_LENGTH, stdin);
    question->HelpLength = (int)strlen(buffer) - 1;
    if(question->HelpLength == 0) {
        question->Help = NULL;
    }
    else {
        if(!CopyBuffer(&question->Help, buffer, question->HelpLength))
            goto load_help;
    }

    HideCursor();

    FILE *file = fopen(QUESTIONS_FILE, "a");
    if (file == NULL)
    {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    AppendQuestion(file, question);

    if(fclose(file)) {
        perror("Failed to close file");
        exit(EXIT_FAILURE);
    }

    QuestionListHeader *list = GetQuestionList();

    ListAdd(list, question);

    printf("\nPytanie dodane pomyślnie\nWciśnij enter aby kontynuować...");

    WaitForEnter();

    return question;
}
