#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <common/macros.h>
#include <common/stringutil.h>

#include <container/list.h>

#include <filesystem/common.h>
#include <filesystem/filelist.h>
#include <filesystem/path.h>

#ifdef OS_WINDOWS
#	include <windows.h>
#endif

#ifdef OS_LINUX
#	include <dirent.h>
#endif

struct xFile_t {
	char acFilePath[MAX_PATH_SIZE];
	char acFileName[MAX_PATH_SIZE];
	char acStem[MAX_PATH_SIZE];
	char acExt[MAX_PATH_SIZE];
	bool bIsDirectory;
};

struct xList_t* FileList_Alloc(const char* pcFilePath) {
	struct xList_t* pxList = List_Alloc(sizeof(struct xFile_t));

#ifdef OS_WINDOWS
	uint64_t wNormFilePathLength;
	char* pcNormFilePath = Path_Normalize(pcFilePath, &wNormFilePathLength, 1);
	pcNormFilePath[wNormFilePathLength - 1] = '*';

	WIN32_FIND_DATA xFindData;
	HANDLE hFile = FindFirstFile(pcNormFilePath, &xFindData);

	struct xFile_t xFile;

	do {
		memset(&xFile, 0, sizeof(xFile));

		GetFullPathName(xFindData.cFileName, sizeof(xFile.acFilePath), xFile.acFilePath, 0);

		StringUtil_ReplaceChar(xFile.acFilePath, '\\', '/');

		int32_t nFileNameLength = strlen(xFile.acFilePath);
		nFileNameLength = MIN(nFileNameLength, MAX_PATH_SIZE);

		memcpy(xFile.acFileName, xFindData.cFileName, MIN(nFileNameLength, MAX_PATH_SIZE));
		xFile.acFileName[MAX_PATH_SIZE - 1] = 0;

		bool bIsDirectory = xFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

		int32_t nExtDotIndex = StringUtil_FindFirstOf(xFindData.cFileName, '.');
		if (bIsDirectory || (nExtDotIndex == -1)) {
			memcpy(xFile.acStem, xFindData.cFileName, nFileNameLength);

			xFile.acStem[MAX_PATH_SIZE - 1] = 0;			
		} else {
			memcpy(xFile.acStem, &xFindData.cFileName[0], nExtDotIndex);
			memcpy(xFile.acExt, &xFindData.cFileName[nExtDotIndex + 1], (nFileNameLength - nExtDotIndex - 1));

			xFile.acStem[MAX_PATH_SIZE - 1] = 0;
			xFile.acExt[MAX_PATH_SIZE - 1] = 0;
		}

		xFile.bIsDirectory = bIsDirectory;

		List_Add(pxList, &xFile);
	} while (FindNextFile(hFile, &xFindData) != 0);

	FindClose(hFile);

	free(pcNormFilePath);
#endif

#ifdef OS_LINUX
	char* pcNormFilePath = Path_Normalize(pcFilePath, 0, 0);

	DIR* pxDir = opendir(pcNormFilePath);
	struct dirent* pxEntry = readdir(pxDir);

	struct xFile_t xFile;

	while (pxEntry) {
		memset(&xFile, 0, sizeof(xFile));

		int32_t nFileNameLength = strlen(pxEntry->d_name);
		nFileNameLength = MIN(nFileNameLength, MAX_PATH_SIZE);

		memcpy(xFile.acFileName, pxEntry->d_name, nFileNameLength);
		xFile.acFileName[MAX_PATH_SIZE - 1] = 0;

		bool bIsDirectory = pxEntry->d_type == DT_DIR;

		int32_t nExtDotIndex = StringUtil_FindFirstOf(pxEntry->d_name, '.');
		if (bIsDirectory || (nExtDotIndex == -1)) {
			memcpy(xFile.acStem, pxEntry->d_name, nFileNameLength);

			xFile.acStem[MAX_PATH_SIZE - 1] = 0;			
		} else {
			memcpy(xFile.acStem, &pxEntry->d_name[0], nExtDotIndex);
			memcpy(xFile.acExt, &pxEntry->d_name[nExtDotIndex + 1], (nFileNameLength - nExtDotIndex - 1));

			xFile.acStem[MAX_PATH_SIZE - 1] = 0;
			xFile.acExt[MAX_PATH_SIZE - 1] = 0;
		}

		xFile.bIsDirectory = bIsDirectory;

		List_Add(pxList, &xFile);

		pxEntry = readdir(pxDir);
	}

	closedir(pxDir);

	free(pcNormFilePath);
#endif

	return pxList;
}

void FileList_Free(struct xList_t* pxList) {
	List_Free(pxList);
}

void* FileList_Begin(struct xList_t* pxList) {
	return List_Begin(pxList);
}

struct xFile_t* FileList_Value(void* pIter) {
	return List_Value(pIter);
}

void* FileList_Next(void* pIter) {
	return List_Next(pIter);
}

const char* FileList_FilePath(struct xFile_t* pxFile) {
	return pxFile->acFilePath;
}

const char* FileList_FileName(struct xFile_t* pxFile) {
	return pxFile->acFileName;
}

const char* FileList_FileStem(struct xFile_t* pxFile) {
	return pxFile->acStem;
}

const char* FileList_FileExt(struct xFile_t* pxFile) {
	return pxFile->acExt;
}

bool FileList_IsDirectory(struct xFile_t* pxFile) {
	return pxFile->bIsDirectory;
}
