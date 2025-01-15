#include "DebugPage.h"
#include "IOHelper.h"
#include "QuizManager.h"
#include "AnsiHelper.h"
#include "TextHelper.h"

#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define MIN_TERMINAL_WIDTH 70
#define MIN_TERMINAL_HEIGHT 26

extern int LatestTerminalWidth, LatestTerminalHeight;
static void CalculateQuizPageWidthRequirements() {

    printf("\nCalculating quiz page width requirements\n");

    const int rewardBoxWidth = 19 + 1;
    const int rewardBoxHeight = 10;
    const int staticWidthContent = 14 + 2;
    const int staticWidthAnswer = 4 + 5 + 3 + rewardBoxWidth + 1;
    const int staticHeightAnswer = (1 + 1 + 1 + 1)*2;
    const int staticHeight = 1 + 1 + 1 + 3 + 1;

    const int absMinWidth = 57;

    int maxLengthWordAnswerID = 0;
    int maxLengthWordAnswer = 0;
    int maxLengthWordContentID = 0;
    int maxLengthWordContent = 0;

    int maxLineCountAnswerID = 0;
    int maxLineCountAnswer = 0;
    int maxLineCountContentID = 0;
    int maxLineCountContent = 0;

    QuestionListHeader* list = GetQuestionList();

    QuestionListItem* current = list->head;
    while (current != NULL)
    {
        int lengthWordContent = GetMaxWordLength(current->data->Content);
        if(lengthWordContent > maxLengthWordContent) {
            maxLengthWordContent = lengthWordContent;
            maxLengthWordContentID = current->data->Id;
        }

        for (int i = 0; i < 4; i++)
        {
            int lengthWordAnswer = GetMaxWordLength(current->data->Answer[i]);
            if(lengthWordAnswer > maxLengthWordAnswer) {
                maxLengthWordAnswer = lengthWordAnswer;
                maxLengthWordAnswerID = current->data->Id;
            }
        }

        current = current->next;
    }


    int width = MAX(absMinWidth, MAX(staticWidthContent + maxLengthWordContent, staticWidthAnswer + maxLengthWordAnswer*2));
    const int minContentWidth = width - staticWidthContent;
    const int minAnswerWidth = (width - staticWidthAnswer)/2;

    current = list->head;
    while (current != NULL)
    {
        int lineCountContent = GetWrappedLineCount(current->data->Content, minContentWidth);
        if(lineCountContent > maxLineCountContent) {
            maxLineCountContent = lineCountContent;
            maxLineCountContentID = current->data->Id;
        }

        for (int i = 0; i < 4; i++)
        {
            int lineCountAnswer = GetWrappedLineCount(current->data->Answer[i], minAnswerWidth);
            if(lineCountAnswer > maxLineCountAnswer) {
                maxLineCountAnswer = lineCountAnswer;
                maxLineCountAnswerID = current->data->Id;
            }
        }

        current = current->next;
    }

    int height = staticHeight + maxLineCountContent + MAX((staticHeightAnswer + maxLineCountAnswer*2), rewardBoxHeight);

    width = MAX(width, MIN_TERMINAL_WIDTH);
    height = MAX(height, MIN_TERMINAL_HEIGHT);

    printf("Terminal size: %d x %d\n", LatestTerminalWidth, LatestTerminalHeight);
    printf("Required size: %d x %d\n", width, height);
    printf("\n");
    printf("Max content word length: %d [%d]\n", maxLengthWordContent, maxLengthWordContentID);
    printf("Max answer word length: %d [%d]\n", maxLengthWordAnswer, maxLengthWordAnswerID);
    printf("\n");
    printf("Max content line count: %d [%d]\n", maxLineCountContent, maxLineCountContentID);
    printf("Max answer line count: %d [%d]\n", maxLineCountAnswer, maxLineCountAnswerID);
    printf("\n");
}

static void DrawUI() {
    ClearScreen();
    printf("Debug page\n");

    CalculateQuizPageWidthRequirements();

    printf("\nSymbols");
    printf("\n║═╔╗╚╝╬╠╣╦╩╧╤▁▂▃▄▅▆▇█▶│─┌┐└┘┤├↑↓←→");

    printf("\n\n");
    printf(ESC_SEQ "1m" "TEST" ESC_SEQ "22m" "\n");
    printf(ESC_SEQ "2m" "TEST" ESC_SEQ "22m" "\n");
    printf(ESC_SEQ "3m" "TEST" ESC_SEQ "23m" "\n");
    printf(ESC_SEQ "4m" "TEST" ESC_SEQ "24m" "\n");
    printf(ESC_SEQ "5m" "TEST" ESC_SEQ "25m" "\n");
    printf(ESC_SEQ "7m" "TEST" ESC_SEQ "27m" "\n");
    printf(ESC_SEQ "8m" "TEST" ESC_SEQ "28m" "\n");
    printf(ESC_SEQ "9m" "TEST" ESC_SEQ "29m" "\n");

    printf("\n[Esc] - Wciśnij aby powrócić do głównego Menu.\n");
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
static void OnResize(void* data) {
#pragma GCC diagnostic warning "-Wunused-parameter"

    DrawUI();
}

void PageEnter_Debug()
{
    DrawUI();
    SetResizeHandler(OnResize, NULL);

    WaitForKeys(ESC, ENTER);

    UnsetResizeHandler();
}
