#include "QuestionListPage.h"

#include "AnsiHelper.h"
#include "QuestionDetailsPage.h"
#include "EditQuestionPage.h"
#include "IOHelper.h"
#include "RGBColors.h"
#include "TextHelper.h"

#include <string.h>

#define ADD_QUESTION_ID -1

#define SCROLL_BAR_HANDLE "█"
#define SCROLL_BAR_LINE "│"

typedef struct {
    int terminalWidth, terminalHeight;
    int blockWidth, elementLimit;

    QuestionListHeader* list;
    int selected;

    int drawQuestionStartIndex;
    int scrollSize;

    bool questionListModified;
} QuestionListPageData;

void EnterPreview(QuestionListPageData* data, bool mainLoop);
void CalculateStartIndex(QuestionListPageData* data);

void OnQuestionListPageResize(int width, int height, void* data);
void OnQuestionListPageScroll(bool down, int mouseX, int mouseY, void* data);
void OnQuestionListPageMouseClick(int button, int mouseX, int mouseY, void* data);
void OnQuestionListPageMouseDoubleClick(int button, int mouseX, int mouseY, void* data);

void DrawVisibleQuestions(QuestionListPageData* data);
void DrawScrollBar(QuestionListPageData* data, int oldSelected);

void AddScrollBar(QuestionListPageData* data);
void RemoveScrollBar(QuestionListPageData* data, int oldSelected);

void DrawStaticScroll(QuestionListPageData* data) {
    if(data->list->count <= data->elementLimit) return;

    for (int i = 0; i < data->terminalHeight; i++)
    {
        SetCursorPosition(data->terminalWidth - 1, 1 + i);
        if(i == 0) {
            printf("▲");
        }
        else if(i == data->terminalHeight - 1) {
            printf("▼");
        }
        else {
            printf(SCROLL_BAR_LINE);
        }
    }
}

void PrintQuestionLine(Question* question, int widthLimit, int blockWidth) {
    int length = GetStringCharCount(question->Content);

    printf(CSR_MOVE_CURSOR_X(blockWidth));
    printf(CLR_LINE_START "\r");

    if(question->Id == ADD_QUESTION_ID) {
        printf("     Dodaj nowe pytanie");
        return;
    }

    printf("%3d. ", question->Id);

    if(widthLimit < length) {
        const char* currentChar = question->Content;
        for (int j = 0; j < widthLimit - 3; j++)
        {
            const char* next = GetNextChar(currentChar);

            printf("%.*s", (int)(next - currentChar), currentChar);

            currentChar = next;
        }

        printf("...");
    }
    else {
        printf("%s", question->Content);
    }
}

void UpdateVisibleQuestions(QuestionListPageData* data, int oldSelected) {
    int widthLimit = data->blockWidth - 5;

    int oldStartIndex = data->drawQuestionStartIndex;
    CalculateStartIndex(data);

    int delta = data->drawQuestionStartIndex - oldStartIndex;
    if(abs(delta) >= data->elementLimit) {
        DrawVisibleQuestions(data);
        DrawScrollBar(data, oldSelected);
        return;
    }

    // Clear old selected
    SetCursorPosition(0, 2 + oldSelected - oldStartIndex);
    PrintQuestionLine(ListGetAt(data->list, oldSelected), widthLimit, data->blockWidth);

    // Remove scrollBarHandle
    RemoveScrollBar(data, oldSelected);

    if(delta > 0) {
        printf(SCREEN_SCROLL_UP(delta));
    }
    else if(delta < 0) {
        printf(SCREEN_SCROLL_DOWN(-delta));
    }

    // Add missing lines
    for (int i = 0; i < abs(delta); i++)
    {
        if(delta > 0) {
            SetCursorPosition(data->terminalWidth - 1, 2 + data->elementLimit - delta + i);
        }
        else {
            SetCursorPosition(data->terminalWidth - 1, 2 + i);
        }

        printf(SCROLL_BAR_LINE);
    }

    // Draw scrollBarHandle
    AddScrollBar(data);

    // Set new selected
    SetColorRGBPreset(RGB_ID_WHITE, false);
    SetColorRGBPreset(RGB_ID_BLUE, true);
    SetCursorPosition(0, 2 + data->selected - data->drawQuestionStartIndex);
    PrintQuestionLine(ListGetAt(data->list, data->selected), widthLimit, data->blockWidth);
    ResetColor();

    if(delta == 0) return;

    for (int i = 0; i < abs(delta); i++)
    {
        int index;

        if(delta > 0) {
            SetCursorPosition(0, 2 + data->elementLimit - delta + i);
            index = data->drawQuestionStartIndex + data->elementLimit - delta + i;
        }
        else {
            SetCursorPosition(0, 2 + i);
            index = data->drawQuestionStartIndex + i;
        }

        if(index == data->selected) continue; // Skip selected

        PrintQuestionLine(ListGetAt(data->list, index), widthLimit, data->blockWidth);
    }
}

