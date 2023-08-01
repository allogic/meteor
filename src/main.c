#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "core.h"
#include "window.h"
#include "glad.h"
#include "list.h"

int32_t main(void) {
	const char acWindowTitle[256] = "Nippon";

	/*
	memcpy((void*)(acWindowTitle + strlen(acWindowTitle)), (void*)VER_MAJOR, strlen(VER_MAJOR));
	memcpy((void*)(acWindowTitle + strlen(acWindowTitle)), (void*)".", 1);
	memcpy((void*)(acWindowTitle + strlen(acWindowTitle)), (void*)VER_MINOR, strlen(VER_MINOR));
	memcpy((void*)(acWindowTitle + strlen(acWindowTitle)), (void*)".", 1);
	memcpy((void*)(acWindowTitle + strlen(acWindowTitle)), (void*)VER_PATCH, strlen(VER_PATCH));
	memcpy((void*)(acWindowTitle + strlen(acWindowTitle)), (void*)" HEAD ", 6);
	memcpy((void*)(acWindowTitle + strlen(acWindowTitle)), (void*)GIT_COMMIT_HASH, strlen(GIT_COMMIT_HASH));
	*/

	if (Window_Alloc(acWindowTitle, 800, 600) == 0) {

		while (Window_ShouldClose() == 0) {
			Window_PollEvents();

			glClearColor(0.1F, 0.0F, 0.0F, 1.0F);
			glClear(GL_COLOR_BUFFER_BIT);
			glFlush();

			Window_SwapBuffers();
		}

		Window_Free();
	}

	return 0;
}
