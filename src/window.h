#ifndef WINDOW_H
#define WINDOW_H

#include <stdint.h>

void Window_AllocWindows(const char* pcWindowTitle);
void Window_PollEventsWindows(void);
void Window_SwapBuffers(void);
void Window_FreeWindows(void);

#endif
