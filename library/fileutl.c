#include <stdio.h>
#include <stdlib.h>

#include <fileutl.h>

void FileUtl_ReadBinary(char** ppcData, uint32_t* pnSize, const char* pcFilePath) {
    FILE* pxFile = fopen(pcFilePath, "rb");

    fseek(pxFile, 0, SEEK_END);
    *pnSize = (uint32_t)ftell(pxFile);
    *ppcData = calloc(*pnSize, sizeof(char));
    fseek(pxFile, 0, SEEK_SET);

    fread(*ppcData, sizeof(char), *pnSize, pxFile);

    fclose(pxFile);
}

void FileUtl_ReadText(char** ppcData, uint32_t* pnSize, const char* pcFilePath) {
    FILE* pxFile = fopen(pcFilePath, "r");

    fseek(pxFile, 0, SEEK_END);
    *pnSize = (uint32_t)ftell(pxFile);
    *ppcData = calloc((*pnSize) + 1, sizeof(char));
    fseek(pxFile, 0, SEEK_SET);

    fread(*ppcData, sizeof(char), *pnSize, pxFile);

    fclose(pxFile);
}

void FileUtl_WriteBinary(const char* pcData, uint32_t nSize, const char* pcFilePath) {
    FILE* pxFile = fopen(pcFilePath, "wb");

    fseek(pxFile, 0, SEEK_SET);

    fwrite(pcData, sizeof(char), nSize, pxFile);

    fclose(pxFile);
}

void FileUtl_WriteText(const char* pcData, uint32_t nSize, const char* pcFilePath) {
    FILE* pxFile = fopen(pcFilePath, "w");

    fseek(pxFile, 0, SEEK_SET);

    fwrite(pcData, sizeof(char), nSize, pxFile);

    fclose(pxFile);
}
