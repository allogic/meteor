#ifndef WINDOW_H
#define WINDOW_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

int32_t Window_Alloc(const char* pcWindowTitle);
void Window_PollEvents(void);
void Window_SwapBuffers(void);
void Window_Free(void);

#endif
