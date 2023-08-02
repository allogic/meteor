#ifndef FS_H
#define FS_H

struct xList_t;
struct xFile_t;

struct xList_t* Fs_Alloc(const char* pcFilePath);
void Fs_Free(struct xList_t* pxList);
struct xFile_t* Fs_Begin(struct xList_t* pxList);
struct xFile_t* Fs_Next(struct xList_t* pxList);
const char* Fs_FilePath(struct xFile_t* pxFile);
const char* Fs_FileName(struct xFile_t* pxFile);
const char* Fs_FileStem(struct xFile_t* pxFile);
const char* Fs_FileExt(struct xFile_t* pxFile);

#endif
