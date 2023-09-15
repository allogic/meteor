#ifndef METEOR_COMMON_STRINGUTIL_H
#define METEOR_COMMON_STRINGUTIL_H

void StringUtil_ReplaceChar(char* pcSubject, char cFrom, char cTo);

char* StringUtil_NormalizePath(const char* pcFilePath, uint64_t* pwFilePathLength, uint64_t wScratch);

char* StringUtil_JoinPath(const char* pcFirstPath, const char* pcSecondPath);

#endif
