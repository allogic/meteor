#ifndef METEOR_UNIFORM_H
#define METEOR_UNIFORM_H

#include <math/matrix.h>

#define TIME_INFO_STRUCT { \
	float fTime; \
	float fDeltaTime; \
}

#define MODEL_VIEW_PROJECTION_STRUCT { \
	xMat4_t xModel; \
	xMat4_t xView; \
	xMat4_t xProjection; \
}

#ifdef OS_WINDOWS
#pragma pack(push, 1)
	typedef struct TIME_INFO_STRUCT xTimeInfo_t;
	typedef struct MODEL_VIEW_PROJECTION_STRUCT xModelViewProjection_t;
#pragma pack(pop)
#endif

#ifdef OS_LINUX
	typedef struct __attribute__((packed)) TIME_INFO_STRUCT xTimeInfo_t;
	typedef struct __attribute__((packed)) MODEL_VIEW_PROJECTION_STRUCT xModelViewProjection_t;
#endif

#endif
