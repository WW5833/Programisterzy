#include "DebugPage.h"
#include "PageUtils.h"
#include "QuizManager.h"
#include "AnsiHelper.h"

#define MAX(x, y) ((x) > (y) ? (x) : (y))

int GetMaxWordLength(const char* line)
{
    int max = 0;
    int current = 0;
    while (*line != '\0')
    {
        if (*line == ' ')
        {
            max = MAX(max, current);
            current = 0;
        }
        else
        {
            current++;
        }
        line++;
    }

    return max;
}

void PageEnter_Debug()
{
    int terminalWidth, terminalHeight;
    GetTerminalSize(&terminalWidth, &terminalHeight);

    ClearScreen();
    printf("Debug page\n");

    QuestionListHeader *list = GetQuestionList();

    QuestionListItem *current = list->head;

    const int rewardBoxWidth = 19 + 1;
    const int rewardBoxHeight = 10;
    const int staticWidthContent = 14 + 2;
    const int staticWidthAnswer = 4 + 5 + 3 + rewardBoxWidth + 1;
    const int staticHeight = 2 + 4 + 4 + 2 + 3 + 1;

    const int absMinWidth = 57;

    const int absMinContentWidth = 45;
    const int absMinAnsWidth = 13;

    int maxLengthWordAnswerID = 0;
    int maxLengthWordAnswer = 0;
    int maxLengthWordContentID = 0;
    int maxLengthWordContent = 0;

    int maxLineCountAnswerID = 0;
    int maxLineCountAnswer = 0;
    int maxLineCountContentID = 0;
    int maxLineCountContent = 0;

    while (current != NULL)
    {
        int lineCountContent = GetWrappedLineCount(current->data->Content, absMinContentWidth);
        if(lineCountContent > maxLineCountContent) {
            maxLineCountContent = lineCountContent;
            maxLineCountContentID = current->data->Id;
        }
        int lengthWordContent = GetMaxWordLength(current->data->Content);
        if(lengthWordContent > maxLengthWordContent) {
            maxLengthWordContent = lengthWordContent;
            maxLengthWordContentID = current->data->Id;
        }

        for (int i = 0; i < 4; i++)
        {
            int lineCountAnswer = GetWrappedLineCount(current->data->Answer[i], absMinAnsWidth);
            if(lineCountAnswer > maxLineCountAnswer) {
                maxLineCountAnswer = lineCountAnswer;
                maxLineCountAnswerID = current->data->Id;
            }
            int lengthWordAnswer = GetMaxWordLength(current->data->Answer[i]);
            if(lengthWordAnswer > maxLengthWordAnswer) {
                maxLengthWordAnswer = lengthWordAnswer;
                maxLengthWordAnswerID = current->data->Id;
            }
        }

        current = current->next;
    }

    int width = MAX(absMinWidth, MAX(staticWidthContent + maxLengthWordContent, staticWidthAnswer + maxLengthWordAnswer*2));
    int height = staticHeight + maxLineCountContent + MAX(maxLineCountAnswer*2, rewardBoxHeight);

    printf("Terminal size: %d x %d\n", terminalWidth, terminalHeight);
    printf("Required size: %d x %d\n", width, height);
    printf("\n");
    printf("\n");
    printf("Max content word length: %d [%d]\n", maxLengthWordContent, maxLengthWordContentID);
    printf("Max answer word length: %d [%d]\n", maxLengthWordAnswer, maxLengthWordAnswerID);
    printf("\n");
    printf("Max content line count: %d [%d]\n", maxLineCountContent, maxLineCountContentID);
    printf("Max answer line count: %d [%d]\n", maxLineCountAnswer, maxLineCountAnswerID);
    printf("\n");
    printf("\n");

    WaitForEnter();
}