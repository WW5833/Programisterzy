#include "Settings.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "AnsiHelper.h"
#include "IOHelper.h"
#include "RGBColors.h"

#define SETTINGS_FILE "./settings.txt"

void LoadDefaultSettings(Settings* settings) {
    settings->CorrectAnswerColor = RGB_ID_GREEN;
    settings->WrongAnswerColor = RGB_ID_RED;
    settings->SelectedAnswerColor = RGB_ID_YELLOW;
    settings->ConfirmedAnswerColor = RGB_ID_CYAN;
    settings->SupportColor = RGB_ID_MAGENTA;
    settings->FullUTF8Support = 1;
    settings->TutorialShown = 0;
    settings->ShowCorrectWhenWrong = 1;
    settings->EnableMouseSupport = 1;
    settings->DarkMode = 1;
}

Settings* LoadSettings()
{
    FILE* file = fopen(SETTINGS_FILE, "r");

    Settings* settings = malloc(sizeof(Settings));

    if(file == NULL) {
        LoadDefaultSettings(settings);

        SaveSettings(settings);

        return settings;
    }

    fscanf(file, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d",
        &settings->CorrectAnswerColor, 
        &settings->WrongAnswerColor, 
        &settings->SelectedAnswerColor, 
        &settings->ConfirmedAnswerColor, 
        &settings->SupportColor, 
        &settings->FullUTF8Support, 
        &settings->TutorialShown,
        &settings->ShowCorrectWhenWrong,
        &settings->EnableMouseSupport,
        &settings->DarkMode);

    if(fclose(file) != 0) {
        perror("Failed to close file");
        ExitApp(EXIT_FAILURE);
    }

    return settings;
}

void SaveSettings(Settings *settings)
{
    FILE* file = fopen(SETTINGS_FILE, "w");

    if(file == NULL) {
        perror("Failed to open file");
        ExitApp(EXIT_FAILURE);
    }

    fprintf(file, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d",
        settings->CorrectAnswerColor, 
        settings->WrongAnswerColor, 
        settings->SelectedAnswerColor, 
        settings->ConfirmedAnswerColor, 
        settings->SupportColor,
        settings->FullUTF8Support, 
        settings->TutorialShown,
        settings->ShowCorrectWhenWrong,
        settings->EnableMouseSupport,
        settings->DarkMode);

    if(fclose(file) != 0) {
        perror("Failed to close file");
        ExitApp(EXIT_FAILURE);
    }
}

void DestroySettings(Settings *settings)
{
    free(settings);
}
