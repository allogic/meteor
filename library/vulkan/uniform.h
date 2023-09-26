#ifndef METEOR_VULKAN_UNIFORM_H
#define METEOR_VULKAN_UNIFORM_H

#include <math/matrix.h>

typedef struct {
	float fTime;
	float fDeltaTime;
} xTimeInfo_t;

typedef struct {
	xMat4_t xView;
	xMat4_t xProjection;
} xViewProjection_t;

typedef struct {
	float fLifetime;
	float fReserved0;
	float fReserved1;
	float fReserved2;
	xVec4_t xVelocity;
	xVec4_t xStartColor;
	xVec4_t xEndColor;
	xVec4_t xStartScale;
	xVec4_t xEndScale;
} xParticleBehaviour_t;

#endif
