#include <stdio.h>
#include <stdlib.h>

#include <filesystem/fileutil.h>

void FileUtil_ReadBinary(char** ppcData, uint64_t* pwSize, const char* pcFilePath) {
    FILE* pxFile = fopen(pcFilePath, "rb");

    fseek(pxFile, 0, SEEK_END);
    *pwSize = ftell(pxFile);
    *ppcData = calloc(*pwSize, sizeof(char));
    fseek(pxFile, 0, SEEK_SET);

    fread(*ppcData, sizeof(char), *pwSize, pxFile);

    fclose(pxFile);
}

void FileUtil_ReadText(char** ppcData, uint64_t* pwSize, const char* pcFilePath) {
    FILE* pxFile = fopen(pcFilePath, "r");

    fseek(pxFile, 0, SEEK_END);
    *pwSize = ftell(pxFile);
    *ppcData = calloc((*pwSize) + 1, sizeof(char));
    fseek(pxFile, 0, SEEK_SET);

    fread(*ppcData, sizeof(char), *pwSize, pxFile);

    fclose(pxFile);
}

void FileUtil_WriteBinary(const char* pcData, uint64_t pwSize, const char* pcFilePath) {
    FILE* pxFile = fopen(pcFilePath, "wb");

    fseek(pxFile, 0, SEEK_SET);

    fwrite(pcData, sizeof(char), pwSize, pxFile);

    fclose(pxFile);
}

void FileUtil_WriteText(const char* pcData, uint64_t pwSize, const char* pcFilePath) {
    FILE* pxFile = fopen(pcFilePath, "w");

    fseek(pxFile, 0, SEEK_SET);

    fwrite(pcData, sizeof(char), pwSize, pxFile);

    fclose(pxFile);
}
