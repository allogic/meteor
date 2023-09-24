#ifndef GAME_COMPONENT_H
#define GAME_COMPONENT_H

#include <stdint.h>

#include <math/vector.h>

#define COMPONENT_TRANSFORM_BIT        0x0000000000000001
#define COMPONENT_CAMERA_BIT           0x0000000000000002
#define COMPONENT_RIGIDBODY_BIT        0x0000000000000004
#define COMPONENT_RENDERABLE_BIT       0x0000000000000008
#define COMPONENT_PARTICLESYSTEM_BIT   0x0000000000000010
#define COMPONENT_PARTICLEAFFECTOR_BIT 0x0000000000000020
#define COMPONENT_PIXELSYSTEM_BIT      0x0000000000000040
#define COMPONENT_PIXELAFFECTOR_BIT    0x0000000000000080

#define COMPONENT_TRANSFORM_IDX        0x0
#define COMPONENT_CAMERA_IDX           0x1
#define COMPONENT_RIGIDBODY_IDX        0x2
#define COMPONENT_RENDERABLE_IDX       0x3
#define COMPONENT_PARTICLESYSTEM_IDX   0x4
#define COMPONENT_PARTICLEAFFECTOR_IDX 0x5
#define COMPONENT_PIXELSYSTEM_IDX      0x6
#define COMPONENT_PIXELAFFECTOR_IDX    0x7

typedef struct {
	xVec3_t xPosition;
	xVec3_t xRotation;
	xVec3_t xScale;
} xTransform_t;

typedef struct {
	struct {
		float fLeft;
		float fRight;
		float fBottom;
		float fTop;
		float fNearZ;
		float fFarZ;
	} xOrthographic;
	struct {
		float fFov;
		float fNearZ;
		float fFarZ;
	} xPerspective;
} xCamera_t;

typedef struct {
	void* pDummy;
} xRigidBody_t;

typedef struct {
	struct xBuffer_t* pxVertexBuffer;
	struct xBuffer_t* pxIndexBuffer;
	uint32_t nIndexCount;
	struct xImage_t* pxAlbedoImage;
} xRenderable_t;

typedef struct {
	uint32_t nWidth;
	uint32_t nHeight;
	struct xBuffer_t* pxParticleBuffer;
	uint32_t nParticleCount;
} xParticleSystem_t;

typedef struct {
	void* pDummy;
} xParticleAffector_t;

typedef struct {
	uint32_t nWidth;
	uint32_t nHeight;
	struct xImage_t* pxAlbedoImage;
	struct xImage_t* pxAlbedoImageN;
	struct xImage_t* pxAlbedoImageS;
	struct xImage_t* pxAlbedoImageW;
	struct xImage_t* pxAlbedoImageE;
	struct xImage_t* pxStateImage;
	struct xImage_t* pxStateImageN;
	struct xImage_t* pxStateImageS;
	struct xImage_t* pxStateImageW;
	struct xImage_t* pxStateImageE;
} xPixelSystem_t;

typedef struct {
	void* pDummy;
} xPixelAffector_t;

#endif
