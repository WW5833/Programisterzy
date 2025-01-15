#include "QuestionDetailsPage.h"
#include "AnsiHelper.h"
#include "IOHelper.h"
#include "QuestionEditPage.h"
#include "QuizQuestionPage.h"
#include "Popup.h"
#include "TextHelper.h"

#define OPTION_COUNT 5 

#define SET_COLOR_RED           ESC_SEQ "38;2;139;0;0m"
#define SET_COLOR_BRIGHT_RED    ESC_SEQ "38;2;255;36;0m"

typedef struct {
    int terminalWidth, terminalHeight;

    Question *question;
    int selectedOption;
    int contentLines;
} QuestionDetailsPageData;

static bool DeleteQuestionPrompt(QuestionDetailsPageData* data) {
    if(!ShowConfirmationPopup("Czy na pewno chcesz usunąc to pytanie?\n\n"
        SET_COLOR_RED "U W A G A !" SET_COLOR_BRIGHT_RED
        "\nTej operacji nie można cofnąc!!", "Tak", "Nie", 48)) {
        return false;
    }

    char* message;
    if(DeleteQuestion(data->question, &message)) {
        ShowAlertPopup("Pytanie usunięte pomyślnie.", 31);
        return true;
    }

    char buffer[256];
    sprintf(buffer, "Nie udało się usunąć pytania!\n\n%s", message);
    ShowAlertPopupWithTitle(ERRMSG_ERROR_POPUP_TITLE, buffer, 40);
    return true;
}

static void DrawUI(QuestionDetailsPageData* data) {
    ClearScreen();

    printf("Informacje o pytaniu (Id: %d)\n", data->question->Id);
    printf("Treść pytania: ");
    data->contentLines = 3 + PrintWrappedLine(data->question->Content, data->terminalWidth - 15, 14, false);
    printf("\n\n");
    printf("[ ] Edytuj pytanie\n");
    printf("[ ] Podgląd pytania\n");
    printf("[ ] Podgląd pytania z odpowiedziami\n");
    printf("[ ] Usuń pytanie\n");
    printf("[ ] Powrót\n");

    SetCursorPosition(2, data->contentLines + data->selectedOption);
    printf("*");
}

extern int LatestTerminalWidth, LatestTerminalHeight;

static void OnResize(void* data) {
    QuestionDetailsPageData* pageData = (QuestionDetailsPageData*)data;
    pageData->terminalWidth = LatestTerminalWidth;
    pageData->terminalHeight = LatestTerminalHeight;

    DrawUI(pageData);
}

static void DrawUI_UpdateOptionSelector(QuestionDetailsPageData* data, int oldSelected) {
    SetCursorPosition(2, data->contentLines + oldSelected);
    printf(" ");
    SetCursorPosition(2, data->contentLines + data->selectedOption);
    printf("*");
}

void PageEnter_QuestionDetails(Question *question, bool* outDeleted)
{
    QuestionDetailsPageData data;
    data.terminalWidth = LatestTerminalWidth;
    data.terminalHeight = LatestTerminalHeight;
    data.selectedOption = 0;
    data.question = question;

    DrawUI(&data);

    SetResizeHandler(OnResize, &data);

    int oldSelected;
    bool continueLoop = true;
    while (continueLoop)
    {
        KeyInputType key = HandleInteractions(true);

        switch (key) {
            case KEY_ARROW_UP:
                oldSelected = data.selectedOption--;
                if (data.selectedOption < 0) data.selectedOption = OPTION_COUNT - 1;

                DrawUI_UpdateOptionSelector(&data, oldSelected);
                break;

            case KEY_ARROW_DOWN:
                oldSelected = data.selectedOption++;
                if (data.selectedOption >= OPTION_COUNT) data.selectedOption = 0;
                
                DrawUI_UpdateOptionSelector(&data, oldSelected);
                break;

            case KEY_ENTER: {
                switch (data.selectedOption) {
                    case 0:
                        // Edit question
                        PageEnter_QuestionEdit(data.question, false);
                        SetResizeHandler(OnResize, &data);
                        break;
                    case 1:
                        // Preview question
                        PageEnter_QuizQuestionPreview(data.question, false);
                        SetResizeHandler(OnResize, &data);
                        break;
                    case 2:
                        // Preview question with answers
                        PageEnter_QuizQuestionPreview(data.question, true);
                        SetResizeHandler(OnResize, &data);
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
                        continueLoop = false;
                        break;
                }
                DrawUI(&data);
                break;
            }

            case KEY_ESCAPE:
                continueLoop = false;
                break;
            
            default:
                break;
        }
    }

    UnsetResizeHandler();
}
