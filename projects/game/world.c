#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <common/macros.h>
#include <common/timer.h>

#include <math/vector.h>

#include <container/vector.h>

#include <random/xorshift128.h>

#include <vulkan/instance.h>
#include <vulkan/vertex.h>
#include <vulkan/buffer.h>
#include <vulkan/buffervariance.h>
#include <vulkan/image.h>
#include <vulkan/imagevariance.h>

#include <game/component.h>
#include <game/entity.h>
#include <game/scene.h>
#include <game/storage.h>
#include <game/world.h>

#define NUM_CHUNKS_X 5
#define NUM_CHUNKS_Y 5

#define CHUNK_WIDTH 5
#define CHUNK_HEIGHT 5

struct xWorld_t {
	struct xBuffer_t* pxSharedVertexBuffer;
	struct xBuffer_t* pxSharedIndexBuffer;
	struct xEntity_t* apChunks[NUM_CHUNKS_X][NUM_CHUNKS_Y];
	struct xEntity_t* apParticles[1];
	struct xEntity_t* apAffectors[1];
};

static xDefaultVertex_t s_axVertices[4] = {
	{ { -0.5F, -0.5F, 0.0F }, { 1.0F, 0.0F }, { 1.0F, 0.0F, 0.0F, 1.0F } },
	{ {  0.5F, -0.5F, 0.0F }, { 0.0F, 0.0F }, { 0.0F, 1.0F, 0.0F, 1.0F } },
	{ {  0.5F,  0.5F, 0.0F }, { 0.0F, 1.0F }, { 0.0F, 0.0F, 1.0F, 1.0F } },
	{ { -0.5F,  0.5F, 0.0F }, { 1.0F, 1.0F }, { 1.0F, 1.0F, 1.0F, 1.0F } },
};

static uint32_t s_anIndices[6] = {
	0, 1, 2, 2, 3, 0,
};

