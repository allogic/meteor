#ifndef SANDSIM_STORAGE_H
#define SANDSIM_STORAGE_H

#include <math/vector.h>

#define PARTICLE_STRUCT { \
	xVec3_t xPosition; \
	xVec3_t xVelocity; \
}

#ifdef OS_WINDOWS
#pragma pack(push, 1)
	typedef struct PARTICLE_STRUCT xParticle_t;
#pragma pack(pop)
#endif

#ifdef OS_LINUX
	typedef struct __attribute__((packed)) PARTICLE_STRUCT xParticle_t;
#endif

#endif
