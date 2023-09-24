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

			Entity_SetTransform(pxWorld->apEntities[i][j], 0);
			Entity_SetRenderable(pxWorld->apEntities[i][j], 0);
			Entity_SetPixelSystem(pxWorld->apEntities[i][j], 0);

			xTransform_t* pxTransform = Entity_GetTransform(pxWorld->apEntities[i][j]);
			xRenderable_t* pxRenderable = Entity_GetRenderable(pxWorld->apEntities[i][j]);
			xPixelSystem_t* pxPixelSystem = Entity_GetPixelSystem(pxWorld->apEntities[i][j]);

			float hw = (float)NUM_CHUNKS_X * CHUNK_WIDTH / 2;
			float hh = (float)NUM_CHUNKS_Y * CHUNK_HEIGHT / 2;

			float hcw = (float)CHUNK_WIDTH / 2;
			float hch = (float)CHUNK_WIDTH / 2;

			float x = (float)i * CHUNK_WIDTH - hw + hcw;
			float y = (float)j * CHUNK_HEIGHT - hh + hch;

			Vector_Set(pxTransform->xPosition, x, y, 0.0F);
			Vector_Set(pxTransform->xScale, CHUNK_WIDTH - 1, CHUNK_HEIGHT - 1, 1.0F);

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

			xPixelSystem_t* pxPixelSystem = Entity_GetPixelSystem(pxWorld->apEntities[i][j]);

			pxPixelSystem->pxAlbedoImageN = Entity_GetRenderable(pxWorld->apEntities[i][jn])->pxAlbedoImage;
			pxPixelSystem->pxAlbedoImageS = Entity_GetRenderable(pxWorld->apEntities[i][jp])->pxAlbedoImage;
			pxPixelSystem->pxAlbedoImageW = Entity_GetRenderable(pxWorld->apEntities[in][j])->pxAlbedoImage;
			pxPixelSystem->pxAlbedoImageE = Entity_GetRenderable(pxWorld->apEntities[ip][j])->pxAlbedoImage;

			pxPixelSystem->pxStateImageN = Entity_GetPixelSystem(pxWorld->apEntities[i][jn])->pxStateImage;
			pxPixelSystem->pxStateImageS = Entity_GetPixelSystem(pxWorld->apEntities[i][jp])->pxStateImage;
			pxPixelSystem->pxStateImageW = Entity_GetPixelSystem(pxWorld->apEntities[in][j])->pxStateImage;
			pxPixelSystem->pxStateImageE = Entity_GetPixelSystem(pxWorld->apEntities[ip][j])->pxStateImage;
		}
	}

	return pxWorld;
}

void World_Free(struct xWorld_t* pxWorld, struct xInstance_t* pxInstance, struct xScene_t* pxScene) {
	for (uint32_t i = 0; i < NUM_CHUNKS_X; ++i) {
		for (uint32_t j = 0; j < NUM_CHUNKS_Y; ++j) {
			xRenderable_t* pxRenderable = Entity_GetRenderable(pxWorld->apEntities[i][j]);
			xPixelSystem_t* pxPixelSystem = Entity_GetPixelSystem(pxWorld->apEntities[i][j]);

			Image_Free(pxRenderable->pxAlbedoImage, pxInstance);
			Image_Free(pxPixelSystem->pxStateImage, pxInstance);

			Scene_FreeEntity(pxScene, pxWorld->apEntities[i][j]);
		}
	}

	Buffer_Free(pxWorld->pxSharedIndexBuffer, pxInstance);
	Buffer_Free(pxWorld->pxSharedVertexBuffer, pxInstance);

	free(pxWorld);
}
