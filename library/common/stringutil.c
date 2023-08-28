#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <common/macros.h>
#include <common/stringutil.h>

#include <filesystem/fs.h>

void StringUtil_ReplaceChar(char* pcSubject, char cFrom, char cTo) {
	while (*pcSubject++) {
		if (*pcSubject == cFrom) {
			*pcSubject = cTo;
		}
	}
}

char* StringUtil_NormalizePath(const char* pcFilePath, uint64_t* pwFilePathLength, uint64_t wScratch) {
	char acBuffer[MAX_PATH_SIZE];

	memset(acBuffer, 0, MAX_PATH_SIZE);

	uint64_t wFilePathLength = strlen(pcFilePath);

	for (uint64_t i = 0; i < wFilePathLength; ++i) {
		if (pcFilePath[i] == '\\') {
			acBuffer[i] = '/';
		} else {
			acBuffer[i] = pcFilePath[i];
		}
	}

	if (acBuffer[wFilePathLength - 1] != '/') {
		acBuffer[wFilePathLength] = '/';
	}

	uint64_t wNormFilePathLength = strlen(acBuffer);

	if (pwFilePathLength) {
		*pwFilePathLength = wNormFilePathLength + wScratch;
	}

	char* pcNormFilePath = (char*)calloc(wNormFilePathLength + wScratch + 1, sizeof(char));
	memcpy(pcNormFilePath, acBuffer, wNormFilePathLength);

	return pcNormFilePath;
}

char* StringUtil_JoinPath(const char* pcFirstPath, const char* pcSecondPath) {
	UNUSED(pcFirstPath);
	UNUSED(pcSecondPath);

	return 0;
}
