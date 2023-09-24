#ifndef GAME_PUSHCONSTANTS_H
#define GAME_PUSHCONSTANTS_H

#include <math/vector.h>

#define DIMENSIONS_STRUCT { \
	xVec3_t xSize; \
}

#ifdef OS_WINDOWS
#pragma pack(push, 1)
	typedef struct DIMENSIONS_STRUCT xDimensions_t;
#pragma pack(pop)
#endif

#ifdef OS_LINUX
	typedef struct __attribute__((packed)) DIMENSIONS_STRUCT xDimensions_t;
#endif

#endif
