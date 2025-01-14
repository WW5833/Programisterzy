#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "Pages/MainMenuPage.h"
#include "Settings.h"
#include "QuizManager.h"
#include "AnsiHelper.h"
#include "IOHelper/IOHelper.h"
#include "Pages/WelcomePage.h"

extern Settings LoadedSettings;

extern int LatestTerminalWidth, LatestTerminalHeight;

int main() {
    srand((unsigned int)time(NULL));

    InitializeIO();

    LoadSettings();

    UpdateTerminalSize();
    ClearScreenManual();
    EnableAlternativeBuffer();

    ResetColor();
    ClearScreen();
    HideCursor();

    SetThisConsoleTitle("Programisterzy");

    LoadQuestions();

    if(GetQuestionList() == NULL) {
        ExitAppWithErrorMessage(EXIT_FAILURE, ERRMSG_QUESTION_FAILED_TO_LOAD);
    }

    IOLoop();

    if(LatestTerminalHeight == -1) {
        UpdateTerminalSize();
    }

    if(LoadedSettings.TutorialShown == 0) {
        PageEnter_Welcome();
    }

    PageEnter_MainMenu();

    ExitApp(EXIT_SUCCESS);
}
