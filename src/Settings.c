#include "Settings.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "AnsiHelper.h"

#define SETTINGS_FILE "./settings.txt"

Settings *LoadSettings()
{
    FILE* file = fopen(SETTINGS_FILE, "r");

    Settings* settings = malloc(sizeof(Settings));

    if(file == NULL) {
        settings->CorrectAnswerColor = COLOR_FG_GREEN;
        settings->WrongAnswerColor = COLOR_FG_RED;
        settings->SelectedAnswerColor = COLOR_FG_YELLOW;
        settings->ConfirmedAnswerColor = COLOR_FG_CYAN;
        settings->SupportColor = COLOR_FG_MAGENTA;
        settings->FullUTF8Support = 1;
        settings->TutorialShown = 0;
        settings->AutoResizeUI = 1;

        SaveSettings(settings);

        return settings;
    }

    fscanf(file, "%d;%d;%d;%d;%d;%d;%d;%d",
        &settings->CorrectAnswerColor, 
        &settings->WrongAnswerColor, 
        &settings->SelectedAnswerColor, 
        &settings->ConfirmedAnswerColor, 
        &settings->SupportColor, 
        &settings->FullUTF8Support, 
        &settings->TutorialShown,
        &settings->AutoResizeUI);

    if(fclose(file) != 0) {
        perror("Failed to close file");
        exit(EXIT_FAILURE);
    }

    return settings;
}

void SaveSettings(Settings *settings)
{
    FILE* file = fopen(SETTINGS_FILE, "w");

    if(file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%d;%d;%d;%d;%d;%d;%d;%d",
        settings->CorrectAnswerColor, 
        settings->WrongAnswerColor, 
        settings->SelectedAnswerColor, 
        settings->ConfirmedAnswerColor, 
        settings->SupportColor,
        settings->FullUTF8Support, 
        settings->TutorialShown,
        settings->AutoResizeUI);

    if(fclose(file) != 0) {
        perror("Failed to close file");
        exit(EXIT_FAILURE);
    }
}

void DestroySettings(Settings *settings)
{
    free(settings);
}
