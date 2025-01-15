#ifndef _INC_SETTINGS_H
#define _INC_SETTINGS_H

/// @brief Settings structure
typedef struct
{
    /// @brief Color id for correct answers
    int CorrectAnswerColor;
    /// @brief Color id for wrong answers
    int WrongAnswerColor;
    /// @brief Color id for selected answers
    int SelectedAnswerColor;
    /// @brief Color id for confirmed answers
    int ConfirmedAnswerColor;
    /// @brief Color id for misc things
    int SupportColor;
    /// @brief If 1, full support for UTF8 is enabled
    int FullUTF8Support;
    /// @brief If 1, the welcome page was already shown
    int WelcomePageShown;
    /// @brief If 1, correct answers will be shown when wrong
    int ShowCorrectWhenWrong;
    /// @brief If 1, mouse support is enabled
    int EnableMouseSupport;
    /// @brief If 1, dark mode is enabled
    int DarkMode;
} Settings;

/// @brief Load settings from file to global variable
void LoadSettingsFromFile();
/// @brief Save settings from global variable to file
void SaveSettingsToFile();

#endif // _INC_SETTINGS_H
