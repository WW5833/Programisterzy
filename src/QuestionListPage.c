#include "QuestionListPage.h"

#include "PageUtils.h"
#include "AnsiHelper.h"
#include "QuizQuestionPage.h"
#include "IOHelper.h"

#include <string.h>

typedef struct {
    int terminalWidth, terminalHeight;
    int blockWidth, elementLimit;

    QuestionListHeader* list;
    int selected;

    int drawQuestionStartIndex;
} QuestionListPageData;

void DrawStaticText() {
    ClearScreen();
    printf("Lista pytań\n");
}

void DrawStaticScroll(QuestionListPageData* data) {
    if(data->list->count <= data->elementLimit) return;

    for (int i = 0; i < data->terminalHeight - 1; i++)
    {
        SetCursorPosition(data->terminalWidth - 1, 1 + i);
        if(i == 0) {
            printf("▲");
        }
        else if(i == data->terminalHeight - 2) {
            printf("▼");
        }
        else {
            printf("│");
        }
    }
}

void DrawVisibleQuestions(QuestionListPageData* data) {
    SetCursorPosition(0, 2);
    int i = 0;

    data->drawQuestionStartIndex = 0;
    if(data->selected > data->elementLimit / 2) {
        data->drawQuestionStartIndex = data->selected - data->elementLimit / 2;
    }

    if(data->drawQuestionStartIndex + data->elementLimit > data->list->count) {
        data->drawQuestionStartIndex = data->list->count - data->elementLimit;
    }

    int widthLimit = data->blockWidth - 5;

    QuestionListItem* current = data->list->head;
    while (current != NULL)
    {
        if(i < data->drawQuestionStartIndex) {
            i++;
            current = current->next;
            continue;
        }

        if(i == data->selected) {
            SetColor(COLOR_FG_WHITE);
            SetColor(COLOR_BG_BLUE);
        }

        int length = GetStringCharCount(current->data->Content);

        printf(CSR_MOVE_CURSOR_X(data->blockWidth));
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

        if(i == data->selected) {
            ResetColor();
        }

        i++;
        current = current->next;

        if(i >= data->drawQuestionStartIndex + data->elementLimit) {
            break;
        }
    }
}

void DrawScrollBar(QuestionListPageData* data, int oldSelected) {
    if(data->list->count <= data->elementLimit) return;

    int scrollHeight = data->terminalHeight - 3;
    float movePerElement = (float)scrollHeight / (float)data->list->count;
    int scrollPosition = (int)((float)data->selected * movePerElement);
    int oldScrollPosition = (int)((float)oldSelected * movePerElement);

    if(scrollPosition == oldScrollPosition) {
        return;
    }

    if(oldScrollPosition >= 0) {
        SetCursorPosition(data->terminalWidth - 1, 2 + oldScrollPosition);
        printf("│");
    }

    SetCursorPosition(data->terminalWidth - 1, 2 + scrollPosition);
    printf("█");

    ResetCursor();  
}

void DrawAll(QuestionListPageData* data) {
    DrawStaticText();
    DrawVisibleQuestions(data);

    DrawStaticScroll(data);
    DrawScrollBar(data, INT_MIN);
}

void Scroll(QuestionListPageData* data, bool down) {
    int oldSelected;

    if(down) {
        if(data->selected >= data->list->count - 1) return;
                
        oldSelected = data->selected++;
    }
    else {
        if(data->selected == 0) return;

        oldSelected = data->selected--;
    }

    DrawVisibleQuestions(data);
    DrawScrollBar(data, oldSelected);
}

void OnResize(int width, int height, void* data)
{
    QuestionListPageData* pageData = (QuestionListPageData*)data;
    pageData->terminalWidth = width;
    pageData->terminalHeight = height;
    pageData->blockWidth = width - 2;
    pageData->elementLimit = height - 2;

    DrawAll(pageData);
}

void OnScroll(bool down, int mouseX, int mouseY, void* data) {
    Scroll((QuestionListPageData*)data, down);
}

void OnMouseClick(int button, int mouseX, int mouseY, void* data) {
    if((button & MOUSE_LEFT_BUTTON) == 0) return;
    QuestionListPageData* pageData = (QuestionListPageData*)data;

    int oldSelected = pageData->selected;
    if(mouseX >= pageData->terminalWidth - 3) {
        int boxLine = mouseY - 1;
        float lineWorth = (float)pageData->list->count / (float)(pageData->elementLimit - 2);
        pageData->selected = boxLine * (int)lineWorth;

        if(pageData->selected >= pageData->list->count) {
            pageData->selected = pageData->list->count - 1;
        }
        else if(pageData->selected < 0) {
            pageData->selected = 0;
        }
    }
    else {
        int index = mouseY - 1 + pageData->drawQuestionStartIndex;
        if(index < 0 || index >= pageData->elementLimit + pageData->drawQuestionStartIndex) return;

        pageData->selected = index;
    }

    DrawVisibleQuestions(pageData);
    DrawScrollBar(pageData, oldSelected);
}

void PageEnter_QuestionList()
{
    HideCursor();

    QuestionListPageData* data = malloc(sizeof(QuestionListPageData));

    GetTerminalSize(&data->terminalWidth, &data->terminalHeight);

    data->drawQuestionStartIndex = 0;

    data->blockWidth = data->terminalWidth - 2;
    data->elementLimit = data->terminalHeight - 2;

    data->selected = 0;

    data->list = GetQuestionList();
    DrawAll(data);

    SetResizeHandler(OnResize, data);
    SetMouseHandler(OnMouseClick, NULL, OnScroll, NULL, data);
    EnableMouseInput(true);

    KeyInputType key;
    while(true) {
        key = HandleInteractions(true);

        switch (key)
        {
            case KEY_ARROW_UP:
                Scroll(data, false);
                break;

            case KEY_ARROW_DOWN:
                Scroll(data, true);
                break;

            case KEY_ENTER:
                PageEnter_QuizQuestionPreview(ListGetAt(data->list, data->selected));
                DrawAll(data);
                break;

            case KEY_ESCAPE:
                UnsetMouseHandler();
                EnableMouseInput(false);
                return;

            default:
                break;
        }
    }
}