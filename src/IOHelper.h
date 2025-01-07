#ifndef _INC_IOHELPER_H
#define _INC_IOHELPER_H

#include <stdbool.h>

#define ESCAPE_CHAR (char)(224)

#undef getch
#undef kbhit

int getch();
int kbhit();

int WaitForAnyInput();

void IOLoop();

void InitializeIO();
void ExitApp(int exitCode) __attribute__((noreturn));

void SetThisConsoleTitle(const char* title);

void EnableMouseInput(bool enable);

#define MOUSE_LEFT_BUTTON 1
#define MOUSE_RIGHT_BUTTON 2

void SetMouseHandler(
    void (*clickHandler)(int, int, int, void *),
    void (*doubleClickHandler)(int, int, int, void *),
    void (*scrollHandler)(bool, int, int, void *),
    void (*moveHandler)(int, int, void *),
    void *data);
void UnsetMouseHandler();

void SetResizeHandler(void (*handler)(int, int, void*), void* data);
void UnsetResizeHandler();

#endif // _INC_IOHELPER_H
