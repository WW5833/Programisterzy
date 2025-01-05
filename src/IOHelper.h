#ifndef _INC_IOHELPER_H
#define _INC_IOHELPER_H

#define ESCAPE_CHAR (char)(224)

#undef getch
#undef kbhit

int getch();
int kbhit();

void IOLoop();

void InitializeIO();
void ExitApp(int exitCode) __attribute__((noreturn));

void SetResizeHandler(void (*handler)(int, int, void*), void* data);
void UnsetResizeHandler();

#endif // _INC_IOHELPER_H
