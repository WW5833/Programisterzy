#include "QuestionEditPage.h"

#include "AnsiHelper.h"
#include <string.h>
#include <ctype.h>
#include "IOHelper.h"
#include "TextHelper.h"
#include "Popup.h"
#include "TextEditor.h"
#include "Utf8Symbols.h"
#include <math.h>

#define SELECTOR_MARKER ">"

#define SET_COLOR_RED           ESC_SEQ "38;2;139;0;0m"
#define SET_COLOR_BRIGHT_RED    ESC_SEQ "38;2;255;36;0m"

typedef struct {
    int terminalWidth;
    int terminalHeight;

    bool newQuestion;
    Question* question;
    int slotNumber;

    int maxLines[6];

    int selectorOffset;
    int textOffset;
    int textStartX;
    int textFieldWidth;
} AddQuestionPageData;

static const char ConfirmAddButtonText[] = "Zatwierdź dodawanie pytania (Wybierz tą opcję)";
static const char ConfirmEditButtonText[] = "Zatwierdź modyfikowanie pytania (Wybierz tą opcję)";

extern int LatestTerminalWidth, LatestTerminalHeight;

static void DrawUI(AddQuestionPageData* data);
static void OnResize(void* data);

static bool EnforceSizeRequirements(AddQuestionPageData* data) {
    int usedHeight = 4 + 2;
    for (int i = 0; i < 6; i++)
    {
        usedHeight += data->maxLines[i] + 1;
    }

    if(usedHeight >= data->terminalHeight) {
        UnsetResizeHandler();
        ShowAlertPopupKeys("Brak miejsca na dodanie nowych linii!", 30, RESIZE_EVENT);
        OnResize(data);
        SetResizeHandler(OnResize, data);
        return true;
    }

    return false;
}

typedef enum {
    LoadTextResult_Cancelled = 0,
    LoadTextResult_ArrowDown = 1,
    LoadTextResult_ArrowUp = 2
} LoadTextResult;

static LoadTextResult LoadText(AddQuestionPageData* data, char** output) {
    int y;
    int outputLength;
    TextEditorResult result = TextEditorResult_Cancelled;
    while(result != TextEditorResult_Completed) {
        y = 5;
        for (int i = 0; i < data->slotNumber; i++)
            y += data->maxLines[i] + 1;

        outputLength = (int)strlen(*output);
        result = OpenTextEditor(output, &outputLength, data->textStartX + 1, y, data->maxLines[data->slotNumber], " " SINGLE_VERTICAL_LINE);
        
        switch (result) {
            case TextEditorResult_ArrowUp:
                return LoadTextResult_ArrowUp;
            case TextEditorResult_ArrowDown:
                return LoadTextResult_ArrowDown;

            case TextEditorResult_OutOfLines:
                data->maxLines[data->slotNumber]++;

                if(!EnforceSizeRequirements(data)) {
                    DrawUI(data);
                }

                continue;

            case TextEditorResult_WindowResized:
                SetResizeHandler(OnResize, data);
                OnResize(data);
                continue;

            case TextEditorResult_Cancelled:
                HideCursor();
                const char* message = data->newQuestion ? "Czy na pewno chcesz anulować dodawanie pytania?" : "Czy na pewno chcesz anulować modyfikowanie pytania?";
                if(ShowConfirmationPopup(message, "Tak", "Nie", 41)) {
                    return LoadTextResult_Cancelled;
                }

                DrawUI(data);
                continue;

            case TextEditorResult_Completed:
                break;
        }

        if(outputLength == 0) {
            HideCursor();
            ShowAlertPopup("To pole jest wymagane !!", 30);
            result = false;
            DrawUI(data);
            continue;
        }
    }

    SetResizeHandler(OnResize, data);

    return LoadTextResult_ArrowDown;
}

static void PrintLine(int lineCount, int offset, const char* top, const char* bottom) {
    printf("\r" CSR_MOVE_UP(lineCount));
    for (int i = -1; i <= lineCount; i++)
    {
        printf(CSR_MOVE_RIGHT(offset));
        if(i == -1) {
            printf("%s", top);
        }
        else if(i == lineCount) {
            printf("%s", bottom);
        }
        else {
            printf(SINGLE_VERTICAL_LINE);
        }
        printf(CSR_MOVE_LEFT_0_DOWN1);
    }
}