void CalculateStartIndex(QuestionListPageData* data) {
    data->drawQuestionStartIndex = 0;
    if(data->selected > data->elementLimit / 2) {
        data->drawQuestionStartIndex = data->selected - data->elementLimit / 2;
    }

    if(data->drawQuestionStartIndex + data->elementLimit > data->list->count) {
        data->drawQuestionStartIndex = data->list->count - data->elementLimit;
    }
}

void DrawVisibleQuestions(QuestionListPageData* data) {
    SetCursorPosition(0, 2);
    int i = 0;

    CalculateStartIndex(data);

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
            SetColorRGBPreset(RGB_ID_WHITE, false);
            SetColorRGBPreset(RGB_ID_BLUE, true);
        }

        PrintQuestionLine(current->data, widthLimit, data->blockWidth);

        if(i == data->selected) {
            ResetColor();
        }

        i++;
        current = current->next;

        if(i >= data->drawQuestionStartIndex + data->elementLimit) {
            break;
        }

        printf("\n");
    }
}

void DrawScrollBar(QuestionListPageData* data, int oldSelected) {
    if(data->list->count <= data->elementLimit) return;

    int scrollHeight = data->terminalHeight - 2;
    float movePerElement = (float)scrollHeight / (float)data->list->count;
    int scrollPosition = (int)((float)data->selected * movePerElement);
    int oldScrollPosition = (int)((float)oldSelected * movePerElement);

    if(scrollPosition == oldScrollPosition) {
        return;
    }

    bool* updated = malloc((size_t)scrollHeight * sizeof(bool));
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
        printf(SCROLL_BAR_HANDLE);
        updated[scrollPosition + i] = true;
    }

    for (int i = 0; i < scrollHeight; i++)
    {
        if(updated[i]) continue;
        SetCursorPosition(data->terminalWidth - 1, 2 + i);
        printf(SCROLL_BAR_LINE);
    }

    free(updated);

    ResetCursor();
}

void RemoveScrollBar(QuestionListPageData* data, int oldSelected) {
    if(data->list->count <= data->elementLimit) return;

    int scrollHeight = data->terminalHeight - 2;
    float movePerElement = (float)scrollHeight / (float)data->list->count;
    int scrollPosition = (int)((float)oldSelected * movePerElement);

    scrollPosition -= data->scrollSize / 2;
    if(scrollPosition < 0) scrollPosition = 0;
    else if(scrollPosition > scrollHeight - data->scrollSize) scrollPosition = scrollHeight - data->scrollSize;

    for (int i = 0; i < data->scrollSize; i++)
    {
        SetCursorPosition(data->terminalWidth - 1, 2 + scrollPosition + i);
        printf(SCROLL_BAR_LINE);
    }

    ResetCursor();
}

void AddScrollBar(QuestionListPageData* data) {
    if(data->list->count <= data->elementLimit) return;

    int scrollHeight = data->terminalHeight - 2;
    float movePerElement = (float)scrollHeight / (float)data->list->count;
    int scrollPosition = (int)((float)data->selected * movePerElement);

    scrollPosition -= data->scrollSize / 2;
    if(scrollPosition < 0) scrollPosition = 0;
    else if(scrollPosition > scrollHeight - data->scrollSize) scrollPosition = scrollHeight - data->scrollSize;

    for (int i = 0; i < data->scrollSize; i++)
    {
        SetCursorPosition(data->terminalWidth - 1, 2 + scrollPosition + i);
        printf(SCROLL_BAR_HANDLE);
    }

    ResetCursor();
}

void DrawAll(QuestionListPageData* data) {
    ClearScreen();

    printf("Lista pytań\n");

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

    UpdateVisibleQuestions(data, oldSelected);
}

extern int LatestTerminalWidth, LatestTerminalHeight;

static Question AddQuestionQuestion = {ADD_QUESTION_ID, "", { "", "", "", "" }};

