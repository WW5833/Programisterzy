#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "MainMenuPage.h"
#include "Settings.h"
#include "QuizManager.h"
#include "AnsiHelper.h"
#include "IOHelper.h"
#include "WelcomePage.h"

Settings* LoadedSettings;

extern int LatestTerminalWidth, LatestTerminalHeight;

int main() {
    srand((unsigned int)time(NULL));

    InitializeIO();

    LoadedSettings = LoadSettings();

    ClearScreenManual();
    EnableAlternativeBuffer();

    ResetColor();
    ClearScreen();
    HideCursor();

    LoadQuestions();

    if(GetQuestionList() == NULL) {
        ExitAppWithErrorMessage(EXIT_FAILURE, "Failed to load questions");
    }

    IOLoop();

    if(LatestTerminalHeight == -1) {
        UpdateTerminalSize();
    }

    if(LoadedSettings->TutorialShown == 0) {
        PageEnter_Welcome();
    }

    PageEnter_MainMenu();

    free(LoadedSettings);
    ExitApp(EXIT_SUCCESS);
}
