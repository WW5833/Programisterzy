#include <stdlib.h>
#include <stdio.h>
#include "UIManager.h"
#include "AnsiHelper.Windows.h"

int main() {
    EnsureAnsiSupport();

    UILoop_MainMenu();

    return 0;
}