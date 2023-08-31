#ifndef METEOR_FILEUTIL_H
#define METEOR_FILEUTIL_H

#include <stdint.h>

void FileUtil_ReadBinary(char** ppcData, uint64_t* pwSize, const char* pcFilePath);
void FileUtil_ReadText(char** ppcData, uint64_t* pwSize, const char* pcFilePath);

void FileUtil_WriteBinary(const char* pcData, uint64_t wSize, const char* pcFilePath);
void FileUtil_WriteText(const char* pcData, uint64_t wSize, const char* pcFilePath);

#endif
