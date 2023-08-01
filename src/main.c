#include "main.h"
#include "core.h"
#include "window.h"
#include "glad.h"
#include "list.h"

int32_t main(void) {
	if (Window_Alloc("Nippon", 800, 600) == 0) {
		while (1) {
			Window_PollEvents();
			//glClearColor(1.0F, 0.0F, 0.0F, 1.0F);
			//glClear(GL_COLOR_BUFFER_BIT);
			//glFlush();
			Window_SwapBuffers();
		}
		Window_Free();
	}
	return 0;
}
