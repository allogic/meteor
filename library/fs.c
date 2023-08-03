#include <stdint.h>
#include <string.h>

#include "fs.h"
#include "macros.h"
#include "list.h"
#include "strutil.h"

#ifdef OS_WINDOWS
#	include <windows.h>
#endif

#define PATH_SIZE 0x400

struct xFile_t {
	char acFilePath[PATH_SIZE];
	char acFileName[PATH_SIZE];
	char acStem[PATH_SIZE];
	char acExt[32];
};

struct xList_t* Fs_Alloc(const char* pcFilePath) {
	struct xList_t* pxList = List_Alloc();

#ifdef OS_WINDOWS
	WIN32_FIND_DATA xFindData;
	HANDLE hFile = FindFirstFile(pcFilePath, &xFindData);
	do {
		if (xFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {

		} else {
			struct xFile_t xFile;

			memset(&xFile, 0, sizeof(xFile));

			GetFullPathName(xFindData.cFileName, sizeof(xFile.acFilePath), xFile.acFilePath, 0);

			StrUtil_Replace(xFile.acFilePath, '\\', '/');

			memcpy(xFile.acFileName, xFindData.cFileName, MIN(MAX_PATH, PATH_SIZE));

			List_Push(pxList, &xFile, sizeof(xFile));
		}
	} while (FindNextFile(hFile, &xFindData) != 0);
	FindClose(hFile);
#endif

	return pxList;
}

void Fs_Free(struct xList_t* pxList) {
	List_Free(pxList);
}

struct xFile_t* Fs_Begin(struct xList_t* pxList) {
	return List_Begin(pxList);
}

struct xFile_t* Fs_Next(struct xList_t* pxList) {
	return List_Next(pxList);
}

const char* Fs_FilePath(struct xFile_t* pxFile) {
	return pxFile->acFilePath;
}

const char* Fs_FileName(struct xFile_t* pxFile) {
	return pxFile->acFileName;
}

const char* Fs_FileStem(struct xFile_t* pxFile) {
	return pxFile->acStem;
}

const char* Fs_FileExt(struct xFile_t* pxFile) {
	return pxFile->acExt;
}
