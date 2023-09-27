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
#include <game/renderer.h>

/*
#define NUM_CHUNKS_X 5
#define NUM_CHUNKS_Y 5

#define CHUNK_WIDTH 5
#define CHUNK_HEIGHT 5
*/

struct xWorld_t {
	struct xBuffer_t* pxSharedVertexBuffer;
	struct xBuffer_t* pxSharedIndexBuffer;
	struct xEntity_t* apTrees[4];

	//struct xEntity_t* apChunks[NUM_CHUNKS_X][NUM_CHUNKS_Y];
	//struct xEntity_t* apParticles[1];
	//struct xEntity_t* apAffectors[1];
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

/*
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

			pxRenderable->bSharedVertexBuffer = true;
			pxRenderable->bSharedIndexBuffer = true;
			pxRenderable->bSharedColorImage = false;
			pxRenderable->pxVertexBuffer = pxWorld->pxSharedVertexBuffer;
			pxRenderable->pxIndexBuffer = pxWorld->pxSharedIndexBuffer;
			pxRenderable->pxColorImage = StorageImage_Alloc(pxInstance, "assets/sand_color.bmp");
			pxRenderable->nIndexCount = 6;

			pxPixelSystem->nWidth = Image_GetWidth(pxRenderable->pxColorImage);
			pxPixelSystem->nHeight = Image_GetHeight(pxRenderable->pxColorImage);
			pxPixelSystem->bSharedColorImage = true;
			pxPixelSystem->bSharedColorImageN = true;
			pxPixelSystem->bSharedColorImageS = true;
			pxPixelSystem->bSharedColorImageW = true;
			pxPixelSystem->bSharedColorImageE = true;
			pxPixelSystem->bSharedStateImage = false;
			pxPixelSystem->bSharedStateImageN = true;
			pxPixelSystem->bSharedStateImageS = true;
			pxPixelSystem->bSharedStateImageW = true;
			pxPixelSystem->bSharedStateImageE = true;
			pxPixelSystem->pxColorImage = pxRenderable->pxColorImage;
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

			pxPixelSystem->pxColorImageN = Entity_GetRenderable(pxWorld->apChunks[i][jn])->pxColorImage;
			pxPixelSystem->pxColorImageS = Entity_GetRenderable(pxWorld->apChunks[i][jp])->pxColorImage;
			pxPixelSystem->pxColorImageW = Entity_GetRenderable(pxWorld->apChunks[in][j])->pxColorImage;
			pxPixelSystem->pxColorImageE = Entity_GetRenderable(pxWorld->apChunks[ip][j])->pxColorImage;

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

	pxWorld->apParticles[0] = Scene_AllocEntity(pxScene, "", 0);

	Entity_SetTransform(pxWorld->apParticles[0], 0);
	Entity_SetRenderable(pxWorld->apParticles[0], 0);
	Entity_SetParticleSystem(pxWorld->apParticles[0], 0);

	xTransform_t* pxTransform = Entity_GetTransform(pxWorld->apParticles[0]);
	xRenderable_t* pxRenderable = Entity_GetRenderable(pxWorld->apParticles[0]);
	xParticleSystem_t* pxParticleSystem = Entity_GetParticleSystem(pxWorld->apParticles[0]);

	Vector3_Set(pxTransform->xPosition, 0.0F, 0.0F, 0.0F);
	Vector3_Set(pxTransform->xScale, 1.0F, 1.0F, 1.0F);

	pxRenderable->bSharedVertexBuffer = true;
	pxRenderable->bSharedIndexBuffer = true;
	pxRenderable->bSharedColorImage = false;
	pxRenderable->pxVertexBuffer = pxWorld->pxSharedVertexBuffer;
	pxRenderable->pxIndexBuffer = pxWorld->pxSharedIndexBuffer;
	pxRenderable->pxColorImage = StorageImage_Alloc(pxInstance, "assets/test.bmp");
	pxRenderable->nIndexCount = 6;

	for (uint32_t i = 0; i < 128; ++i) {
		xParticle_t xParticle;

		Vector4_Set(xParticle.xPosition,
			0.0F,
			0.0F,
			0.0F,
			0.0F);

		Vector4_Set(xParticle.xVelocity,
			XorShift128_Float(-0.0015F, 0.0015F),
			XorShift128_Float(0.0F, -0.005F),
			0.0F,
			0.0F);

		Vector_Push(pxParticles, &xParticle);
	}

	pxParticleSystem->bDebug = true;
	pxParticleSystem->fWidth = 15.0F;
	pxParticleSystem->fHeight = 25.0F;
	pxParticleSystem->fDepth = 0.0F;
	pxParticleSystem->bSharedBehaviourBuffer = false;
	pxParticleSystem->bSharedParticleBuffer = false;
	pxParticleSystem->pxBehaviourBuffer = UniformBuffer_AllocCoherent(pxInstance, sizeof(xParticleBehaviour_t));
	pxParticleSystem->pxParticleBuffer = StorageBuffer_AllocDevice(pxInstance, Vector_Data(pxParticles), Vector_Size(pxParticles));
	pxParticleSystem->nParticleCount = Vector_Count(pxParticles);

	xParticleBehaviour_t* pxParticleBehaviour = Buffer_GetMappedData(pxParticleSystem->pxBehaviourBuffer);

	pxParticleBehaviour->fLifetime = 5.0F;
	Vector4_Set(pxParticleBehaviour->xVelocity, 0.0F, 0.001F, 0.0F, 0.0F);
	Vector4_Set(pxParticleBehaviour->xStartColor, 0.0F, 0.0F, 0.0F, 0.0F);
	Vector4_Set(pxParticleBehaviour->xEndColor, 0.0F, 0.0F, 0.0F, 0.0F);
	Vector4_Set(pxParticleBehaviour->xStartScale, 0.0F, 0.0F, 0.0F, 0.0F);
	Vector4_Set(pxParticleBehaviour->xEndScale, 0.0F, 0.0F, 0.0F, 0.0F);

	Vector_Free(pxParticles);
}

static void World_FreeChunks(struct xWorld_t* pxWorld, struct xInstance_t* pxInstance, struct xScene_t* pxScene) {
	for (uint32_t i = 0; i < NUM_CHUNKS_X; ++i) {
		for (uint32_t j = 0; j < NUM_CHUNKS_Y; ++j) {
			Scene_FreeEntity(pxScene, pxInstance, pxWorld->apChunks[i][j]);
		}
	}
}
static void World_FreeParticles(struct xWorld_t* pxWorld, struct xInstance_t* pxInstance, struct xScene_t* pxScene) {
	for (uint32_t i = 0; i < ARRAY_LENGTH(pxWorld->apParticles); ++i) {
		Scene_FreeEntity(pxScene, pxInstance, pxWorld->apParticles[i]);
	}
}
*/

static void World_AllocTrees(struct xWorld_t* pxWorld, struct xInstance_t* pxInstance, struct xScene_t* pxScene) {
	pxWorld->apTrees[0] = Scene_AllocEntity(pxScene, "Tree A", 0);
	pxWorld->apTrees[1] = Scene_AllocEntity(pxScene, "Tree B", 0);
	pxWorld->apTrees[2] = Scene_AllocEntity(pxScene, "Tree C", 0);
	pxWorld->apTrees[3] = Scene_AllocEntity(pxScene, "Tree D", 0);

	Entity_SetTransform(pxWorld->apTrees[0], 0);
	Entity_SetTransform(pxWorld->apTrees[1], 0);
	Entity_SetTransform(pxWorld->apTrees[2], 0);
	Entity_SetTransform(pxWorld->apTrees[3], 0);

	Entity_SetRenderable(pxWorld->apTrees[0], 0);
	Entity_SetRenderable(pxWorld->apTrees[1], 0);
	Entity_SetRenderable(pxWorld->apTrees[2], 0);
	Entity_SetRenderable(pxWorld->apTrees[3], 0);

	xTransform_t* pxTransformA = Entity_GetTransform(pxWorld->apTrees[0]);
	xTransform_t* pxTransformB = Entity_GetTransform(pxWorld->apTrees[1]);
	xTransform_t* pxTransformC = Entity_GetTransform(pxWorld->apTrees[2]);
	xTransform_t* pxTransformD = Entity_GetTransform(pxWorld->apTrees[3]);

	xRenderable_t* pxRenderableA = Entity_GetRenderable(pxWorld->apTrees[0]);
	xRenderable_t* pxRenderableB = Entity_GetRenderable(pxWorld->apTrees[1]);
	xRenderable_t* pxRenderableC = Entity_GetRenderable(pxWorld->apTrees[2]);
	xRenderable_t* pxRenderableD = Entity_GetRenderable(pxWorld->apTrees[3]);

	Vector3_Set(pxTransformA->xPosition, -7.5F, 0.0F, 0.0F);
	Vector3_Set(pxTransformB->xPosition, -2.5F, 0.0F, 0.0F);
	Vector3_Set(pxTransformC->xPosition, 2.5F, 0.0F, 0.0F);
	Vector3_Set(pxTransformD->xPosition, 7.5F, 0.0F, 0.0F);

	Vector3_Set(pxTransformA->xScale, 4.0F, 4.0F, 1.0F);
	Vector3_Set(pxTransformB->xScale, 4.0F, 4.0F, 1.0F);
	Vector3_Set(pxTransformC->xScale, 4.0F, 4.0F, 1.0F);
	Vector3_Set(pxTransformD->xScale, 4.0F, 4.0F, 1.0F);

	pxRenderableA->bSharedVertexBuffer = true;
	pxRenderableA->bSharedIndexBuffer = true;
	pxRenderableA->bSharedColorImage = false;
	pxRenderableA->pxVertexBuffer = pxWorld->pxSharedVertexBuffer;
	pxRenderableA->pxIndexBuffer = pxWorld->pxSharedIndexBuffer;
	pxRenderableA->pxColorImage = StandardImage_Alloc(pxInstance, "assets/trees/tree_a.bmp");
	pxRenderableA->nIndexCount = 6;

	pxRenderableB->bSharedVertexBuffer = true;
	pxRenderableB->bSharedIndexBuffer = true;
	pxRenderableB->bSharedColorImage = false;
	pxRenderableB->pxVertexBuffer = pxWorld->pxSharedVertexBuffer;
	pxRenderableB->pxIndexBuffer = pxWorld->pxSharedIndexBuffer;
	pxRenderableB->pxColorImage = StandardImage_Alloc(pxInstance, "assets/trees/tree_b.bmp");
	pxRenderableB->nIndexCount = 6;

	pxRenderableC->bSharedVertexBuffer = true;
	pxRenderableC->bSharedIndexBuffer = true;
	pxRenderableC->bSharedColorImage = false;
	pxRenderableC->pxVertexBuffer = pxWorld->pxSharedVertexBuffer;
	pxRenderableC->pxIndexBuffer = pxWorld->pxSharedIndexBuffer;
	pxRenderableC->pxColorImage = StandardImage_Alloc(pxInstance, "assets/trees/tree_c.bmp");
	pxRenderableC->nIndexCount = 6;

	pxRenderableD->bSharedVertexBuffer = true;
	pxRenderableD->bSharedIndexBuffer = true;
	pxRenderableD->bSharedColorImage = false;
	pxRenderableD->pxVertexBuffer = pxWorld->pxSharedVertexBuffer;
	pxRenderableD->pxIndexBuffer = pxWorld->pxSharedIndexBuffer;
	pxRenderableD->pxColorImage = StandardImage_Alloc(pxInstance, "assets/trees/tree_d.bmp");
	pxRenderableD->nIndexCount = 6;
}

static void World_FreeTrees(struct xWorld_t* pxWorld, struct xInstance_t* pxInstance, struct xScene_t* pxScene) {
	for (uint32_t i = 0; i < ARRAY_LENGTH(pxWorld->apTrees); ++i) {
		Scene_FreeEntity(pxScene, pxInstance, pxWorld->apTrees[i]);
	}
}

struct xWorld_t* World_Alloc(struct xInstance_t* pxInstance, struct xScene_t* pxScene) {
	struct xWorld_t* pxWorld = (struct xWorld_t*)calloc(1, sizeof(struct xWorld_t));

