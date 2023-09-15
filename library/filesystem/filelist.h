#ifndef METEOR_FILESYSTEM_FILELIST_H
#define METEOR_FILESYSTEM_FILELIST_H

#include <stdbool.h>

struct xList_t;
struct xFile_t;

struct xList_t* FileList_Alloc(const char* pcFilePath);
void FileList_Free(struct xList_t* pxList);

void* FileList_Begin(struct xList_t* pxList);
struct xFile_t* FileList_Value(void* pIter);
void* FileList_Next(void* pIter);

const char* FileList_FilePath(struct xFile_t* pxFile);
const char* FileList_FileName(struct xFile_t* pxFile);
const char* FileList_FileStem(struct xFile_t* pxFile);
const char* FileList_FileExt(struct xFile_t* pxFile);
bool FileList_IsDirectory(struct xFile_t* pxFile);

#endif