bool delayedEnterQuiz = false;
void EnterPreview(QuestionListPageData* data, bool mainLoop) {
    if(!mainLoop) {
        delayedEnterQuiz = true;
        return;
    }

    UnsetResizeHandler();
    UnsetMouseHandler();

    Question* question = ListGetAt(data->list, data->selected);
    if(question->Id == ADD_QUESTION_ID) {
        Question* newQuestion = malloc(sizeof(Question));
        newQuestion->Id = GetMaxQuestionId() + 1;
        newQuestion->Content = malloc(1 * sizeof(char));
        newQuestion->Content[0] = '\0';
        for (int i = 0; i < 4; i++)
        {
            newQuestion->Answer[i] = malloc(1 * sizeof(char));
            newQuestion->Answer[i][0] = '\0';
        }

        if(PageEnter_EditQuestion(newQuestion, true)) {
            data->questionListModified = true;
        }
    }
    else {
        data->questionListModified = false;
        PageEnter_QuestionDetails(question, &data->questionListModified);
    }
    

    delayedEnterQuiz = false;

    SetResizeHandler(OnQuestionListPageResize, data);
    SetMouseHandler(OnQuestionListPageMouseClick, OnQuestionListPageMouseDoubleClick, OnQuestionListPageScroll, NULL, data);

    if(data->terminalWidth != LatestTerminalWidth || data->terminalHeight != LatestTerminalHeight) {
        OnQuestionListPageResize(LatestTerminalWidth, LatestTerminalHeight, data);
    }
    else if(!data->questionListModified) {
        DrawAll(data);
    }

    if(data->questionListModified) {
        data->questionListModified = false;
        ListDestroy(data->list, false);
        data->list = GetQuestionListCopy();
        ListInsert(data->list, 0, &AddQuestionQuestion);
        DrawAll(data);
    }
}

void PageEnter_QuestionList()
{
    HideCursor();

    QuestionListPageData data;
    data.terminalWidth = LatestTerminalWidth;
    data.terminalHeight = LatestTerminalHeight;

    data.drawQuestionStartIndex = 0;
    data.selected = 0;
    data.questionListModified = false;
    data.list = GetQuestionListCopy();
    ListInsert(data.list, 0, &AddQuestionQuestion);

    OnQuestionListPageResize(data.terminalWidth, data.terminalHeight, &data);

    SetResizeHandler(OnQuestionListPageResize, &data);
    SetMouseHandler(OnQuestionListPageMouseClick, OnQuestionListPageMouseDoubleClick, OnQuestionListPageScroll, NULL, &data);

    KeyInputType key;
    while(true) {
        if(delayedEnterQuiz) {
            EnterPreview(&data, true);
        }

        key = HandleInteractions(false);

        if(key == KEY_NONE) continue; // No key pressed

        switch (key)
        {
            case KEY_ARROW_UP:
                Scroll(&data, false);
                break;

            case KEY_ARROW_DOWN:
                Scroll(&data, true);
                break;

            case KEY_ENTER:
                EnterPreview(&data, true);
                break;

            case KEY_ESCAPE:
                UnsetResizeHandler();
                UnsetMouseHandler();
                ListDestroy(data.list, false);
                printf(SCREEN_SCROLL_REGION_RESET);
                return;

            default:
                break;
        }
    }
}

void OnQuestionListPageResize(int width, int height, void* data)
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

    printf(SCREEN_SCROLL_REGION(2, pageData->terminalHeight - 1)); // Set scrolling region

    DrawAll(pageData);
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void OnQuestionListPageScroll(bool down, int mouseX, int mouseY, void* data) {
    Scroll((QuestionListPageData*)data, down);
}
#pragma GCC diagnostic warning "-Wunused-parameter"

void OnQuestionListPageMouseClick(int button, int mouseX, int mouseY, void* data) {
    if((button & MOUSE_LEFT_BUTTON) == 0) return;
    QuestionListPageData* pageData = (QuestionListPageData*)data;

    int oldSelected = pageData->selected;
    if(mouseX >= pageData->terminalWidth - 3) {

        if(mouseY == 0) {
            Scroll(pageData, false);
        }
        else if(mouseY == pageData->terminalHeight - 1) {
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

        if(pageData->selected == oldSelected) {
            EnterPreview(pageData, false);
            return;
        }
    }

    UpdateVisibleQuestions(pageData, oldSelected);
}

void OnQuestionListPageMouseDoubleClick(int button, int mouseX, int mouseY, void* data) {
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

    EnterPreview(pageData, false);
}
