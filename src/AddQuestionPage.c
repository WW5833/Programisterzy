#include "AddQuestionPage.h"

#include "PageUtils.h"
#include "AnsiHelper.h"
#include <string.h>
#include <ctype.h>
#include "IOHelper.h"
#include "TextHelper.h"
#include "Popup.h"

#define MAX_QUESTION_LENGTH 256

typedef struct {
    int terminalWidth;
    int terminalHeight;

    Question* question;
    int slotNumber;

    char buffer[MAX_QUESTION_LENGTH];
    int cursorPosition;
} AddQuestionPageData;

static void DrawUI(AddQuestionPageData* data);

static void CopyBuffer(char** dest, const char* src, int length) {
    *dest = malloc((size_t)(length + 1));
    for (int i = 0; i < length; i++) {
        (*dest)[i] = src[i];
    }

    (*dest)[length] = '\0';
}

static int ReadText(AddQuestionPageData* data, int maxLength) {
    int c;
    do {
        c = getch();
        if(c == '\b') {
            if(data->cursorPosition == 0) continue;

            data->cursorPosition -= GetCurrentCharSize(data->buffer, data->buffer + data->cursorPosition - 1);
            printf("\b \b");
            continue;
        }

        if(c == '\n' || c == '\r') {
            data->buffer[data->cursorPosition] = '\0';
            printf("\n");
            return data->cursorPosition;
        }

        if(c == ESCAPE_CHAR) {
            getch();
            continue; // Ignore arrow keys
        }

        if(c == ESC) {
            data->buffer[data->cursorPosition] = '\0';
            return -1;
        }

        if(c == ';') { // Disallow semicolon
            continue;
        }

        if(!(isalnum(c) || ispunct(c) || isspace(c) || (c & 0x80) /* UTF-8 */)) {
            continue;
        }

        data->buffer[data->cursorPosition] = (char)c;
        printf("%c", c);
        data->cursorPosition++;
    } while(data->cursorPosition < maxLength - 1);

    data->buffer[data->cursorPosition] = '\0';
    return data->cursorPosition;
}

static bool LoadText(AddQuestionPageData* data, char** output, int* outputLength) {
    *outputLength = -1;
    while(*outputLength == -1) {
        *outputLength = ReadText(data, MAX_QUESTION_LENGTH);
        if(*outputLength == -1) {
            HideCursor();
            if(ShowConfirmationPopup("Czy na pewno chcesz anulować dodawanie pytania?", "Tak", "Nie", 60)) {
                return false;
            }
            ShowCursor();
            DrawUI(data);
        }

        if(*outputLength == 0) {
            ShowAlertPopup("To pole jest wymagane !!", 30);
            *outputLength = -1;
            DrawUI(data);
        }
    }

    CopyBuffer(output, data->buffer, *outputLength);
    data->cursorPosition = 0;
    return true;
}

static void DrawUI_Inputed(AddQuestionPageData* data) {
    if(data->slotNumber == 0) return;
    printf("Podaj treść pytania: %s\n", data->question->Content);
    for (int j = 0; j < 4; j++)
    {
        if(data->slotNumber == j+1) return;
        if(j == 0) {
            printf("Podaj poprawną odpowiedź: %s\n", data->question->Answer[j]);
        }
        else {
            printf("Podaj odpowiedź %c: %s\n", 'A' + j, data->question->Answer[j]);
        }
    }
}

static void DrawUI_Current(AddQuestionPageData* data) {
    if(data->slotNumber == 0) {
        printf("Podaj treść pytania: %.*s", data->cursorPosition, data->buffer);
    } else if(data->slotNumber == 1) {
        printf("Podaj poprawną odpowiedź: %.*s", data->cursorPosition, data->buffer);
    } else {
        printf("Podaj odpowiedź %c: %.*s", 'A' + (data->slotNumber - 1), data->cursorPosition, data->buffer);
    }
}

static void DrawUI(AddQuestionPageData* data) {
    ClearScreen();

    printf("Dodaj pytanie:\n");

    DrawUI_Inputed(data);

    DrawUI_Current(data);
}

bool InputLoop(AddQuestionPageData* data) {
    while (data->slotNumber < 5)
    {
        DrawUI(data);

        if(data->slotNumber == 0) {
            if(!LoadText(data, &data->question->Content, &data->question->ContentLength))
            {
                return false;
            }
        } else {

            if(!LoadText(data, &data->question->Answer[data->slotNumber - 1], &data->question->AnswerLength[data->slotNumber - 1]))
            {
                return false;
            }
        }

        data->slotNumber++;
    }

    return true;
}

void AddQuestion(AddQuestionPageData* data) {
    FILE *file = fopen(QUESTIONS_FILE, "a");
    if (file == NULL)
    {
        perror("Failed to open file");
        ExitApp(EXIT_FAILURE);
    }

    AppendQuestion(file, data->question);

    if(fclose(file)) {
        perror("Failed to close file");
        ExitApp(EXIT_FAILURE);
    }

    QuestionListHeader *list = GetQuestionList();

    ListAdd(list, data->question);
}

extern int LatestTerminalWidth, LatestTerminalHeight;

Question* PageEnter_AddQuestion()
{
    ClearScreen();

    AddQuestionPageData data;
    data.terminalWidth = LatestTerminalWidth;
    data.terminalHeight = LatestTerminalHeight;
    data.slotNumber = 0;
    data.cursorPosition = 0;
    data.question = malloc(sizeof(Question));
    data.question->Id = GetMaxQuestionId() + 1;

    ShowCursor();

    if(!InputLoop(&data)) {
        free(data.question);
        return NULL;
    }

    HideCursor();

    AddQuestion(&data);

    printf("\nPytanie dodane pomyślnie\nWciśnij enter aby kontynuować...");
    WaitForEnter();

    return data.question;
}
