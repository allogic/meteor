#ifndef METEOR_FILESYSTEM_PATH
#define METEOR_FILESYSTEM_PATH

#include <stdint.h>

char* Path_Normalize(const char* pcFilePath, uint64_t* pwFilePathLength, uint64_t wScratch);
char* Path_Join(const char* pcFirstPath, const char* pcSecondPath);

#endif
