#ifndef FILEUTL_H
#define FILEUTL_H

#include <stdint.h>

void FileUtl_ReadBinary(char** ppcData, uint32_t* pnSize, const char* pcFilePath);
void FileUtl_ReadText(char** ppcData, uint32_t* pnSize, const char* pcFilePath);

void FileUtl_WriteBinary(const char* pcData, uint32_t nSize, const char* pcFilePath);
void FileUtl_WriteText(const char* pcData, uint32_t nSize, const char* pcFilePath);

#endif