	pxWorld->pxSharedVertexBuffer = VertexBuffer_AllocDevice(pxInstance, s_axVertices, sizeof(xDefaultVertex_t) * 4);
	pxWorld->pxSharedIndexBuffer = IndexBuffer_AllocDevice(pxInstance, s_anIndices, sizeof(uint32_t) * 6);

	World_AllocTrees(pxWorld, pxInstance, pxScene);

	//World_AllocChunks(pxWorld, pxInstance, pxScene);
	//World_AllocParticles(pxWorld, pxInstance, pxScene);

	return pxWorld;
}

void World_Free(struct xWorld_t* pxWorld, struct xInstance_t* pxInstance, struct xScene_t* pxScene) {
	//World_FreeParticles(pxWorld, pxInstance, pxScene);
	//World_FreeChunks(pxWorld, pxInstance, pxScene);

	World_FreeTrees(pxWorld, pxInstance, pxScene);

	Buffer_Free(pxWorld->pxSharedIndexBuffer, pxInstance);
	Buffer_Free(pxWorld->pxSharedVertexBuffer, pxInstance);

	free(pxWorld);
}

void World_Update(struct xWorld_t* pxWorld, struct xRenderer_t* pxRenderer, struct xTimer_t* pxTimer) {
	UNUSED(pxWorld);
	UNUSED(pxRenderer);
	UNUSED(pxTimer);
}
