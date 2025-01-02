#include "QuestionEditPage.h"

#include "AnsiHelper.h"
#include "PageUtils.h"
#include <conio.h>
#include <ctype.h>

#define BUFFER_SIZE 1024
#define LEFT_OFFSET 26

void PrintStaticText(Question* question, const int terminalWidth, int* lineIndexes, int selected)
{
    int cursorX, cursorY;

    ClearScreen();
    printf("Edytuj pytanie");
    printf("\n[ ] Zapisz i wyjdź");
    printf("\n[ ] Wyjdź bez zapisywania");
    printf("\n[ ] Usuń pytanie");

    lineIndexes[0] = 2;
    lineIndexes[1] = 3;
    lineIndexes[2] = 4;

    printf("\n    Id pytania: %d", question->Id);
    printf("\n[ ] Treść pytania      : ");
    GetCursorPosition(&cursorX, &cursorY);
    lineIndexes[3] = cursorY;
    int lineCount = GetWrappedLineCount(question->Content, terminalWidth - LEFT_OFFSET);
    if(lineCount > 1) printf(CSR_MOVE_LEFT_0_DOWN(lineCount - 1));
    
    printf("\n[ ] Odpowiedź Poprawna : ");
    GetCursorPosition(&cursorX, &cursorY);
    lineIndexes[4] = cursorY;
    lineCount = GetWrappedLineCount(question->Answer[0], terminalWidth - LEFT_OFFSET);
    if(lineCount > 1) printf(CSR_MOVE_LEFT_0_DOWN(lineCount - 1));
    
    printf("\n[ ] Odpowiedź B        : ");
    GetCursorPosition(&cursorX, &cursorY);
    lineIndexes[5] = cursorY;
    lineCount = GetWrappedLineCount(question->Answer[1], terminalWidth - LEFT_OFFSET);
    if(lineCount > 1) printf(CSR_MOVE_LEFT_0_DOWN(lineCount - 1));
    
    printf("\n[ ] Odpowiedź C        : ");
    GetCursorPosition(&cursorX, &cursorY);
    lineIndexes[6] = cursorY;
    lineCount = GetWrappedLineCount(question->Answer[2], terminalWidth - LEFT_OFFSET);
    if(lineCount > 1) printf(CSR_MOVE_LEFT_0_DOWN(lineCount - 1));
    
    printf("\n[ ] Odpowiedź D        : ");
    GetCursorPosition(&cursorX, &cursorY);
    lineIndexes[7] = cursorY;
    lineCount = GetWrappedLineCount(question->Answer[3], terminalWidth - LEFT_OFFSET);
    if(lineCount > 1) printf(CSR_MOVE_LEFT_0_DOWN(lineCount - 1));
    
    printf("\n[ ] Telefon przyjaciela: ");
    GetCursorPosition(&cursorX, &cursorY);
    lineIndexes[8] = cursorY;

    if(question->Help != NULL) {
        lineCount = GetWrappedLineCount(question->Help, terminalWidth - LEFT_OFFSET);
        if(lineCount > 1) printf(CSR_MOVE_LEFT_0_DOWN(lineCount - 1));
    }

    GetCursorPosition(&cursorX, &cursorY);
    lineIndexes[9] = cursorY + 1;

    SetCursorPosition(2, lineIndexes[selected]);
    printf("*");
}

