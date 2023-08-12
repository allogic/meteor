#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <stacktrace.h>
#include <macros.h>
#include <list.h>
#include <filelist.h>
#include <fileutl.h>
#include <strutl.h>

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

	printf("\n");

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
		printf("FileExt:%u\n", FileList_IsDirectory(pxFile));
		printf("\n");

		pxFile = FileList_Next(pxList);
	}

	FileList_Free(pxList);
}

static void StrUtl_Test(void) {
	uint32_t nLength;

	char* pcPath0 = StrUtl_NormalizePath(".", 0, 0);
	char* pcPath1 = StrUtl_NormalizePath("..", 0, 0);
	char* pcPath2 = StrUtl_NormalizePath("./", 0, 0);
	char* pcPath3 = StrUtl_NormalizePath("../", 0, 0);
	char* pcPath4 = StrUtl_NormalizePath(".\\test", 0, 0);
	char* pcPath5 = StrUtl_NormalizePath("..\\\\test.exe", 0, 0);
	char* pcPath6 = StrUtl_NormalizePath("..\\", &nLength, 1);

	pcPath6[nLength - 1] = '*';

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
	FileList_Test();
	StrUtl_Test();
	
#ifdef DEBUG
	StackTrace_Free();
#endif

	return 0;
}
