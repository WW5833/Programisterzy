#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "MainMenuPage.h"
#include "AnsiHelper.Windows.h"
#include "Settings.h"
#include "QuizManager.h"
#include "AnsiHelper.h"

Settings* LoadedSettings;

int main() {
    srand((unsigned int)time(NULL));

    system("chcp 65001");
    EnsureAnsiSupport();

    ClearScreen();

    LoadQuestions();

    if(GetQuestionList() == NULL) {
        fprintf(stderr, "Failed to load questions\n");
        exit(EXIT_FAILURE);
    }

    LoadedSettings = LoadSettings();

    PageEnter_MainMenu();

    return 0;
}