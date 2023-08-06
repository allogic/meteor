#ifndef FILELIST_H
#define FILELIST_H

struct xList_t;
struct xFile_t;

struct xList_t* FileList_Alloc(const char* pcFilePath);
void FileList_Free(struct xList_t* pxList);

struct xFile_t* FileList_Begin(struct xList_t* pxList);
struct xFile_t* FileList_Next(struct xList_t* pxList);

const char* FileList_FilePath(struct xFile_t* pxFile);
const char* FileList_FileName(struct xFile_t* pxFile);
const char* FileList_FileStem(struct xFile_t* pxFile);
const char* FileList_FileExt(struct xFile_t* pxFile);

#endif
