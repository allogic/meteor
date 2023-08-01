#include "main.h"
#include "core.h"
#include "window.h"
#include "glad.h"
#include "list.h"

int32_t main(void) {
	struct xList_t* pxList = List_Alloc();

	for (uint32_t i = 0; i < 8; ++i) {
		List_Push(pxList, &i, sizeof(i));
	}

	List_Dump(pxList);

	if (Window_Alloc("Nippon") == 0) {
		while (1) {
			Window_PollEvents();
			printf("Poll\n");
			glClearColor(1.0F, 0.0F, 0.0F, 1.0F);
			glClear(GL_COLOR_BUFFER_BIT);
			glFlush();
			Window_SwapBuffers();
			printf("Swap\n");
		}
		Window_Free();
	}
	return 0;
}
