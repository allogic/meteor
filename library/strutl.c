#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "fs.h"
#include "macros.h"
#include "strutl.h"

void StrUtl_ReplaceChar(char* pcSubject, char cFrom, char cTo) {
	while (*pcSubject++) {
		if (*pcSubject == cFrom) {
			*pcSubject = cTo;
		}
	}
}

char* StrUtl_NormalizePath(const char* pcFilePath, uint32_t* pnFilePathLength, uint32_t nScratch) {
	char acBuffer[MAX_PATH_SIZE];

	memset(acBuffer, 0, MAX_PATH_SIZE);

	uint32_t nFilePathLength = strlen(pcFilePath);

	for (uint32_t i = 0; i < nFilePathLength; ++i) {
		if (pcFilePath[i] == '\\') {
			acBuffer[i] = '/';
		} else {
			acBuffer[i] = pcFilePath[i];
		}
	}

	if (acBuffer[nFilePathLength - 1] != '/') {
		acBuffer[nFilePathLength] = '/';
	}

	uint32_t nNormFilePathLength = strlen(acBuffer);

	if (pnFilePathLength) {
		*pnFilePathLength = nNormFilePathLength + nScratch;
	}

	char* pcNormFilePath = (char*)calloc(1, nNormFilePathLength + nScratch + 1);
	memcpy(pcNormFilePath, acBuffer, nNormFilePathLength);

	return pcNormFilePath;
}

char* StrUtl_JoinPath(const char* pcFirstPath, const char* pcSecondPath) {
	UNUSED(pcFirstPath);
	UNUSED(pcSecondPath);

	return 0;
}
