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
    int scrollSize;
} QuestionListPageData;

void EnterPreview(QuestionListPageData* data);

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

    bool* updated = malloc(scrollHeight * sizeof(bool));
    for (int i = 0; i < scrollHeight; i++)
    {
        updated[i] = false;
    }
    

    scrollPosition -= data->scrollSize / 2;
    if(scrollPosition < 0) scrollPosition = 0;
    else if(scrollPosition > scrollHeight - data->scrollSize) scrollPosition = scrollHeight - data->scrollSize;

    for (int i = 0; i < data->scrollSize; i++)
    {
        SetCursorPosition(data->terminalWidth - 1, 2 + scrollPosition + i);
        printf("█");
        updated[scrollPosition + i] = true;
    }

    for (int i = 0; i < scrollHeight; i++)
    {
        if(updated[i]) continue;
        SetCursorPosition(data->terminalWidth - 1, 2 + i);
        printf("│");
    }

    free(updated);
    
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

    pageData->scrollSize = pageData->elementLimit*2 - 1 - pageData->list->count;
    if(pageData->scrollSize > pageData->elementLimit) {
        pageData->scrollSize = pageData->elementLimit;
    }
    else if(pageData->scrollSize < 1) {
        pageData->scrollSize = 1;
    }

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

        if(mouseY == 0) {
            Scroll(pageData, false);
        }
        else if(mouseY == pageData->terminalHeight - 2) {
            Scroll(pageData, true);
        }
        else {
            int boxLine = mouseY - 1;
            float lineWorth = (float)pageData->list->count / (float)(pageData->elementLimit - 2);
            pageData->selected = (int)((float)boxLine * lineWorth);

            if(pageData->selected >= pageData->list->count) {
                pageData->selected = pageData->list->count - 1;
            }
            else if(pageData->selected < 0) {
                pageData->selected = 0;
            }
        }
    }
    else {
        int index = mouseY - 1;
        if(index < 0 || index >= pageData->elementLimit) return;

        pageData->selected = index + pageData->drawQuestionStartIndex;
    }

    DrawVisibleQuestions(pageData);
    DrawScrollBar(pageData, oldSelected);
}

void OnMouseDoubleClick(int button, int mouseX, int mouseY, void* data) {
    if((button & MOUSE_LEFT_BUTTON) == 0) return;
    QuestionListPageData* pageData = (QuestionListPageData*)data;

    if(mouseX >= pageData->terminalWidth - 3) {
        return; // Scrollbar
    }
    else {
        int index = mouseY - 1;
        if(index < 0 || index >= pageData->elementLimit) return; // Out of bounds

        // In bounds but on mouse click has already selected the item
    }

    EnterPreview(pageData);
}

void EnterPreview(QuestionListPageData* data) {
    UnsetResizeHandler();
    UnsetMouseHandler();

    PageEnter_QuizQuestionPreview(ListGetAt(data->list, data->selected));

    SetResizeHandler(OnResize, data);
    SetMouseHandler(OnMouseClick, OnMouseDoubleClick, OnScroll, NULL, data);

    DrawAll(data);
}

void PageEnter_QuestionList()
{
    HideCursor();

    QuestionListPageData* data = malloc(sizeof(QuestionListPageData));

    GetTerminalSize(&data->terminalWidth, &data->terminalHeight);

    data->drawQuestionStartIndex = 0;
    data->selected = 0;
    data->list = GetQuestionList();

    OnResize(data->terminalWidth, data->terminalHeight, data);

    SetResizeHandler(OnResize, data);
    SetMouseHandler(OnMouseClick, OnMouseDoubleClick, OnScroll, NULL, data);

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
                EnterPreview(data);
                break;

            case KEY_ESCAPE:
                UnsetResizeHandler();
                UnsetMouseHandler();
                return;

            default:
                break;
        }
    }
}