static void DrawUI_Element(AddQuestionPageData* data, int slotNumber, int lineCount, char* content, bool selected) {
    bool top = slotNumber == -1;
    bool bottom = slotNumber == 5;

    if(top) PRINT_SINGLE_TOP_BORDER(data->terminalWidth);
    else PRINT_SINGLE_TJUNCTION_BORDER(data->terminalWidth);
    for (int i = 0; i < lineCount; i++)
    {
        PrintGenericBorderEdgesHere(0, data->terminalWidth, SINGLE_VERTICAL_LINE);
        printf("\n");
    }
    if(bottom) PRINT_SINGLE_BOTTOM_BORDER(data->terminalWidth);
    else printf(CSR_MOVE_LEFT_0_DOWN1);

    printf(CSR_MOVE_UP(2));
    PrintLine(lineCount, data->selectorOffset + 1, top ? SINGLE_BREAK_BOTTOM : SINGLE_CROSS, bottom ? SINGLE_BREAK_TOP : SINGLE_CROSS);

    printf(CSR_MOVE_UP(lineCount + 1));
    int offset = (lineCount - 1) / 2;
    int inverseOffset = lineCount - offset - 1;
    if(offset > 0) printf(CSR_MOVE_DOWN(offset));
    if(selected) {
        printf(CSR_MOVE_RIGHT(data->selectorOffset));
        printf(SELECTOR_MARKER);
        printf("\r");
    }

    printf(CSR_MOVE_RIGHT(data->textOffset));

    if(slotNumber == -1) {
        printf("ID pytania:");
    } else if(slotNumber == 0) {
        printf("Podaj treść pytania:");
    } else if(slotNumber == 1) {
        printf("Podaj poprawną odpowiedź:");
    } else if(slotNumber == 5){
        printf("ZATWIERDŹ");
    } else {
        printf("Podaj odpowiedź %c:", 'A' + (slotNumber - 1));
    }

    if(inverseOffset > 0) printf(CSR_MOVE_DOWN(inverseOffset));
    PrintLine(lineCount, data->textStartX - 2, top ? SINGLE_BREAK_BOTTOM : SINGLE_CROSS, bottom ? SINGLE_BREAK_TOP : SINGLE_CROSS);

    printf(CSR_MOVE_UP(lineCount + 1));
    printf("\r" CSR_MOVE_RIGHT(data->textStartX));

    if(content != NULL) {
        int lines = PrintWrappedLine(content, data->textFieldWidth, data->textStartX, slotNumber == -1);

        printf(CSR_MOVE_LEFT_0_DOWN(lineCount - lines + 1));
    }
}

static void DrawOnOnlyMoved(AddQuestionPageData* data) {
    HideCursor();
    SaveCursorPosition();
    ResetCursor();
    printf(CSR_MOVE_DOWN(4));
    for (int i = 0; i < 5; i++)
    {
        if(i == data->slotNumber) SaveCursorPosition();
        int offset = (data->maxLines[i] - 1) / 2;
        int inverseOffset = data->maxLines[i] - offset;
        if(offset > 0) printf(CSR_MOVE_DOWN(offset));

        printf(CSR_MOVE_RIGHT(data->selectorOffset));
        if(i == data->slotNumber) printf(SELECTOR_MARKER);
        else printf(" ");
        printf("\r" CSR_MOVE_DOWN(inverseOffset + 1));
    }

    RestoreCursorPosition();
    printf(CSR_MOVE_RIGHT(data->textStartX));
    ShowCursor();
}

static void DrawUI(AddQuestionPageData* data) {
    HideCursor();
    ClearScreen();

    if(data->newQuestion) {
        printf("Dodaj pytanie:\n");
    }
    else {
        printf("Edytuj pytanie:\n");
    }

    char buffer[4];
    sprintf(buffer, "%3d", data->question->Id);
    DrawUI_Element(data, -1, 1, buffer, false);

    HideCursor();
    SetCursorPosition(0, 4);

    if(data->slotNumber == 0) {
        DrawUI_Element(data, data->slotNumber, data->maxLines[0], NULL, true);
        SaveCursorPosition();
        printf(CSR_MOVE_LEFT_0_DOWN(data->maxLines[0]));

    } else {
        DrawUI_Element(data, 0, data->maxLines[0], data->question->Content, false);
    }

    for (int i = 0; i < 4; i++)
    {
        if(data->slotNumber == i+1) {
            DrawUI_Element(data, data->slotNumber, data->maxLines[i+1], NULL, true);
            SaveCursorPosition();
            printf(CSR_MOVE_LEFT_0_DOWN(data->maxLines[i+1]));
            continue;
        }
        DrawUI_Element(data, i + 1, data->maxLines[i+1], data->question->Answer[i], false);
    }

    DrawUI_Element(data, 5, data->maxLines[5], NULL, data->slotNumber == 5);
    PrintWrappedLine(data->newQuestion ? ConfirmAddButtonText : ConfirmEditButtonText, data->textFieldWidth, data->textStartX, true);

    RestoreCursorPosition();
}

