#include "AddQuestionPage.h"

#include "PageUtils.h"
#include "AnsiHelper.h"
#include <string.h>
#include <ctype.h>
#include "IOHelper.h"

#define MAX_QUESTION_LENGTH 256

bool CopyBuffer(char** dest, const char* src, int length) {
    if(length == 0) {
        printf("\nTo pole jest wymagane !!\n");
        return false;
    }

    if(length == MAX_QUESTION_LENGTH - 2) {
        int i = length + 1;
        int read;
        char buffer[MAX_QUESTION_LENGTH];
        do {
            fgets(buffer, MAX_QUESTION_LENGTH, stdin);
            read = (int)strlen(buffer);
            i += read - 1;
        } while(read == MAX_QUESTION_LENGTH - 1);
        printf("\nLimit znaków wynosi %d, podano %d (polskie/specjalne znaki liczą się za więcej niż jeden znak)!\n", MAX_QUESTION_LENGTH - 3, i);
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

int ReadText(char* buffer, int maxLength) {
    int i = 0;
    int c;
    do {
        c = getch();
        if(c == '\b') {
            if(i == 0) continue;

            i -= GetCurrentCharSize(buffer, buffer + i - 1);
            printf("\b \b");
            continue;
        }

        if(c == '\n' || c == '\r') {
            buffer[i] = '\0';
            printf("\n");
            return i;
        }

        if(c == ESCAPE_CHAR) {
            getch();
            continue; // Ignore arrow keys
        }

        if(c == ESC) {
            buffer[0] = '\0';
            return -1;
        }

        if(!(isalnum(c) || ispunct(c) || isspace(c) || (c & 0x80) /* UTF-8 */)) {
            continue;
        }

        buffer[i] = (char)c;
        printf("%c", c);
        i++;
    } while(i != maxLength - 1);

    buffer[i] = '\0';
    return i;
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
    question->ContentLength = ReadText(buffer, MAX_QUESTION_LENGTH);
    if(question->ContentLength == -1) {
        DestroyQuestion(question);
        HideCursor();
        return NULL;
    }
    if(!CopyBuffer(&question->Content, buffer, question->ContentLength))
        goto load_content;

    for (int j = 0; j < 4; j++)
    {
        load_answer:
        if(j == 0)
            printf("Podaj poprawnej Odpowiedzi: ");
        else
            printf("Podaj odpowiedź %c: ", 'A' + j);

        question->AnswerLength[j] = ReadText(buffer, MAX_QUESTION_LENGTH);
        if(question->AnswerLength[j] == -1) {
            DestroyQuestion(question);
            HideCursor();
            return NULL;
        }
        if(!CopyBuffer(&question->Answer[j], buffer, question->AnswerLength[j]))
            goto load_answer;
    }

    HideCursor();

    FILE *file = fopen(QUESTIONS_FILE, "a");
    if (file == NULL)
    {
        perror("Failed to open file");
        ExitApp(EXIT_FAILURE);
    }

    AppendQuestion(file, question);

    if(fclose(file)) {
        perror("Failed to close file");
        ExitApp(EXIT_FAILURE);
    }

    QuestionListHeader *list = GetQuestionList();

    ListAdd(list, question);

    printf("\nPytanie dodane pomyślnie\nWciśnij enter aby kontynuować...");

    WaitForEnter();

    return question;
}
