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

void FileUtil_ReadBmp(char** ppcData, uint64_t* pwSize, uint32_t* pnWidth, uint32_t* pnHeight, const char* pcFilePath) {
	FILE* pxFile = fopen(pcFilePath, "rb");

#define BMP_HEADER_STRUCT { \
		int16_t sSignature; \
		int32_t nFileSize; \
		int32_t nReserved; \
		int32_t nDataOffset; \
		int32_t nHeaderSize; \
		int32_t nWidth; \
		int32_t nHeight; \
		int16_t sPlanes; \
		int16_t sBitsPerPixel; \
		int32_t nCompression; \
		int32_t nDataSize; \
		int32_t nHorizontalRes; \
		int32_t nVerticalRes; \
		int32_t nColors; \
		int32_t nImportantColors; \
	}

#ifdef OS_WINDOWS
#pragma pack(push, 1)
	struct xBmpHeader_t BMP_HEADER_STRUCT;
#pragma pack(pop)
#endif

#ifdef OS_LINUX
	__attribute__((packed)) struct xBmpHeader_t BMP_HEADER_STRUCT;
#endif

	struct xBmpHeader_t xBmpHeader;

	fread(&xBmpHeader, sizeof(xBmpHeader), 1, pxFile);
	*pwSize = xBmpHeader.nDataSize;
	*pnWidth = xBmpHeader.nWidth;
	*pnHeight = xBmpHeader.nHeight;
	*ppcData = calloc(*pwSize, sizeof(char));
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
