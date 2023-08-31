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

#ifdef OS_WINDOWS
#pragma pack(push, 1)
	struct xBmpHeader_t {
		int16_t sSignature;       // File signature ('B' and 'M')
		int32_t nFileSize;        // File size in bytes
		int32_t nReserved;        // Reserved, should be set to 0
		int32_t nDataOffset;      // Offset to the beginning of image data
		int32_t nHeaderSize;      // Size of the header (40 bytes for BMP)
		int32_t nWidth;           // Image width in pixels
		int32_t nHeight;          // Image height in pixels
		int16_t sPlanes;          // Number of color planes (1)
		int16_t sBitsPerPixel;    // Number of bits per pixel (usually 24 for RGB)
		int32_t nCompression;     // Compression method (0 for no compression)
		int32_t nDataSize;        // Size of the raw image data
		int32_t nHorizontalRes;   // Horizontal resolution (pixels per meter)
		int32_t nVerticalRes;     // Vertical resolution (pixels per meter)
		int32_t nColors;          // Number of colors in the palette (0 for no palette)
		int32_t nImportantColors; // Number of important colors (0 when every color is important)
	};
#pragma pack(pop)
#endif

#ifdef OS_LINUX
	__attribute__((packed)) struct xBmpHeader_t {
		int16_t sSignature;       // File signature ('B' and 'M')
		int32_t nFileSize;        // File size in bytes
		int32_t nReserved;        // Reserved, should be set to 0
		int32_t nDataOffset;      // Offset to the beginning of image data
		int32_t nHeaderSize;      // Size of the header (40 bytes for BMP)
		int32_t nWidth;           // Image width in pixels
		int32_t nHeight;          // Image height in pixels
		int16_t sPlanes;          // Number of color planes (1)
		int16_t sBitsPerPixel;    // Number of bits per pixel (usually 24 for RGB)
		int32_t nCompression;     // Compression method (0 for no compression)
		int32_t nDataSize;        // Size of the raw image data
		int32_t nHorizontalRes;   // Horizontal resolution (pixels per meter)
		int32_t nVerticalRes;     // Vertical resolution (pixels per meter)
		int32_t nColors;          // Number of colors in the palette (0 for no palette)
		int32_t nImportantColors; // Number of important colors (0 when every color is important)
	};
#endif

	struct xBmpHeader_t xBmpHeader;

	fread(&xBmpHeader, sizeof(xBmpHeader), 1, pxFile);
	*pwSize = xBmpHeader.nDataSize;
	*pnWidth = xBmpHeader.nWidth;
	*pnHeight = xBmpHeader.nHeight;
	*ppcData = calloc(*pwSize, sizeof(char));
	fread(*ppcData, sizeof(char), *pwSize, pxFile);

	printf("Width:%u Height:%u Size:%llu\n", *pnWidth, *pnHeight, *pwSize);

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
