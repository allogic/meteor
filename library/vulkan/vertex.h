#ifndef METEOR_VULKAN_VERTEX_H
#define METEOR_VULKAN_VERTEX_H

#include <math/vector.h>

#define DEFAULT_VERTEX_STRUCT { \
    xVec3_t xPosition; \
    xVec2_t xUv; \
    xVec4_t xColor; \
}

#define DEBUG_VERTEX_STRUCT { \
    xVec3_t xPosition; \
    xVec4_t xColor; \
}

#ifdef OS_WINDOWS
#pragma pack(push, 1)
	typedef struct DEFAULT_VERTEX_STRUCT xDefaultVertex_t;
    typedef struct DEBUG_VERTEX_STRUCT xDebugVertex_t;
#pragma pack(pop)
#endif

#ifdef OS_LINUX
	typedef struct __attribute__((packed)) DEFAULT_VERTEX_STRUCT xDefaultVertex_t;
    typedef struct __attribute__((packed)) DEBUG_VERTEX_STRUCT xDebugVertex_t;
#endif

#endif
