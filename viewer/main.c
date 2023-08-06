#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "debug.h"
#include "macros.h"
#include "window.h"
#include "glad.h"
#include "list.h"
#include "fs.h"

static void List_Test(void) {
	struct xList_t* pxList = List_Alloc();

	for (int32_t i = 0; i < 8; ++i) {
		List_Push(pxList, &i, sizeof(i));
	}

	void* pData = List_Begin(pxList);
	while (pData) {
		printf("%d\n", *(int32_t*)pData);

		pData = List_Next(pxList);
	}

	List_Free(pxList);
}

static void Fs_Test(void) {
	struct xList_t* pxList = Fs_Alloc("./");

	struct xFile_t* pxFile = Fs_Begin(pxList);
	while (pxFile) {
		printf("FilePath:%s\n", Fs_FilePath(pxFile));
		printf("FileName:%s\n", Fs_FileName(pxFile));
		printf("FileStem:%s\n", Fs_FileStem(pxFile));
		printf("FileExt:%s\n", Fs_FileExt(pxFile));

		pxFile = Fs_Next(pxList);
	}

	Fs_Free(pxList);
}

int32_t main(void) {
	Debug_Alloc();

	List_Test();
	Fs_Test();

	struct xWindow_t* pxWindow = Window_Alloc(WINDOW_NAME, 800, 600);

	if (pxWindow) {
		gladLoadGL();

		while (Window_ShouldNotClose(pxWindow)) {
			Window_PollEvents(pxWindow);

			glClearColor(0.1F, 0.0F, 0.0F, 1.0F);
			glClear(GL_COLOR_BUFFER_BIT);
			glFlush();

			Window_SwapBuffers(pxWindow);
		}

		Window_Free(pxWindow);
	}

	Debug_Free();

	return 0;
}
