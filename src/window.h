#ifndef WINDOW_H
#define WINDOW_H

#include <stdint.h>
#include <stdbool.h>

int32_t Window_Alloc(const char* pcWindowTitle, uint32_t nWidth, uint32_t nHeight);
bool Window_ShouldClose(void);
void Window_PollEvents(void);
void Window_SwapBuffers(void);
void Window_Free(void);

#endif
