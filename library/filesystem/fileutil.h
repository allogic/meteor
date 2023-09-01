#ifndef METEOR_FILEUTIL_H
#define METEOR_FILEUTIL_H

#include <stdint.h>

void FileUtil_ReadBinary(uint8_t** ppcData, uint64_t* pwSize, const char* pcFilePath);
void FileUtil_ReadText(int8_t** ppcData, uint64_t* pwSize, const char* pcFilePath);
void FileUtil_ReadBmp(uint8_t** ppcData, uint64_t* pwSize, uint32_t* pnWidth, uint32_t* pnHeight, const char* pcFilePath);

void FileUtil_WriteBinary(uint8_t* pcData, uint64_t wSize, const char* pcFilePath);
void FileUtil_WriteText(int8_t* pcData, uint64_t wSize, const char* pcFilePath);

#endif
