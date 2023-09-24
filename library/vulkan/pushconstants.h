#ifndef METEOR_VULKAN_PUSHCONSTANTS_H
#define METEOR_VULKAN_PUSHCONSTANTS_H

#include <math/matrix.h>

#define PER_ENTITY_DATA_STRUCT { \
	xMat4_t xModel; \
}

#ifdef OS_WINDOWS
#pragma pack(push, 1)
	typedef struct PER_ENTITY_DATA_STRUCT xPerEntityData_t;
#pragma pack(pop)
#endif

#ifdef OS_LINUX
	typedef struct __attribute__((packed)) PER_ENTITY_DATA_STRUCT xPerEntityData_t;
#endif

#endif
