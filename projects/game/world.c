#include <stdlib.h>

#include <math/vector.h>

#include <vulkan/instance.h>
#include <vulkan/vertex.h>
#include <vulkan/buffer.h>
#include <vulkan/buffervariance.h>
#include <vulkan/image.h>
#include <vulkan/imagevariance.h>

#include <game/component.h>
#include <game/entity.h>
#include <game/scene.h>
#include <game/world.h>

#define NUM_CHUNKS_X 5
#define NUM_CHUNKS_Y 5

#define CHUNK_WIDTH 5
#define CHUNK_HEIGHT 5

struct xWorld_t {
	struct xBuffer_t* pxSharedVertexBuffer;
	struct xBuffer_t* pxSharedIndexBuffer;
	struct xEntity_t* apEntities[NUM_CHUNKS_X][NUM_CHUNKS_Y];
};

static xVertex_t s_axVertices[4] = {
	{ { -0.5F, -0.5F, 0.0F }, { 1.0F, 0.0F }, { 1.0F, 0.0F, 0.0F, 1.0F } },
	{ {  0.5F, -0.5F, 0.0F }, { 0.0F, 0.0F }, { 0.0F, 1.0F, 0.0F, 1.0F } },
	{ {  0.5F,  0.5F, 0.0F }, { 0.0F, 1.0F }, { 0.0F, 0.0F, 1.0F, 1.0F } },
	{ { -0.5F,  0.5F, 0.0F }, { 1.0F, 1.0F }, { 1.0F, 1.0F, 1.0F, 1.0F } },
};

static uint32_t s_anIndices[6] = {
	0, 1, 2, 2, 3, 0,
};

struct xWorld_t* World_Alloc(struct xInstance_t* pxInstance, struct xScene_t* pxScene) {
	struct xWorld_t* pxWorld = (struct xWorld_t*)calloc(1, sizeof(struct xWorld_t));

	pxWorld->pxSharedVertexBuffer = VertexBuffer_Alloc(pxInstance, s_axVertices, sizeof(xVertex_t) * 4);
	pxWorld->pxSharedIndexBuffer = IndexBuffer_Alloc(pxInstance, s_anIndices, sizeof(uint32_t) * 6);

	for (uint32_t i = 0; i < NUM_CHUNKS_X; ++i) {
		for (uint32_t j = 0; j < NUM_CHUNKS_Y; ++j) {
			pxWorld->apEntities[i][j] = Scene_AllocEntity(pxScene, "chunk", 0);

			xRenderable_t xRenderable = {
				pxWorld->pxSharedVertexBuffer,
				pxWorld->pxSharedIndexBuffer,
				6,
				StorageImage_Alloc(pxInstance, "assets/chunk.bmp"),
			};

			Entity_SetTransform(pxWorld->apEntities[i][j], 0);
			Entity_SetRenderable(pxWorld->apEntities[i][j], &xRenderable);

			xTransform_t* pxTransform = Entity_GetTransform(pxWorld->apEntities[i][j]);

			float hw = (float)NUM_CHUNKS_X * CHUNK_WIDTH / 2;
			float hh = (float)NUM_CHUNKS_Y * CHUNK_HEIGHT / 2;

			float hcw = (float)CHUNK_WIDTH / 2;
			float hch = (float)CHUNK_WIDTH / 2;

			float x = (float)i * CHUNK_WIDTH - hw + hcw;
			float y = (float)j * CHUNK_HEIGHT - hh + hch;

			Vector_Set(pxTransform->xPosition, x, y, 0.0F);
			Vector_Set(pxTransform->xScale, CHUNK_WIDTH - 1, CHUNK_HEIGHT - 1, 1.0F);
		}
	}

	for (uint32_t i = 1; i < (NUM_CHUNKS_X - 1); ++i) {
		for (uint32_t j = 1; j < (NUM_CHUNKS_Y - 1); ++j) {
			xPixelSystem_t xPixelSystem = {
				Image_GetWidth(Entity_GetRenderable(pxWorld->apEntities[i][j])->pxAlbedoImage),
				Image_GetHeight(Entity_GetRenderable(pxWorld->apEntities[i][j])->pxAlbedoImage),
				Entity_GetRenderable(pxWorld->apEntities[i][j])->pxAlbedoImage,
				Entity_GetRenderable(pxWorld->apEntities[i][j - 1])->pxAlbedoImage,
				Entity_GetRenderable(pxWorld->apEntities[i][j + 1])->pxAlbedoImage,
				Entity_GetRenderable(pxWorld->apEntities[i - 1][j])->pxAlbedoImage,
				Entity_GetRenderable(pxWorld->apEntities[i + 1][j])->pxAlbedoImage,
			};

			Entity_SetPixelSystem(pxWorld->apEntities[i][j], &xPixelSystem);
		}
	}

	return pxWorld;
}

void World_Free(struct xWorld_t* pxWorld, struct xInstance_t* pxInstance, struct xScene_t* pxScene) {
	for (uint32_t i = 0; i < NUM_CHUNKS_X; ++i) {
		for (uint32_t j = 0; j < NUM_CHUNKS_Y; ++j) {
			xRenderable_t* pxRenderable = Entity_GetRenderable(pxWorld->apEntities[i][j]);

			Image_Free(pxRenderable->pxAlbedoImage, pxInstance);

			Scene_FreeEntity(pxScene, pxWorld->apEntities[i][j]);
		}
	}

	Buffer_Free(pxWorld->pxSharedIndexBuffer, pxInstance);
	Buffer_Free(pxWorld->pxSharedVertexBuffer, pxInstance);

	free(pxWorld);
}
