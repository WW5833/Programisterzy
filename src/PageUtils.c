#include "PageUtils.h"
#include "AnsiHelper.h"
#include "IOHelper.h"

KeyInputType HandleInteractions(bool blocking) {
    if(!blocking && !kbhit()) return KEY_NONE;

    char c = (char)getch();

    switch (c)
    {
        case ENTER:
            return KEY_ENTER;
        case '1':
            return KEY_1;
        case '2':
            return KEY_2;
        case '3':
            return KEY_3;
        
        case 'R':
            return KEY_R;

        case ESC:
            return KEY_ESCAPE;

        case ESCAPE_CHAR:
            switch (getch())
            {
                case 72: // Arrow Up
                    return KEY_ARROW_UP;
                case 80: // Arrow Down
                    return KEY_ARROW_DOWN;
                case 77: // Arrow Right
                    return KEY_ARROW_RIGHT;
                case 75: // Arrow Left
                    return KEY_ARROW_LEFT;

                default:
                    break;
            }
            break;

        default:
            break;
    }

    return KEY_NONE;
}

void WaitForEnter() {
    WaitForKeys(ENTER);
}

char _internal_WaitForKeys(int count, char* keys)
{
    int c;
    while(true) {
        c = WaitForAnyInput();

        for (int i = 0; i < count; i++)
        {
            if(keys[i] == ANY_MOUSE_BUTTON && c == INT_MAX) {
                return ANY_MOUSE_BUTTON;
            }
            else if(keys[i] == RESIZE_EVENT && c == INT_MAX - 1) {
                return RESIZE_EVENT;
            }
            else if(c == keys[i]) {
                return (char)c;
            }
        }
    }
}
