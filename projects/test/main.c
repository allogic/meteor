#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <common/macros.h>
#include <common/stringutil.h>

#include <debug/stacktrace.h>

#include <standard/list.h>
#include <standard/queue.h>

#include <filesystem/filelist.h>
#include <filesystem/fileutil.h>

#define PRINT_TEST_BEGIN(NAME) \
printf("///////////////////////////////////////////\n"); \
printf("// %s\n", #NAME); \
printf("///////////////////////////////////////////\n"); \
printf("\n");

static void List_Test(void) {
	PRINT_TEST_BEGIN(List_Test);

	struct xList_t* pxList = List_Alloc();

	for (int32_t i = 0; i < 8; ++i) {
		List_Push(pxList, &i, sizeof(i));
	}

	void* pData = List_Begin(pxList);
	while (pData) {
		printf("%d\n", *(int32_t*)pData);

		pData = List_Next(pxList);
	}

	printf("\n");

	List_Free(pxList);
}

static void Queue_Test(void) {
	PRINT_TEST_BEGIN(Queue_Test);

	struct xQueue_t* pxQueue = Queue_Alloc(sizeof(uint32_t), 4);

	for (uint32_t i = 0; i < 8; ++i) {
		printf("Push:%u ReadIndex:%u WriteIndex:%u ReadOffset:%u WriteOffset:%u\n",
			i,
			Queue_GetReadIndex(pxQueue),
			Queue_GetWriteIndex(pxQueue),
			Queue_GetReadOffset(pxQueue),
			Queue_GetWriteOffset(pxQueue));
		Queue_Push(pxQueue, &i);
	}

	printf("\n");

	Queue_Dump(pxQueue);

	printf("\n");

	while (!Queue_Empty(pxQueue)) {
		uint32_t i;
		Queue_Pop(pxQueue, &i);
		printf("Pop:%u\n", i);
	}

	printf("\n");
}

static void FileList_Test(void) {
	PRINT_TEST_BEGIN(FileList_Test);

	struct xList_t* pxList = FileList_Alloc("./");

	struct xFile_t* pxFile = FileList_Begin(pxList);
	while (pxFile) {
		printf("FilePath:%s\n", FileList_FilePath(pxFile));
		printf("FileName:%s\n", FileList_FileName(pxFile));
		printf("FileStem:%s\n", FileList_FileStem(pxFile));
		printf("FileExt:%s\n", FileList_FileExt(pxFile));
		printf("FileExt:%u\n", FileList_IsDirectory(pxFile));
		printf("\n");

		pxFile = FileList_Next(pxList);
	}

	FileList_Free(pxList);
}

static void StringUtil_Test(void) {
	PRINT_TEST_BEGIN(StringUtil_Test);

	uint64_t wLength;

	char* pcPath0 = StringUtil_NormalizePath(".", 0, 0);
	char* pcPath1 = StringUtil_NormalizePath("..", 0, 0);
	char* pcPath2 = StringUtil_NormalizePath("./", 0, 0);
	char* pcPath3 = StringUtil_NormalizePath("../", 0, 0);
	char* pcPath4 = StringUtil_NormalizePath(".\\test", 0, 0);
	char* pcPath5 = StringUtil_NormalizePath("..\\\\test.exe", 0, 0);
	char* pcPath6 = StringUtil_NormalizePath("..\\", &wLength, 1);

	pcPath6[wLength - 1] = '*';

	printf("%s\n", pcPath0);
	printf("%s\n", pcPath1);
	printf("%s\n", pcPath2);
	printf("%s\n", pcPath3);
	printf("%s\n", pcPath4);
	printf("%s\n", pcPath5);
	printf("%s\n", pcPath6);

	printf("\n");

	free(pcPath0);
	free(pcPath1);
	free(pcPath2);
	free(pcPath3);
	free(pcPath4);
	free(pcPath5);
	free(pcPath6);
}

int32_t main(void) {

#ifdef DEBUG
	StackTrace_Alloc();
#endif

	List_Test();
	Queue_Test();
	FileList_Test();
	StringUtil_Test();
	
#ifdef DEBUG
	StackTrace_Free();
#endif

	return 0;
}
