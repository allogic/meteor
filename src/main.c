#include <stdint.h>

#include "window.h"
#include "glad.h"

#include <windows.h>

int32_t main(void) {
    Window_AllocWindows("Nippon");
    while (1) {
        Window_PollEventsWindows();
        glClearColor(1.0F, 0.0F, 0.0F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT);
        glFlush();
        Window_SwapBuffers();
    }
    Window_FreeWindows();
    return 0;
}
