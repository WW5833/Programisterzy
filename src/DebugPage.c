#include "DebugPage.h"
#include "PageUtils.h"
#include "QuizManager.h"
#include "AnsiHelper.h"

#define MAX(x, y) ((x) > (y) ? (x) : (y))

int GetMaxWordLength(const char* line)
{
    int max = 0;
    const char* lastSpace = line;
    while (*line != '\0')
    {
        if (*line == ' ')
        {
            max = MAX(max, GetCharCount(lastSpace, line));
            lastSpace = line;
        }

        line++;
    }

    max = MAX(max, GetCharCount(lastSpace, line));

    return max;
}

void CalculateQuizPageWidthRequirements(const int terminalWidth, const int terminalHeight) {

    printf("Calculating quiz page width requirements\n");

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


    const int width = MAX(absMinWidth, MAX(staticWidthContent + maxLengthWordContent, staticWidthAnswer + maxLengthWordAnswer*2));
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

    printf("Terminal size: %d x %d\n", terminalWidth, terminalHeight);
    printf("Required size: %d x %d\n", width, height);
    printf("\n");
    printf("Max content word length: %d [%d]\n", maxLengthWordContent, maxLengthWordContentID);
    printf("Max answer word length: %d [%d]\n", maxLengthWordAnswer, maxLengthWordAnswerID);
    printf("\n");
    printf("Max content line count: %d [%d]\n", maxLineCountContent, maxLineCountContentID);
    printf("Max answer line count: %d [%d]\n", maxLineCountAnswer, maxLineCountAnswerID);
    printf("\n");
}

extern int LatestTerminalWidth, LatestTerminalHeight;

void PageEnter_Debug()
{
    ClearScreen();
    printf("Debug page\n");

    CalculateQuizPageWidthRequirements(LatestTerminalWidth, LatestTerminalHeight);

    printf("\n[Esc] - Wciśnij Escape aby powrócić do głównego Menu.\n");
    WaitForKeys(ESC, ENTER);
}