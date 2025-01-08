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

    GetTerminalSize(&LatestTerminalWidth, &LatestTerminalHeight);

    ClearScreenManual();
    EnableAlternativeBuffer();

    ResetColor();
    ClearScreen();

    LoadQuestions();

    if(GetQuestionList() == NULL) {
        fprintf(stderr, "Failed to load questions\n");
        ExitApp(EXIT_FAILURE);
    }

    LoadedSettings = LoadSettings();

    if(LoadedSettings->TutorialShown == 0) {
        PageEnter_Welcome();
    }

    PageEnter_MainMenu();

    free(LoadedSettings);
    ExitApp(EXIT_SUCCESS);
}
