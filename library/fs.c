#define PATH_SIZE 0x400

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "fs.h"
#include "macros.h"
#include "list.h"
#include "strutl.h"

#ifdef OS_WINDOWS
#	include <windows.h>
#endif

#ifdef OS_LINUX
#	include <dirent.h>
#endif

struct xFile_t {
	char acFilePath[PATH_SIZE];
	char acFileName[PATH_SIZE];
	char acStem[PATH_SIZE];
	char acExt[32];
};

struct xList_t* Fs_Alloc(const char* pcFilePath) {
	struct xList_t* pxList = List_Alloc();

#ifdef OS_WINDOWS
	char acFilePath[PATH_SIZE];
	uint32_t nFilePathLength = strlen(pcFilePath);
	memset(acFilePath, 0, PATH_SIZE);
	memcpy(acFilePath, pcFilePath, nFilePathLength);
	acFilePath[nFilePathLength] = '*';

	WIN32_FIND_DATA xFindData;
	HANDLE hFile = FindFirstFile(acFilePath, &xFindData);
	do {
		if (xFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {

		} else {
			struct xFile_t xFile;

			memset(&xFile, 0, sizeof(xFile));

			GetFullPathName(xFindData.cFileName, sizeof(xFile.acFilePath), xFile.acFilePath, 0);

			StrUtl_Replace(xFile.acFilePath, '\\', '/');

			memcpy(xFile.acFileName, xFindData.cFileName, MIN(MAX_PATH, PATH_SIZE));

			List_Push(pxList, &xFile, sizeof(xFile));
		}
	} while (FindNextFile(hFile, &xFindData) != 0);
	FindClose(hFile);
#endif

#ifdef OS_LINUX
	DIR* pxDir = opendir(pcFilePath);
	struct dirent* pxEntry;
	if (pxDir) {
		pxEntry = readdir(pxDir);
		while (pxEntry) {
			if (strcmp(pxEntry->d_name, ".") == 0) {

			} else if (strcmp(pxEntry->d_name, "..") == 0) {
				
			} else {
				struct xFile_t xFile;

				memset(&xFile, 0, sizeof(xFile));

				uint32_t nFileNameLength = strlen(pxEntry->d_name);

				memcpy(xFile.acFileName, pxEntry->d_name, MIN(nFileNameLength, PATH_SIZE));

				List_Push(pxList, &xFile, sizeof(xFile));
			}
			pxEntry = readdir(pxDir);
		}
		closedir(pxDir);
	}
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
