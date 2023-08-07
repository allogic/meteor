#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "debug.h"
#include "macros.h"
#include "window.h"
#include "list.h"
#include "filelist.h"
#include "fileutl.h"
#include "vulkan.h"

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

/*
static void Shader_Test(void) {
	uint32_t nDisplayShader = Shader_AllocDisplay("../shaders/test.vert", "../shaders/test.frag");
	uint32_t nComputeShader = Shader_AllocCompute("../shaders/test.comp");

	Shader_Free(nComputeShader);
	Shader_Free(nDisplayShader);
}
*/

int32_t main(void) {

#ifdef DEBUG
	Debug_Alloc();
#endif

	List_Test();
	FileList_Test();

	struct xWindow_t* pxWindow = Window_Alloc(WINDOW_NAME, 800, 600);

	if (pxWindow) {

		if (Vulkan_Alloc(pxWindow)) {

			while (Window_ShouldNotClose(pxWindow)) {
				Window_PollEvents(pxWindow);
				Window_SwapBuffers(pxWindow);
			}

			Vulkan_Free();

		}

		Window_Free(pxWindow);
	}

#ifdef DEBUG
	Debug_Free();
#endif

	return 0;
}