static void World_AllocChunks(struct xWorld_t* pxWorld, struct xInstance_t* pxInstance, struct xScene_t* pxScene) {
	for (uint32_t i = 0; i < NUM_CHUNKS_X; ++i) {
		for (uint32_t j = 0; j < NUM_CHUNKS_Y; ++j) {
			pxWorld->apChunks[i][j] = Scene_AllocEntity(pxScene, "chunk", 0);

			Entity_SetTransform(pxWorld->apChunks[i][j], 0);
			Entity_SetRenderable(pxWorld->apChunks[i][j], 0);
			Entity_SetPixelSystem(pxWorld->apChunks[i][j], 0);

			xTransform_t* pxTransform = Entity_GetTransform(pxWorld->apChunks[i][j]);
			xRenderable_t* pxRenderable = Entity_GetRenderable(pxWorld->apChunks[i][j]);
			xPixelSystem_t* pxPixelSystem = Entity_GetPixelSystem(pxWorld->apChunks[i][j]);

			float hw = (float)NUM_CHUNKS_X * CHUNK_WIDTH / 2;
			float hh = (float)NUM_CHUNKS_Y * CHUNK_HEIGHT / 2;

			float hcw = (float)CHUNK_WIDTH / 2;
			float hch = (float)CHUNK_WIDTH / 2;

			float x = (float)i * CHUNK_WIDTH - hw + hcw;
			float y = (float)j * CHUNK_HEIGHT - hh + hch;

			Vector3_Set(pxTransform->xPosition, x, y, 0.0F);
			Vector3_Set(pxTransform->xScale, CHUNK_WIDTH - 1, CHUNK_HEIGHT - 1, 1.0F);

			pxRenderable->pxAlbedoImage = StorageImage_Alloc(pxInstance, "assets/sand_albedo.bmp");
			pxRenderable->pxVertexBuffer = pxWorld->pxSharedVertexBuffer;
			pxRenderable->pxIndexBuffer = pxWorld->pxSharedIndexBuffer;
			pxRenderable->nIndexCount = 6;

			pxPixelSystem->nWidth = Image_GetWidth(pxRenderable->pxAlbedoImage);
			pxPixelSystem->nHeight = Image_GetHeight(pxRenderable->pxAlbedoImage);
			pxPixelSystem->pxAlbedoImage = pxRenderable->pxAlbedoImage;
			pxPixelSystem->pxStateImage = StorageImage_Alloc(pxInstance, "assets/sand_state.bmp");
		}
	}

	for (uint32_t i = 0; i < NUM_CHUNKS_X; ++i) {
		for (uint32_t j = 0; j < NUM_CHUNKS_Y; ++j) {
			uint32_t in = i - 1;
			uint32_t ip = i + 1;
			uint32_t jn = j - 1;
			uint32_t jp = j + 1;

			if (i == 0) in = (NUM_CHUNKS_X - 1);
			if (j == 0) jn = (NUM_CHUNKS_Y - 1);

			if (i == (NUM_CHUNKS_X - 1)) ip = 0;
			if (j == (NUM_CHUNKS_Y - 1)) jp = 0;

			xPixelSystem_t* pxPixelSystem = Entity_GetPixelSystem(pxWorld->apChunks[i][j]);

			pxPixelSystem->pxAlbedoImageN = Entity_GetRenderable(pxWorld->apChunks[i][jn])->pxAlbedoImage;
			pxPixelSystem->pxAlbedoImageS = Entity_GetRenderable(pxWorld->apChunks[i][jp])->pxAlbedoImage;
			pxPixelSystem->pxAlbedoImageW = Entity_GetRenderable(pxWorld->apChunks[in][j])->pxAlbedoImage;
			pxPixelSystem->pxAlbedoImageE = Entity_GetRenderable(pxWorld->apChunks[ip][j])->pxAlbedoImage;

			pxPixelSystem->pxStateImageN = Entity_GetPixelSystem(pxWorld->apChunks[i][jn])->pxStateImage;
			pxPixelSystem->pxStateImageS = Entity_GetPixelSystem(pxWorld->apChunks[i][jp])->pxStateImage;
			pxPixelSystem->pxStateImageW = Entity_GetPixelSystem(pxWorld->apChunks[in][j])->pxStateImage;
			pxPixelSystem->pxStateImageE = Entity_GetPixelSystem(pxWorld->apChunks[ip][j])->pxStateImage;
		}
	}

	for (uint32_t i = 0; i < ARRAY_LENGTH(pxWorld->apAffectors); ++i) {
		pxWorld->apAffectors[i] = Scene_AllocEntity(pxScene, "", 0);

		Entity_SetTransform(pxWorld->apAffectors[i], 0);
		Entity_SetPixelAffector(pxWorld->apAffectors[i], 0);

		xPixelAffector_t* pxPixelAffector = Entity_GetPixelAffector(pxWorld->apAffectors[i]);
	}
}
static void World_AllocParticles(struct xWorld_t* pxWorld, struct xInstance_t* pxInstance, struct xScene_t* pxScene) {
	XorShift128_Init(0x42);

	struct xVector_t* pxParticles = Vector_Alloc(sizeof(xParticle_t));

	for (uint32_t i = 0; i < 32; ++i) {
		xParticle_t xParticle;

		Vector3_Set(xParticle.xPosition, XorShift128_Float(-2.5F, 2.5F), XorShift128_Float(-2.5F, 2.5F), 0.0F);
		Vector3_Set(xParticle.xVelocity, 0.0F, 1.0F, 0.0F);

		Vector_Push(pxParticles, &xParticle);
	}

	for (uint32_t i = 0; i < ARRAY_LENGTH(pxWorld->apParticles); ++i) {
		pxWorld->apParticles[i] = Scene_AllocEntity(pxScene, "", 0);

		Entity_SetTransform(pxWorld->apParticles[i], 0);
		Entity_SetRenderable(pxWorld->apParticles[i], 0);
		Entity_SetParticleSystem(pxWorld->apParticles[i], 0);

		xTransform_t* pxTransform = Entity_GetTransform(pxWorld->apParticles[i]);
		xRenderable_t* pxRenderable = Entity_GetRenderable(pxWorld->apParticles[i]);
		xParticleSystem_t* pxParticleSystem = Entity_GetParticleSystem(pxWorld->apParticles[i]);

		Vector3_Set(pxTransform->xPosition, 0.0F, 0.0F, 1.0F);
		Vector3_Set(pxTransform->xScale, 1.0F, 1.0F, 1.0F);

		pxRenderable->pxAlbedoImage = StorageImage_Alloc(pxInstance, "assets/test.bmp");
		pxRenderable->pxVertexBuffer = pxWorld->pxSharedVertexBuffer;
		pxRenderable->pxIndexBuffer = pxWorld->pxSharedIndexBuffer;
		pxRenderable->nIndexCount = 6;

		pxParticleSystem->nWidth = 10;
		pxParticleSystem->nHeight = 10;
		pxParticleSystem->pxParticleBuffer = StorageBuffer_Alloc(pxInstance, Vector_Data(pxParticles), Vector_Size(pxParticles));
		pxParticleSystem->nParticleCount = Vector_Count(pxParticles);
	}

	Vector_Free(pxParticles);
}

