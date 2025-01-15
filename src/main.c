#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "MainMenuPage.h"
#include "Settings.h"
#include "QuizManager.h"
#include "AnsiHelper.h"
#include "IOHelper.h"
#include "WelcomePage.h"

extern Settings LoadedSettings;
extern int LatestTerminalWidth, LatestTerminalHeight;

int main() {
    srand((unsigned int)time(NULL));

    InitializeIO();

    LoadSettingsFromFile();

    UpdateTerminalSize();
    ClearScreenManual();
    EnableAlternativeBuffer();

    ResetColor();
    ClearScreen();
    HideCursor();

    SetThisConsoleTitle("Programisterzy");

    LoadQuestionsFromFile();

    if(GetQuestionList() == NULL) {
        ExitAppWithErrorMessage(EXIT_FAILURE, ERRMSG_QUESTION_FAILED_TO_LOAD);
    }

    IOLoop();

    if(LatestTerminalHeight == -1) {
        UpdateTerminalSize();
    }

    if(LoadedSettings.WelcomePageShown == 0) {
        PageEnter_Welcome();
    }

    PageEnter_MainMenu();

    ExitApp(EXIT_SUCCESS);
}
