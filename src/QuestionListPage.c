#include "QuestionListPage.h"

#include "AnsiHelper.h"
#include "QuestionDetailsPage.h"
#include "QuestionEditPage.h"
#include "IOHelper.h"
#include "RGBColors.h"
#include "TextHelper.h"

#include <string.h>

#define ADD_QUESTION_ID -1
#define RETURN_QUESTION_ID -2

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

    bool exit;
} QuestionListPageData;

void EnterPreview(QuestionListPageData* data, bool mainLoop);
void CalculateStartIndex(QuestionListPageData* data);

void OnQuestionListPageResize(void* data);
void OnQuestionListPageScroll(bool down, void* data);
void OnQuestionListPageMouseClick(int button, void* data);
void OnQuestionListPageMouseDoubleClick(int button, void* data);

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
    else if(question->Id == RETURN_QUESTION_ID) {
        printf("     Powróć do menu");
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

    if(data->drawQuestionStartIndex < 0) {
        data->drawQuestionStartIndex = 0;
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
    mallocCheck(updated);
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
static Question ReturnQuestion = {RETURN_QUESTION_ID, "", { "", "", "", "" }};

bool delayedEnterQuiz = false;
void EnterPreview(QuestionListPageData* data, bool mainLoop) {
    if(!mainLoop) {
        delayedEnterQuiz = true;
        return;
    }

    UnsetResizeHandler();
    UnsetMouseHandler();

    Question* question = ListGetAt(data->list, data->selected);
    if(question->Id == RETURN_QUESTION_ID) {
        data->exit = true;
        return;
    }
    else if(question->Id == ADD_QUESTION_ID) {
        Question* newQuestion = malloc(sizeof(Question));
        mallocCheck(newQuestion);
        newQuestion->Id = GetMaxQuestionId() + 1;
        newQuestion->Content = malloc(1 * sizeof(char));
        mallocCheck(newQuestion->Content);
        newQuestion->Content[0] = '\0';
        for (int i = 0; i < 4; i++)
        {
            newQuestion->Answer[i] = malloc(1 * sizeof(char));
            mallocCheck(newQuestion->Answer[i]);
            newQuestion->Answer[i][0] = '\0';
        }

        if(PageEnter_QuestionEdit(newQuestion, true)) {
            data->questionListModified = true;
            data->selected = data->list->count;
            PageEnter_QuestionDetails(newQuestion, &data->questionListModified);
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
        OnQuestionListPageResize(data);
    }
    else if(!data->questionListModified) {
        DrawAll(data);
    }

    if(data->questionListModified) {
        data->questionListModified = false;
        
        ListDestroy(data->list, false);
        data->list = GetQuestionListCopy();
        ListInsert(data->list, 0, &AddQuestionQuestion);
        ListInsert(data->list, 0, &ReturnQuestion);

        if(data->selected >= data->list->count) {
            data->selected = data->list->count - 1;
        }

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
    data.exit = false;
    data.list = GetQuestionListCopy();
    ListInsert(data.list, 0, &AddQuestionQuestion);
    ListInsert(data.list, 0, &ReturnQuestion);

    OnQuestionListPageResize(&data);

    SetResizeHandler(OnQuestionListPageResize, &data);
    SetMouseHandler(OnQuestionListPageMouseClick, OnQuestionListPageMouseDoubleClick, OnQuestionListPageScroll, NULL, &data);

    KeyInputType key;
    while(!data.exit) {
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
                data.exit = true;
                break;

            default:
                break;
        }
    }

    UnsetResizeHandler();
    UnsetMouseHandler();
    ListDestroy(data.list, false);
    printf(SCREEN_SCROLL_REGION_RESET);
}

void OnQuestionListPageResize(void* data)
{
    QuestionListPageData* pageData = (QuestionListPageData*)data;
    pageData->terminalWidth = LatestTerminalWidth;
    pageData->terminalHeight = LatestTerminalHeight;
    pageData->blockWidth = pageData->terminalWidth - 2;
    pageData->elementLimit = pageData->terminalHeight - 2;

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

void OnQuestionListPageScroll(bool down, void* data) {
    Scroll((QuestionListPageData*)data, down);
}

extern int LatestMouseX, LatestMouseY;

void OnQuestionListPageMouseClick(int button, void* data) {
    if((button & MOUSE_LEFT_BUTTON) == 0) return;
    QuestionListPageData* pageData = (QuestionListPageData*)data;

    int oldSelected = pageData->selected;
    if(LatestMouseX >= pageData->terminalWidth - 3) {

        if(LatestMouseY == 0) {
            Scroll(pageData, false);
        }
        else if(LatestMouseY == pageData->terminalHeight - 1) {
            Scroll(pageData, true);
        }
        else {
            int boxLine = LatestMouseY - 1;
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
        int index = LatestMouseY - 1;
        if(index < 0 || index >= pageData->elementLimit) return;

        pageData->selected = index + pageData->drawQuestionStartIndex;

        if(pageData->selected == oldSelected) {
            EnterPreview(pageData, false);
            return;
        }
    }

    UpdateVisibleQuestions(pageData, oldSelected);
}

void OnQuestionListPageMouseDoubleClick(int button, void* data) {
    if((button & MOUSE_LEFT_BUTTON) == 0) return;
    QuestionListPageData* pageData = (QuestionListPageData*)data;

    if(LatestMouseX >= pageData->terminalWidth - 3) {
        return; // Scrollbar
    }
    else {
        int index = LatestMouseY - 1;
        if(index < 0 || index >= pageData->elementLimit) return; // Out of bounds

        // In bounds but on mouse click has already selected the item
    }

    EnterPreview(pageData, false);
}
