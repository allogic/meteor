#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <common/macros.h>
#include <common/stringutil.h>

#include <container/list.h>
#include <container/vector.h>
#include <container/queue.h>

#include <debug/stacktrace.h>

#include <filesystem/filelist.h>
#include <filesystem/fileutil.h>
#include <filesystem/path.h>

#define PRINT_TEST_BEGIN(NAME) \
printf("///////////////////////////////////////////\n"); \
printf("// %s\n", #NAME); \
printf("///////////////////////////////////////////\n"); \
printf("\n");

static void List_Test(void) {
	PRINT_TEST_BEGIN(List_Test);

	{
		struct xItem_t {
			uint32_t nIndex;
			void* pIter;
		};

		struct xList_t* pxList = List_Alloc(sizeof(struct xItem_t));

		{
			printf("Insert\n");
			for (uint32_t i = 0; i < 1; ++i) {
				void* pIter = List_Push(pxList, 0);
				struct xItem_t* pxItem = (struct xItem_t*)List_Value(pIter);
				pxItem->pIter = pIter;
				printf("%u\n", i);
			}
			printf("\n");
		}

		void* pIter0 = List_Push(pxList, 0);
		struct xItem_t* pxItem0 = (struct xItem_t*)List_Value(pIter0);
		pxItem0->pIter = pIter0;

		void* pIter1 = List_Push(pxList, 0);
		struct xItem_t* pxItem1 = (struct xItem_t*)List_Value(pIter1);
		pxItem1->pIter = pIter1;

		void* pIter2 = List_Push(pxList, 0);
		struct xItem_t* pxItem2 = (struct xItem_t*)List_Value(pIter2);
		pxItem2->pIter = pIter2;

		List_Remove(pxList, pIter0);
		List_Remove(pxList, pIter1);
		List_Remove(pxList, pIter2);

		{
			printf("Iterate\n");
			void* pIter = List_Begin(pxList);
			while (pIter) {
				struct xItem_t* pxItem = (struct xItem_t*)List_Value(pIter);
				printf("%u %p\n", pxItem->nIndex, pxItem->pIter);
				pIter = List_Next(pIter);
			}
			printf("\n");
		}

		List_Free(pxList);
	}
}

static void Vector_Test(void) {
	PRINT_TEST_BEGIN(Vector_Test);

	struct xVector_t* pxVector = Vector_Alloc(sizeof(uint32_t));

	printf("Insert\n");
	for (uint32_t i = 0; i < 33; ++i) {
		Vector_Push(pxVector, &i);
		printf("%u\n", i);
	}
	printf("\n");

	printf("Iterate\n");
	uint32_t* pnData = Vector_Data(pxVector);
	for (uint32_t i = 0; i < Vector_Count(pxVector); ++i) {
		printf("%u\n", pnData[i]);
	}
	printf("\n");

	printf("Resize\n");
	printf("X == %u\n", Vector_Count(pxVector));
	Vector_Resize(pxVector, 24);
	printf("24 == %u\n", Vector_Count(pxVector));
	Vector_Resize(pxVector, 255);
	printf("255 == %u\n", Vector_Count(pxVector));
	Vector_Resize(pxVector, 17);
	printf("17 == %u\n", Vector_Count(pxVector));
	Vector_Resize(pxVector, 16);
	printf("16 == %u\n", Vector_Count(pxVector));
	Vector_Resize(pxVector, 15);
	printf("15 == %u\n", Vector_Count(pxVector));
	Vector_Resize(pxVector, 1);
	printf("1 == %u\n", Vector_Count(pxVector));
	printf("\n");

	Vector_Free(pxVector);
}

static void Queue_Test(void) {
	PRINT_TEST_BEGIN(Queue_Test);

	struct xQueue_t* pxQueue = Queue_Alloc(sizeof(uint32_t));

	printf("Enqueue\n");
	for (uint32_t i = 0; i < 8; ++i) {
		Queue_Push(pxQueue, &i);
		printf("%u\n", i);
	}
	printf("\n");

	printf("Dequeue\n");
	while (!Queue_Empty(pxQueue)) {
		uint32_t i;
		Queue_Pop(pxQueue, &i);
		printf("%u\n", i);
	}
	printf("\n");

	printf("Enqueue\n");
	for (uint32_t i = 0; i < 33; ++i) {
		Queue_Push(pxQueue, &i);
		printf("%u\n", i);
	}
	printf("\n");

	printf("Dequeue\n");
	while (!Queue_Empty(pxQueue)) {
		uint32_t i;
		Queue_Pop(pxQueue, &i);
		printf("%u\n", i);
	}
	printf("\n");

	Queue_Free(pxQueue);
}

static void FileList_Test(void) {
	PRINT_TEST_BEGIN(FileList_Test);

	struct xList_t* pxList = FileList_Alloc(".");

	void* pIter = FileList_Begin(pxList);
	while (pIter) {
		struct xFile_t* pxFile = List_Value(pIter);

		printf("FilePath:%s\n", FileList_FilePath(pxFile));
		printf("FileName:%s\n", FileList_FileName(pxFile));
		printf("FileStem:%s\n", FileList_FileStem(pxFile));
		printf("FileExt:%s\n", FileList_FileExt(pxFile));
		printf("FileExt:%u\n", FileList_IsDirectory(pxFile));
		printf("\n");

		pIter = FileList_Next(pIter);
	}

	FileList_Free(pxList);
}

static void StringUtil_Test(void) {
	PRINT_TEST_BEGIN(StringUtil_Test);

	uint64_t wLength;

	char* pcPath0 = Path_Normalize(".", 0, 0);
	char* pcPath1 = Path_Normalize("..", 0, 0);
	char* pcPath2 = Path_Normalize("./", 0, 0);
	char* pcPath3 = Path_Normalize("../", 0, 0);
	char* pcPath4 = Path_Normalize(".\\test", 0, 0);
	char* pcPath5 = Path_Normalize("..\\\\test.exe", 0, 0);
	char* pcPath6 = Path_Normalize("..\\", &wLength, 1);

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
	//Vector_Test();
	//Queue_Test();
	//FileList_Test();
	//StringUtil_Test();
	
#ifdef DEBUG
	StackTrace_Free();
#endif

	return 0;
}