void PrintDynamicText(Question* question, const int terminalWidth, const int* lineIndexes, int* contentEndsX, int* contentEndsY, int printOnly)
{
    if(printOnly == -1 || printOnly == 3) {
        SetCursorPosition(26, lineIndexes[3]);
        for(int i = 0; i < (lineIndexes[4] - lineIndexes[3]); i++) {
            printf(CLR_LINE_END CSR_MOVE_LEFT_0_DOWN1);
        }
        SetCursorPosition(26, lineIndexes[3]);
        PrintWrappedLine(question->Content, terminalWidth - LEFT_OFFSET, LEFT_OFFSET, false);
        GetCursorPosition(&contentEndsX[0], &contentEndsY[0]);
    }

    if(printOnly == -1 || printOnly == 4) {
        SetCursorPosition(26, lineIndexes[4]);
        for(int i = 0; i < (lineIndexes[5] - lineIndexes[4]); i++) {
            printf(CLR_LINE_END CSR_MOVE_LEFT_0_DOWN1);
        }
        SetCursorPosition(26, lineIndexes[4]);
        PrintWrappedLine(question->Answer[0], terminalWidth - LEFT_OFFSET, LEFT_OFFSET, false);
        GetCursorPosition(&contentEndsX[1], &contentEndsY[1]);
    }

    if(printOnly == -1 || printOnly == 5) {
        SetCursorPosition(26, lineIndexes[5]);
        for(int i = 0; i < (lineIndexes[6] - lineIndexes[5]); i++) {
            printf(CLR_LINE_END CSR_MOVE_LEFT_0_DOWN1);
        }
        SetCursorPosition(26, lineIndexes[5]);
        PrintWrappedLine(question->Answer[1], terminalWidth - LEFT_OFFSET, LEFT_OFFSET, false);
        GetCursorPosition(&contentEndsX[2], &contentEndsY[2]);
    }

    if(printOnly == -1 || printOnly == 6) {
        SetCursorPosition(26, lineIndexes[6]);
        for(int i = 0; i < (lineIndexes[7] - lineIndexes[6]); i++) {
            printf(CLR_LINE_END CSR_MOVE_LEFT_0_DOWN1);
        }
        SetCursorPosition(26, lineIndexes[6]);
        PrintWrappedLine(question->Answer[2], terminalWidth - LEFT_OFFSET, LEFT_OFFSET, false);
        GetCursorPosition(&contentEndsX[3], &contentEndsY[3]);
    }

    if(printOnly == -1 || printOnly == 7) {
        SetCursorPosition(26, lineIndexes[7]);
        for(int i = 0; i < (lineIndexes[8] - lineIndexes[7]); i++) {
            printf(CLR_LINE_END CSR_MOVE_LEFT_0_DOWN1);
        }
        SetCursorPosition(26, lineIndexes[7]);
        PrintWrappedLine(question->Answer[3], terminalWidth - LEFT_OFFSET, LEFT_OFFSET, false);
        GetCursorPosition(&contentEndsX[4], &contentEndsY[4]);
    }

    if(printOnly == -1 || printOnly == 8) {
        SetCursorPosition(26, lineIndexes[8]);
        for(int i = 0; i < (lineIndexes[9] - lineIndexes[8]); i++) {
            printf(CLR_LINE_END CSR_MOVE_LEFT_0_DOWN1);
        }
        SetCursorPosition(26, lineIndexes[8]);
        if(question->Help != NULL) {
            PrintWrappedLine(question->Help, terminalWidth - LEFT_OFFSET, LEFT_OFFSET, false);
        }
        GetCursorPosition(&contentEndsX[5], &contentEndsY[5]);
    }
}

char** GetBuffer(Question* question, int index, int* length) {
    switch (index)
    {
        case 3:
            *length = question->ContentLength;
            return &question->Content;
        case 4:
            *length = question->AnswerLength[0];
            return &question->Answer[0];
        case 5:
            *length = question->AnswerLength[1];
            return &question->Answer[1];
        case 6:
            *length = question->AnswerLength[2];
            return &question->Answer[2];
        case 7:
            *length = question->AnswerLength[3];
            return &question->Answer[3];
        case 8:
            *length = question->HelpLength;
            return &question->Help;
        default:
            return NULL;
    }
}

void InsertChar(char* buffer, int* length, int* cursor, char c) {
    if(*length >= BUFFER_SIZE - 1) return;

    printf(CSR_MOVE_RIGHT(*length - *cursor));
    for (int i = *length; i > *cursor; i--)
    {
        buffer[i] = buffer[i - 1];
        printf("%c", buffer[i - 1]);
        printf(CSR_MOVE_LEFT(2));
    }

    // printf(CSR_MOVE_LEFT(*length - *cursor));

    printf("%c", c);

    buffer[*cursor] = c;
    (*cursor)++;
    (*length)++;
}

void DeleteChar(char* buffer, int* length, int* cursor) {
    if(*cursor == 0) return;

    printf(CSR_MOVE_LEFT(1));

    for (int i = *cursor - 1; i < *length; i++)
    {
        buffer[i] = buffer[i + 1];
        printf("%c", buffer[i + 1]);
        // printf(CSR_MOVE_LEFT(2));
    }

    printf(" ");
    printf(CSR_MOVE_LEFT(*length - (*cursor - 1)));

    (*cursor)--;
    (*length)--;
}

