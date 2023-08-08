#ifndef STRUTL_H
#define STRUTL_H

void StrUtl_ReplaceChar(char* pcSubject, char cFrom, char cTo);

char* StrUtl_NormalizePath(const char* pcFilePath, uint32_t* pnFilePathLength, uint32_t nScratch);

char* StrUtl_JoinPath(const char* pcFirstPath, const char* pcSecondPath);

#endif
