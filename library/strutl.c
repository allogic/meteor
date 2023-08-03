#include "strutl.h"

void StrUtl_Replace(char* acSubject, char cFrom, char cTo) {
	while (*acSubject++) {
		if (*acSubject == cFrom) {
			*acSubject = cTo;
		}
	}
}