static bool InputLoop(AddQuestionPageData* data) {
    while(true) {
        DrawUI(data);

        while (data->slotNumber < 5)
        {
            DrawOnOnlyMoved(data);

            char** output;

            if(data->slotNumber == 0) {
                output = &data->question->Content;
            } else {
                output = &data->question->Answer[data->slotNumber - 1];
            }

            switch(LoadText(data, output))
            {
                case LoadTextResult_Cancelled:
                    return false;
                case LoadTextResult_ArrowDown:
                    data->slotNumber++;
                    break;
                case LoadTextResult_ArrowUp:
                    data->slotNumber--;
                    if(data->slotNumber < 0) {
                        data->slotNumber = 0;
                    }
                    break;
            }
        }

        HideCursor();
        if(data->question->Content[0] == '\0') {
            ShowAlertPopup("Treść pytania nie może być pusta!", 41);
            data->slotNumber = 0;
            continue;
        }

        bool allPasss = true;
        for (int i = 0; i < 4; i++)
        {
            if(data->question->Answer[i][0] == '\0') {
                allPasss = false;
                if(i == 0) {
                    ShowAlertPopup("Poprawna odpowiedź nie może być pusta!", 42);
                }
                else {
                    char buffer[40];
                    sprintf(buffer, "Odpowiedź %c nie może być pusta!", 'A' + i);
                    ShowAlertPopup(buffer, 41);
                }
                data->slotNumber = i + 1;
                break;
            }
        }

        if(!allPasss) continue;

        const char* confirmText = data->newQuestion ? "Czy na pewno chcesz dodać to pytanie?" : "Czy na pewno chcesz zmodyfikować to pytanie?";

        if(ShowConfirmationPopup(confirmText, "Tak", "Nie", 45)) {
            return true;
        }
        data->slotNumber = 4;
    }
}

static void CalculateValues(AddQuestionPageData* data);

static void OnResize(void* data) {
    AddQuestionPageData* pageData = (AddQuestionPageData*)data;
    pageData->terminalWidth = LatestTerminalWidth;
    pageData->terminalHeight = LatestTerminalHeight;

    CalculateValues(pageData);

    if(!EnforceSizeRequirements(data)) {
        DrawUI(pageData);
    }
}

static void CalculateMaxLines(AddQuestionPageData* data) {
    data->maxLines[5] = 1;
    if(data->question->Content == NULL) {
        data->maxLines[0] = 1;
    }
    else {
        data->maxLines[0] = GetWrappedLineCount(data->question->Content, data->textFieldWidth);
    }
    for (int i = 0; i < 4; i++)
    {
        if(data->question->Answer[i] == NULL) {
            data->maxLines[i + 1] = 1;
            continue;
        }
        data->maxLines[i + 1] = GetWrappedLineCount(data->question->Answer[i], data->textFieldWidth);
    }

    data->maxLines[5] = GetWrappedLineCount(data->newQuestion ? ConfirmAddButtonText : ConfirmEditButtonText, data->textFieldWidth);
}

static void CalculateValues(AddQuestionPageData* data) {
    data->selectorOffset = 1;
    data->textOffset = data->selectorOffset + 3;
    const int width = 25;
    data->textStartX = data->textOffset + width + 3;
    data->textFieldWidth = data->terminalWidth - data->textStartX - 2;

    CalculateMaxLines(data);
}

static bool FinalizeEdit(AddQuestionPageData* data) {
    char buffer[256];
    char* message;
    if(data->newQuestion) {
        if(!AddQuestion(data->question, &message)) {
            sprintf(buffer, "Nie udało się dodać pytania!\n\n%s", message);
            ShowAlertPopupWithTitle(ERRMSG_ERROR_POPUP_TITLE, buffer, 40);
            return false;
        }
        
        ShowAlertPopup("Pytanie dodane pomyślnie.", 31);
    }
    else {
        if(!EditQuestion(data->question, &message)) {
            sprintf(buffer, "Nie udało się edytować pytania!\n\n%s", message);
            ShowAlertPopupWithTitle(ERRMSG_ERROR_POPUP_TITLE, buffer, 40);
            return false;
        }

        ShowAlertPopup("Pytanie zmodyfikowane pomyślnie.", 32);
    }

    return true;
}

bool PageEnter_QuestionEdit(Question* question, bool newQuestion)
{
    ClearScreen();

    AddQuestionPageData data;
    data.terminalWidth = LatestTerminalWidth;
    data.terminalHeight = LatestTerminalHeight;
    data.slotNumber = 0;

    data.question = question;
    data.newQuestion = newQuestion;

    CalculateValues(&data);

    SetResizeHandler(OnResize, &data);

    while(true) {
        if(!InputLoop(&data)) {
            UnsetResizeHandler();
            LoadQuestionsFromFile(); // Reload questions
            return false;
        }

        HideCursor();
        UnsetResizeHandler();

        if(FinalizeEdit(&data)) {
            return true;
        }

        SetResizeHandler(OnResize, &data);
    }
}
