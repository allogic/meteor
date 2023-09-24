#ifndef METEOR_VULKAN_UNIFORM_H
#define METEOR_VULKAN_UNIFORM_H

#include <math/matrix.h>

#define TIME_INFO_STRUCT { \
	float fTime; \
	float fDeltaTime; \
}

#define VIEW_PROJECTION_STRUCT { \
	xMat4_t xView; \
	xMat4_t xProjection; \
}

#define PER_ENTITY_DATA_STRUCT { \
	xMat4_t xModel; \
}

#ifdef OS_WINDOWS
#pragma pack(push, 1)
	typedef struct TIME_INFO_STRUCT xTimeInfo_t;
	typedef struct VIEW_PROJECTION_STRUCT xViewProjection_t;
#pragma pack(pop)
#endif

#ifdef OS_LINUX
	typedef struct __attribute__((packed)) TIME_INFO_STRUCT xTimeInfo_t;
	typedef struct __attribute__((packed)) VIEW_PROJECTION_STRUCT xViewProjection_t;
#endif

#endif
