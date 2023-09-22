#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <common/macros.h>
#include <common/stringutil.h>

#include <filesystem/common.h>

void StringUtil_ReplaceChar(char* pcString, char cFrom, char cTo) {
	while (*pcString++) {
		if (*pcString == cFrom) {
			*pcString = cTo;
		}
	}
}

int32_t StringUtil_FindFirstOf(const char* pcString, char cValue) {
	int32_t nIndex = 1;

	while (*pcString++) {
		if (*pcString == cValue) {
			return nIndex;
		}

		nIndex++;
	}

	return -1;
}
