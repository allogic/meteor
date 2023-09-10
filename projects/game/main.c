#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <common/timer.h>

#include <debug/stacktrace.h>

#include <math/vector.h>

#include <platform/nativewindow.h>

#include <random/xorshift128.h>

#include <vulkan/instance.h>
#include <vulkan/vertex.h>
#include <vulkan/buffer.h>
#include <vulkan/buffervariance.h>

#include <game/assets.h>
#include <game/scene.h>
#include <game/entity.h>
#include <game/component.h>
#include <game/storage.h>

xVertex_t axVertices[4] = {
	{ { -0.5F, -0.5F, 0.0F }, { 1.0F, 0.0F }, { 1.0F, 0.0F, 0.0F, 1.0F } },
	{ {  0.5F, -0.5F, 0.0F }, { 0.0F, 0.0F }, { 0.0F, 1.0F, 0.0F, 1.0F } },
	{ {  0.5F,  0.5F, 0.0F }, { 0.0F, 1.0F }, { 0.0F, 0.0F, 1.0F, 1.0F } },
	{ { -0.5F,  0.5F, 0.0F }, { 1.0F, 1.0F }, { 1.0F, 1.0F, 1.0F, 1.0F } },
};

uint32_t anIndices[6] = {
	0, 1, 2, 2, 3, 0,
};

int32_t main(void) {

#ifdef DEBUG
	StackTrace_Alloc();
#endif

	NativeWindow_Alloc(WINDOW_NAME, 1200, 1200);

	struct xTimer_t* pxTimer = Timer_Alloc();

	struct xInstance_t* pxInstance = Instance_Alloc();

	Assets_Alloc();

	Assets_CreateImageForName(pxInstance, "Test", "test.bmp");

	uint32_t nParticleCount = 10000;

	xParticle_t* pxParticles = (xParticle_t*)calloc(nParticleCount, sizeof(xParticle_t));

	XorShift128_Init(0x42);

	for (uint32_t i = 0; i < nParticleCount; ++i) {
		pxParticles[i].xPosition[0] = XorShift128_Float(-10.0F, 10.0F);
		pxParticles[i].xPosition[1] = XorShift128_Float(-10.0F, 10.0F);
		pxParticles[i].xVelocity[0] = XorShift128_Float(-10.0F, 10.0F);
		pxParticles[i].xVelocity[1] = XorShift128_Float(-10.0F, 10.0F);
	}

	xRenderable_t xSharedQuad = {
		VertexBuffer_Alloc(pxInstance, axVertices, sizeof(xVertex_t) * 4),
		IndexBuffer_Alloc(pxInstance, anIndices, sizeof(uint32_t) * 6),
		6,
		0,
	};

	xParticleSystem_t xParticleSystem = {
		StorageBuffer_Alloc(pxInstance, pxParticles, sizeof(xParticle_t) * nParticleCount),
		nParticleCount,
	};

	free(pxParticles);

	struct xScene_t* pxScene = Scene_Alloc(pxInstance);

	struct xEntity_t* pxEntityA = Scene_CreateEntity(pxScene, "EntityA", 0);
	struct xEntity_t* pxEntityB = Scene_CreateEntity(pxScene, "EntityB", 0);
	struct xEntity_t* pxEntityC = Scene_CreateEntity(pxScene, "EntityC", 0);

	Entity_SetTransform(pxEntityA, 0);
	Entity_SetTransform(pxEntityB, 0);
	Entity_SetTransform(pxEntityC, 0);

	Entity_SetRenderable(pxEntityA, &xSharedQuad);
	Entity_SetRenderable(pxEntityB, &xSharedQuad);
	Entity_SetRenderable(pxEntityC, &xSharedQuad);

	xTransform_t* pxTransformA = Entity_GetTransform(pxEntityA);
	xTransform_t* pxTransformB = Entity_GetTransform(pxEntityB);
	xTransform_t* pxTransformC = Entity_GetTransform(pxEntityC);

	Vector_Set(pxTransformA->xPosition, -5.0F, 0.0F, 0.0F);
	Vector_Set(pxTransformB->xPosition,  0.0F, 0.0F, 0.0F);
	Vector_Set(pxTransformC->xPosition,  5.0F, 0.0F, 0.0F);

	Timer_Start(pxTimer);

	while (NativeWindow_ShouldNotClose()) {
		NativeWindow_PollEvents();

		Timer_Measure(pxTimer);

		if (NativeWindow_HasResized()) {
			Instance_WaitIdle(pxInstance);

			Scene_Resize(pxScene, pxInstance);
		}

		Scene_Draw(pxScene, pxInstance, pxTimer);
	}

	Instance_WaitIdle(pxInstance);

	Scene_Free(pxScene, pxInstance);

	Buffer_Free(xParticleSystem.pxParticleBuffer, pxInstance);

	Buffer_Free(xSharedQuad.pxIndexBuffer, pxInstance);
	Buffer_Free(xSharedQuad.pxVertexBuffer, pxInstance);

	Assets_Free(pxInstance);

	Instance_Free(pxInstance);

	Timer_Free(pxTimer);

	NativeWindow_Free();

#ifdef DEBUG
	StackTrace_Free();
#endif

	return 0;
}
