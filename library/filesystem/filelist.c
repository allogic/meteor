#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <common/macros.h>
#include <common/stringutil.h>

#include <container/list.h>

#include <filesystem/fs.h>
#include <filesystem/filelist.h>

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
	char acExt[32];
	bool bIsDirectory;
};

struct xList_t* FileList_Alloc(const char* pcFilePath) {
	struct xList_t* pxList = List_Alloc(sizeof(struct xFile_t), 16);

#ifdef OS_WINDOWS
	uint64_t wNormFilePathLength;
	char* pcNormFilePath = StringUtil_NormalizePath(pcFilePath, &wNormFilePathLength, 1);
	pcNormFilePath[wNormFilePathLength - 1] = '*';

	WIN32_FIND_DATA xFindData;
	HANDLE hFile = FindFirstFile(pcNormFilePath, &xFindData);

	do {
		struct xFile_t xFile;

		memset(&xFile, 0, sizeof(xFile));

		GetFullPathName(xFindData.cFileName, sizeof(xFile.acFilePath), xFile.acFilePath, 0);

		StringUtil_ReplaceChar(xFile.acFilePath, '\\', '/');

		memcpy(xFile.acFileName, xFindData.cFileName, MIN(MAX_PATH, MAX_PATH_SIZE));

		xFile.bIsDirectory = xFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

		List_Push(pxList, &xFile);
	} while (FindNextFile(hFile, &xFindData) != 0);

	FindClose(hFile);

	free(pcNormFilePath);
#endif

#ifdef OS_LINUX
	char* pcNormFilePath = StringUtil_NormalizePath(pcFilePath, 0, 0);

	DIR* pxDir = opendir(pcNormFilePath);
	struct dirent* pxEntry = readdir(pxDir);

	while (pxEntry) {
		struct xFile_t xFile;

		memset(&xFile, 0, sizeof(xFile));

		uint32_t nFileNameLength = strlen(pxEntry->d_name);

		memcpy(xFile.acFileName, pxEntry->d_name, MIN(nFileNameLength, MAX_PATH_SIZE));

		xFile.bIsDirectory = pxEntry->d_type == DT_DIR;

		List_Push(pxList, &xFile);

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

struct xFile_t* FileList_Begin(struct xList_t* pxList) {
	return List_Begin(pxList);
}

struct xFile_t* FileList_Next(struct xList_t* pxList) {
	return List_Next(pxList);
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
