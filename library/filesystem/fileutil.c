#include <stdio.h>
#include <stdlib.h>

#include <filesystem/fileutil.h>

void FileUtil_ReadBinary(uint8_t** ppcData, uint64_t* pwSize, const char* pcFilePath) {
	FILE* pxFile = fopen(pcFilePath, "rb");

	fseek(pxFile, 0, SEEK_END);
	*pwSize = ftell(pxFile);
	*ppcData = calloc(*pwSize, 1);
	fseek(pxFile, 0, SEEK_SET);

	fread(*ppcData, 1, *pwSize, pxFile);

	fclose(pxFile);
}

void FileUtil_ReadText(int8_t** ppcData, uint64_t* pwSize, const char* pcFilePath) {
	FILE* pxFile = fopen(pcFilePath, "r");

	fseek(pxFile, 0, SEEK_END);
	*pwSize = ftell(pxFile);
	*ppcData = calloc((*pwSize) + 1, 1);
	fseek(pxFile, 0, SEEK_SET);

	fread(*ppcData, 1, *pwSize, pxFile);

	fclose(pxFile);
}

void FileUtil_ReadBmp(uint8_t** ppcData, uint64_t* pwSize, uint32_t* pnWidth, uint32_t* pnHeight, const char* pcFilePath) {
	FILE* pxFile = fopen(pcFilePath, "rb");

#define BMP_HEADER_STRUCT { \
		uint16_t sSignature; \
		uint32_t nFileSize; \
		uint32_t nReserved; \
		uint32_t nDataOffset; \
		uint32_t nHeaderSize; \
		uint32_t nWidth; \
		uint32_t nHeight; \
		uint16_t sPlanes; \
		uint16_t sBitsPerPixel; \
		uint32_t nCompression; \
		uint32_t nDataSize; \
		uint32_t nHorizontalRes; \
		uint32_t nVerticalRes; \
		uint32_t nColors; \
		uint32_t nImportantColors; \
		uint32_t nRedMask; \
		uint32_t nGreenMask; \
		uint32_t nBlueMask; \
		uint32_t nAlphaMask; \
	}

#ifdef OS_WINDOWS
#pragma pack(push, 1)
	struct xBmpHeader_t BMP_HEADER_STRUCT;
#pragma pack(pop)
#endif

#ifdef OS_LINUX
	struct __attribute__((packed)) xBmpHeader_t BMP_HEADER_STRUCT;
#endif

	struct xBmpHeader_t xBmpHeader;

	fread(&xBmpHeader, sizeof(xBmpHeader), 1, pxFile);

	*pwSize = xBmpHeader.nDataSize;
	*pnWidth = xBmpHeader.nWidth;
	*pnHeight = xBmpHeader.nHeight;
	*ppcData = calloc(*pwSize, 1);

	fread(*ppcData, 1, *pwSize, pxFile);	

	fclose(pxFile);
}

void FileUtil_WriteBinary(uint8_t* pcData, uint64_t pwSize, const char* pcFilePath) {
	FILE* pxFile = fopen(pcFilePath, "wb");

	fseek(pxFile, 0, SEEK_SET);

	fwrite(pcData, 1, pwSize, pxFile);

	fclose(pxFile);
}

void FileUtil_WriteText(int8_t* pcData, uint64_t pwSize, const char* pcFilePath) {
	FILE* pxFile = fopen(pcFilePath, "w");

	fseek(pxFile, 0, SEEK_SET);

	fwrite(pcData, 1, pwSize, pxFile);

	fclose(pxFile);
}
