#include "QuestionDetailsPage.h"
#include "AnsiHelper.h"
#include "IOHelper.h"
#include "EditQuestionPage.h"
#include "QuizQuestionPage.h"
#include "Popup.h"

#define OPTION_COUNT 5 

#define SET_COLOR_RED           ESC_SEQ "38;2;139;0;0m"
#define SET_COLOR_BRIGHT_RED    ESC_SEQ "38;2;255;36;0m"

typedef struct {
    int terminalWidth, terminalHeight;

    Question *question;
    int selectedOption;
} QuestionDetailsPageData;

bool DeleteQuestionPrompt(QuestionDetailsPageData* data) {
    if(!ShowConfirmationPopup("Czy na pewno chcesz usunąc to pytanie?\n\n " 
        SET_COLOR_RED "U W A G A !" SET_COLOR_BRIGHT_RED 
        " \nTej operacji nie można cofnąc!!", "Tak", "Nie", 48)) {
        return false;
    }

    ListRemove(GetQuestionList(), data->question);
    DestroyQuestion(data->question);
    data->question = NULL;
    return true;
}

void PrintUI(QuestionDetailsPageData* data) {
    ClearScreen();

    printf("Informacje o pytaniu (Id: %3d)\n", data->question->Id);
    printf("\n");
    printf("[ ] Edytuj pytanie\n");
    printf("[ ] Podgląd pytania\n");
    printf("[ ] Podgląd pytania z odpowiedziami\n");
    printf("[ ] Usuń pytanie\n");
    printf("[ ] Powrót\n");

    SetCursorPosition(2, 3 + data->selectedOption);
    printf("*");
}

extern int LatestTerminalWidth, LatestTerminalHeight;

void PageEnter_QuestionDetails(Question *question, bool* outDeleted)
{
    QuestionDetailsPageData data;
    data.terminalWidth = LatestTerminalWidth;
    data.terminalHeight = LatestTerminalHeight;
    data.selectedOption = 0;
    data.question = question;

    PrintUI(&data);

    while (true)
    {
        KeyInputType key = HandleInteractions(true);

        switch (key) {
            case KEY_ARROW_UP:
                data.selectedOption--;
                if (data.selectedOption < 0) data.selectedOption = 0;
                else PrintUI(&data);
                break;

            case KEY_ARROW_DOWN:
                data.selectedOption++;
                if (data.selectedOption >= OPTION_COUNT) data.selectedOption = OPTION_COUNT - 1;
                else PrintUI(&data);
                break;

            case KEY_ENTER: {
                switch (data.selectedOption) {
                    case 0:
                        // Edit question
                        PageEnter_EditQuestion(data.question, false);
                        break;
                    case 1:
                        // Preview question
                        PageEnter_QuizQuestionPreview(data.question, false);
                        break;
                    case 2:
                        // Preview question with answers
                        PageEnter_QuizQuestionPreview(data.question, true);
                        break;
                    case 3:
                        // Delete question
                        if(DeleteQuestionPrompt(&data)) {
                            *outDeleted = true;
                            return;
                        }
                        break;
                    case 4:
                        // Back
                        return;
                }
                PrintUI(&data);
                break;
            }
            
            default:
                break;
        }
    } 
}