#include "Settings.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "AnsiHelper.h"
#include "IOHelper.h"
#include "RGBColors.h"

#define SETTINGS_FILE "./settings.txt"

/// @brief The loaded settings
Settings LoadedSettings;

static void LoadDefaultSettings()
{
    LoadedSettings.CorrectAnswerColor = RGB_ID_GREEN;
    LoadedSettings.WrongAnswerColor = RGB_ID_RED;
    LoadedSettings.SelectedAnswerColor = RGB_ID_YELLOW;
    LoadedSettings.ConfirmedAnswerColor = RGB_ID_CYAN;
    LoadedSettings.SupportColor = RGB_ID_MAGENTA;
    LoadedSettings.FullUTF8Support = 1;
    LoadedSettings.WelcomePageShown = 0;
    LoadedSettings.ShowCorrectWhenWrong = 1;
    LoadedSettings.EnableMouseSupport = 1;
    LoadedSettings.DarkMode = 1;
}

void LoadSettingsFromFile()
{
    FILE* file = fopen(SETTINGS_FILE, "r");

    if(file == NULL)
    {
        LoadDefaultSettings();

        SaveSettingsToFile();

        return;
    }

    fscanf(file, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d",
           &LoadedSettings.CorrectAnswerColor,
           &LoadedSettings.WrongAnswerColor,
           &LoadedSettings.SelectedAnswerColor,
           &LoadedSettings.ConfirmedAnswerColor,
           &LoadedSettings.SupportColor,
           &LoadedSettings.FullUTF8Support,
           &LoadedSettings.WelcomePageShown,
           &LoadedSettings.ShowCorrectWhenWrong,
           &LoadedSettings.EnableMouseSupport,
           &LoadedSettings.DarkMode);

    CloseFileChecked(file);
}

void SaveSettingsToFile()
{
    OpenFileChecked(file, SETTINGS_FILE, "w");

    fprintf(file, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d",
            LoadedSettings.CorrectAnswerColor,
            LoadedSettings.WrongAnswerColor,
            LoadedSettings.SelectedAnswerColor,
            LoadedSettings.ConfirmedAnswerColor,
            LoadedSettings.SupportColor,
            LoadedSettings.FullUTF8Support,
            LoadedSettings.WelcomePageShown,
            LoadedSettings.ShowCorrectWhenWrong,
            LoadedSettings.EnableMouseSupport,
            LoadedSettings.DarkMode);

    CloseFileChecked(file);
}
