#include <stdlib.h>
#include <stdio.h>
#include "UIManager.h"
#include "AnsiHelper.Windows.h"

int main() {
    system("chcp 65001");
    EnsureAnsiSupport();

    UILoop_MainMenu();

    return 0;
}