#include <stdlib.h>
#include <stdio.h>
#include "MainMenuPage.h"
#include "AnsiHelper.Windows.h"
#include "Settings.h"

Settings* LoadedSettings;

int main() {
    system("chcp 65001");
    EnsureAnsiSupport();

    LoadedSettings = LoadSettings();

    PageEnter_MainMenu();

    return 0;
}