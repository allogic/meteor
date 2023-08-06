#include <string.h>

#include "strutl.h"

void StrUtl_Replace(char* acSubject, char cFrom, char cTo) {
	while (*acSubject++) {
		if (*acSubject == cFrom) {
			*acSubject = cTo;
		}
	}
}

char* StrUtl_NormalizePath(const char* acFilePath) {
	char* pFilePath = 0;



	return pFilePath;
}
