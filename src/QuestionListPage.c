#include "QuestionListPage.h"

#include "PageUtils.h"
#include "AnsiHelper.h"
#include "QuizQuestionPage.h"

#include <string.h>

void DrawStaticText() {
    ClearScreen();
    printf("Lista pytań\n");
}

void DrawStaticScroll(int terminalWidth, int terminalHeight) {
    for (int i = 0; i < terminalHeight - 1; i++)
    {
        SetCursorPosition(terminalWidth - 1, 1 + i);
        if(i == 0) {
            printf("▲");
        }
        else if(i == terminalHeight - 2) {
            printf("▼");
        }
        else {
            printf("│");
        }
    }
}

void DrawVisibleQuestions(QuestionListHeader* list, int elementLimit, int blockWidth, int selected) {
    SetCursorPosition(0, 2);
    int i = 0;

    int startIndex = 0;
    if(selected > elementLimit / 2) {
        startIndex = selected - elementLimit / 2;
    }

    if(startIndex + elementLimit > list->count) {
        startIndex = list->count - elementLimit;
    }

    int widthLimit = blockWidth - 5;

    QuestionListItem* current = list->head;
    while (current != NULL)
    {
        if(i < startIndex) {
            i++;
            current = current->next;
            continue;
        }

        if(i == selected) {
            SetColor(COLOR_FG_WHITE);
            SetColor(COLOR_BG_BLUE);
        }

        int length = GetStringCharCount(current->data->Content);

        printf(CSR_MOVE_CURSOR_X(blockWidth));
        printf(CLR_LINE_START "\r" "%d. ", current->data->Id);

        if(widthLimit < length) {
            const char* currentChar = current->data->Content;
            for (int j = 0; j < widthLimit - 3; j++)
            {
                const char* next = GetNextChar(currentChar);

                printf("%.*s", (int)(next - currentChar), currentChar);

                currentChar = next;
            }
            
            printf("...\n");
        }
        else {
            printf("%s\n", current->data->Content);
        }

        if(i == selected) {
            ResetColor();
        }

        i++;
        current = current->next;

        if(i >= startIndex + elementLimit) {
            break;
        }
    }
}

void DrawScrollBar(int terminalWidth, int terminalHeight, int oldSelected, int selected, int totalElements) {
    int scrollHeight = terminalHeight - 3;
    float movePerElement = (float)scrollHeight / (float)totalElements;
    int scrollPosition = (int)((float)selected * movePerElement);
    int oldScrollPosition = (int)((float)oldSelected * movePerElement);

    if(scrollPosition == oldScrollPosition) {
        return;
    }

    if(oldScrollPosition >= 0) {
        SetCursorPosition(terminalWidth - 1, 2 + oldScrollPosition);
        printf("│");
    }

    SetCursorPosition(terminalWidth - 1, 2 + scrollPosition);
    printf("█");

    ResetCursor();  
}

void DrawAll(int terminalWidth, int terminalHeight, int selected, int elementLimit, int blockWidth, int elementCount) {
    DrawStaticText();
    DrawVisibleQuestions(GetQuestionList(), elementLimit, blockWidth, selected);

    if(elementCount > elementLimit) {
        DrawStaticScroll(terminalWidth, terminalHeight);
        DrawScrollBar(terminalWidth, terminalHeight, INT_MIN, selected, elementCount);
    }
}

void PageEnter_QuestionList()
{
    HideCursor();

    int terminalWidth, terminalHeight;
    GetTerminalSize(&terminalWidth, &terminalHeight);

    int blockWidth = terminalWidth - 2;
    int elementLimit = terminalHeight - 2;

    int selected = 0;
    int oldSelected;

    QuestionListHeader *list = GetQuestionList();
    int elementCount = list->count;
    DrawAll(terminalWidth, terminalHeight, selected, elementLimit, blockWidth, elementCount);

    KeyInputType key;
    while(true) {
        key = HandleInteractions(true);

        switch (key)
        {
            case KEY_ARROW_UP:
                if(selected == 0) break;

                oldSelected = selected--;
                DrawVisibleQuestions(GetQuestionList(), elementLimit, blockWidth, selected);
                if(elementCount > elementLimit) DrawScrollBar(terminalWidth, terminalHeight, oldSelected, selected, elementCount);
                break;

            case KEY_ARROW_DOWN:
                if(selected >= list->count - 1) break;
                
                oldSelected = selected++;
                DrawVisibleQuestions(GetQuestionList(), elementLimit, blockWidth, selected);
                if(elementCount > elementLimit) DrawScrollBar(terminalWidth, terminalHeight, oldSelected, selected, elementCount);
                break;

            case KEY_ENTER:
                PageEnter_QuizQuestionPreview(ListGetAt(list, selected));
                DrawAll(terminalWidth, terminalHeight, selected, elementLimit, blockWidth, elementCount);
                break;

            case KEY_ESCAPE:
                return;

            default:
                break;
        }
    }
}