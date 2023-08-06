#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "debug.h"
#include "macros.h"
#include "window.h"
#include "glad.h"
#include "list.h"
#include "filelist.h"
#include "fileutl.h"

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

static void FileList_Test(void) {
	struct xList_t* pxList = FileList_Alloc("./");

	struct xFile_t* pxFile = FileList_Begin(pxList);
	while (pxFile) {
		printf("FilePath:%s\n", FileList_FilePath(pxFile));
		printf("FileName:%s\n", FileList_FileName(pxFile));
		printf("FileStem:%s\n", FileList_FileStem(pxFile));
		printf("FileExt:%s\n", FileList_FileExt(pxFile));

		pxFile = FileList_Next(pxList);
	}

	FileList_Free(pxList);
}

int32_t main(void) {

#ifdef DEBUG
	Debug_Alloc();
#endif

	List_Test();
	FileList_Test();

/*
	<<<<<<<<<<<< STACK TRACE >>>>>>>>>>>>

	main + 0x1D
	invoke_main + 0x39
	__scrt_common_main_seh + 0x12E
	__scrt_common_main + 0xE
*/

	*(uint32_t*)0 = 42;

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

#ifdef DEBUG
	Debug_Free();
#endif

	return 0;
}