static void World_FreeChunks(struct xWorld_t* pxWorld, struct xInstance_t* pxInstance, struct xScene_t* pxScene) {
	for (uint32_t i = 0; i < NUM_CHUNKS_X; ++i) {
		for (uint32_t j = 0; j < NUM_CHUNKS_Y; ++j) {
			xRenderable_t* pxRenderable = Entity_GetRenderable(pxWorld->apChunks[i][j]);
			xPixelSystem_t* pxPixelSystem = Entity_GetPixelSystem(pxWorld->apChunks[i][j]);

			Image_Free(pxRenderable->pxAlbedoImage, pxInstance);
			Image_Free(pxPixelSystem->pxStateImage, pxInstance);

			Scene_FreeEntity(pxScene, pxWorld->apChunks[i][j]);
		}
	}
}
static void World_FreeParticles(struct xWorld_t* pxWorld, struct xInstance_t* pxInstance, struct xScene_t* pxScene) {
	for (uint32_t i = 0; i < ARRAY_LENGTH(pxWorld->apParticles); ++i) {
		xRenderable_t* pxRenderable = Entity_GetRenderable(pxWorld->apParticles[i]);
		xParticleSystem_t* pxParticleSystem = Entity_GetParticleSystem(pxWorld->apParticles[i]);

		Image_Free(pxRenderable->pxAlbedoImage, pxInstance);

		Buffer_Free(pxParticleSystem->pxParticleBuffer, pxInstance);

		Scene_FreeEntity(pxScene, pxWorld->apParticles[i]);
	}
}

struct xWorld_t* World_Alloc(struct xInstance_t* pxInstance, struct xScene_t* pxScene) {
	struct xWorld_t* pxWorld = (struct xWorld_t*)calloc(1, sizeof(struct xWorld_t));

	pxWorld->pxSharedVertexBuffer = VertexBuffer_Alloc(pxInstance, s_axVertices, sizeof(xDefaultVertex_t) * 4);
	pxWorld->pxSharedIndexBuffer = IndexBuffer_Alloc(pxInstance, s_anIndices, sizeof(uint32_t) * 6);

	World_AllocChunks(pxWorld, pxInstance, pxScene);
	World_AllocParticles(pxWorld, pxInstance, pxScene);

	return pxWorld;
}

void World_Free(struct xWorld_t* pxWorld, struct xInstance_t* pxInstance, struct xScene_t* pxScene) {
	World_FreeParticles(pxWorld, pxInstance, pxScene);
	World_FreeChunks(pxWorld, pxInstance, pxScene);

	Buffer_Free(pxWorld->pxSharedIndexBuffer, pxInstance);
	Buffer_Free(pxWorld->pxSharedVertexBuffer, pxInstance);

	free(pxWorld);
}

void World_Update(struct xWorld_t* pxWorld, struct xTimer_t* pxTimer) {
	for (uint32_t i = 0; i < ARRAY_LENGTH(pxWorld->apAffectors); ++i) {
		xTransform_t* pxTransform = Entity_GetTransform(pxWorld->apAffectors[i]);
		
		Vector3_Set(pxTransform->xPosition, sinf(Timer_GetTime(pxTimer)) * 2.0F, 0.0F, 0.0F);
	}
}
