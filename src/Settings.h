#ifndef _INC_SETTINGS_H
#define _INC_SETTINGS_H

typedef struct {
    int CorrectAnswerColor;
    int WrongAnswerColor;
    int SelectedAnswerColor;
    int ConfirmedAnswerColor;
    int SupportColor;
    int FullUTF8Support;
    int TutorialShown;
    int ShowCorrectWhenWrong;
    int EnableMouseSupport;
    int DarkMode;
} Settings;

void LoadSettingsFromFile();
void SaveSettingsToFile();

#endif // _INC_SETTINGS_H
