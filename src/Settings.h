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
} Settings;

Settings* LoadSettings();
void SaveSettings(Settings* settings);
void DestroySettings(Settings *settings);

#endif // _INC_SETTINGS_H