bool PageEnter_QuestionEdit(Question *question) // Returns whether to save the question
{
    int terminalWidth, terminalHeight;
    GetTerminalSize(&terminalWidth, &terminalHeight);

    ClearScreen();

    int lineIndexes[10];
    int contentEndsX[6];
    int contentEndsY[6];

    int selected = 0;
    const int optionCount = 9;
    bool editMode = false;

    PrintStaticText(question, terminalWidth, lineIndexes, selected);
    PrintDynamicText(question, terminalWidth, lineIndexes, contentEndsX, contentEndsY, -1);

    while (true)
    {
        KeyInputType key = HandleInteractions(false);
        if (key == KEY_ARROW_UP || key == KEY_ARROW_DOWN)
        {
            SetCursorPosition(2, lineIndexes[selected]);
            printf(" ");

            if(key == KEY_ARROW_UP) {
                selected--;
                if (selected < 0) selected = optionCount - 1;
            }
            else {
                selected++;
                if (selected >= optionCount) selected = 0;
            }

            SetCursorPosition(2, lineIndexes[selected]);
            printf("*");
        }
        else if (key == KEY_ENTER)
        {
            if(selected == 0) {
                return true; // Save and exit
            }
            else if(selected == 1) {
                return false; // Exit without saving
            }
            else if(selected == 2) {   
                question->Id = INT_MIN; // Mark as deleted
                return true;
            }
            else {
                editMode = true;
                int cursorXOffset, cursorYOffset;

                SetCursorPosition(2, lineIndexes[selected]);
                printf(UNDERLINE_ON "*" UNDERLINE_OFF);

                char buffer[BUFFER_SIZE];
                int length;

                char** contentPtr = GetBuffer(question, selected, &length);
                char* origianlContent = *contentPtr;

                char* content = buffer;

                if(origianlContent == NULL) {
                    length = 0;
                }
                else {
                    for (int i = 0; i < length; i++)
                    {
                        buffer[i] = origianlContent[i];
                    }

                    buffer[length] = '\0';
                }

                SetCursorPosition(contentEndsX[selected - 3], contentEndsY[selected - 3]);
                ShowCursor();

                int cursor = length;

                int c = _getch();
                while(c != ENTER) {
                    if(c == CTRL_C) {
                        ExitOnCtrlC();
                    }
                    if(c == '\b') { // Backspace
                        if(length == 0) continue;
                        DeleteChar(buffer, &length, &cursor);
                        buffer[length] = '\0';

                        PrintDynamicText(question, terminalWidth, lineIndexes, contentEndsX, contentEndsY, selected);
                        GetWrappedLineCursorPosition(buffer, terminalWidth - LEFT_OFFSET, cursor, &cursorXOffset, &cursorYOffset);
                        SetCursorPosition(LEFT_OFFSET + cursorXOffset, lineIndexes[selected] + cursorYOffset);
                    }
                    else if(c == 224) {
                        c = _getch();
                        switch (c)
                        {
                            case 77: // Right
                                cursor++;

                                if(cursor > length) {
                                    cursor = length;
                                }
                                else {
                                    printf(CSR_MOVE_RIGHT(1));
                                }
                                PrintDynamicText(question, terminalWidth, lineIndexes, contentEndsX, contentEndsY, selected);
                                GetWrappedLineCursorPosition(buffer, terminalWidth - LEFT_OFFSET, cursor, &cursorXOffset, &cursorYOffset);
                                SetCursorPosition(LEFT_OFFSET + cursorXOffset, lineIndexes[selected] + cursorYOffset);
                                break;
                            case 75: // Left
                                cursor--;

                                if(cursor < 0) {
                                    cursor = 0;
                                }
                                else {
                                    printf(CSR_MOVE_LEFT(1));
                                }
                                PrintDynamicText(question, terminalWidth, lineIndexes, contentEndsX, contentEndsY, selected);
                                GetWrappedLineCursorPosition(buffer, terminalWidth - LEFT_OFFSET, cursor, &cursorXOffset, &cursorYOffset);
                                SetCursorPosition(LEFT_OFFSET + cursorXOffset, lineIndexes[selected] + cursorYOffset);
                                break;
                        }
                    }
                    else if(isalnum(c) || ispunct(c) || isspace(c)) {
                        InsertChar(buffer, &length, &cursor, c);
                        buffer[length] = '\0';

                        PrintDynamicText(question, terminalWidth, lineIndexes, contentEndsX, contentEndsY, selected);
                        GetWrappedLineCursorPosition(buffer, terminalWidth - LEFT_OFFSET, cursor, &cursorXOffset, &cursorYOffset);
                        SetCursorPosition(LEFT_OFFSET + cursorXOffset, lineIndexes[selected] + cursorYOffset);
                    }

                    c = _getch();
                }

                if(content != NULL) {
                    free(content);
                    *contentPtr = NULL;
                }

                content = malloc(length + 1);
                if(content == NULL) {
                    perror("Failed to allocate memory for content");
                    exit(EXIT_FAILURE);
                }

                for (int i = 0; i < length; i++)
                {
                    content[i] = buffer[i];
                }

                content[length] = '\0';

                *contentPtr = content;

                HideCursor();
                editMode = false;
                PrintStaticText(question, terminalWidth, lineIndexes, selected);
                PrintDynamicText(question, terminalWidth, lineIndexes, contentEndsX, contentEndsY, -1);
            }
        }
    }

    WaitForEnter();

    return false;
}
