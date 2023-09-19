#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/timer.h>

#include <container/list.h>

#include <math/vector.h>
#include <math/matrix.h>
#include <math/orthographic.h>
#include <math/view.h>

#include <vulkan/uniform.h>
#include <vulkan/swapchain.h>

#include <game/entity.h>
#include <game/component.h>
#include <game/scene.h>
#include <game/renderer.h>
#include <game/world.h>

struct xScene_t {
	struct xList_t* pxEntities;
	struct xSwapChain_t* pxSwapChain;
	struct xRenderer_t* pxRenderer;
	struct xWorld_t* pxWorld;
};

struct xScene_t* Scene_Alloc(struct xInstance_t* pxInstance) {
	struct xScene_t* pxScene = (struct xScene_t*)calloc(1, sizeof(struct xScene_t));

	pxScene->pxEntities = List_Alloc(sizeof(struct xEntity_t*));
	pxScene->pxSwapChain = SwapChain_Alloc(pxInstance);
	pxScene->pxRenderer = Renderer_Alloc(pxInstance, pxScene->pxSwapChain);
	pxScene->pxWorld = World_Alloc(pxInstance, pxScene);

	return pxScene;
}

void Scene_Free(struct xScene_t* pxScene, struct xInstance_t* pxInstance) {
	World_Free(pxScene->pxWorld, pxInstance, pxScene);
	Renderer_Free(pxScene->pxRenderer, pxInstance);
	SwapChain_Free(pxScene->pxSwapChain, pxInstance);
	List_Free(pxScene->pxEntities);

	free(pxScene);
}

struct xEntity_t* Scene_AllocEntity(struct xScene_t* pxScene, const char* pcName, struct xEntity_t* pxParent) {
	struct xEntity_t* pxEntity = Entity_Alloc(pcName, pxParent);

	List_Add(pxScene->pxEntities, &pxEntity);

	return pxEntity;
}

void Scene_FreeEntity(struct xScene_t* pxScene, struct xEntity_t* pxEntity) {
	//List_Remove(pxScene->pxEntities, pxEntity);

	Entity_Free(pxEntity);

	// TODO: Still not working..
}

void Scene_CommitEntities(struct xScene_t* pxScene, struct xInstance_t* pxInstance) {
	Renderer_RebuildEnitityDescriptorSets(pxScene->pxRenderer, pxInstance, pxScene->pxEntities);
}

void Scene_Resize(struct xScene_t* pxScene, struct xInstance_t* pxInstance) {
	Renderer_Free(pxScene->pxRenderer, pxInstance);
	SwapChain_Free(pxScene->pxSwapChain, pxInstance);

	pxScene->pxSwapChain = SwapChain_Alloc(pxInstance);
	pxScene->pxRenderer = Renderer_Alloc(pxInstance, pxScene->pxSwapChain);

	Renderer_RebuildEnitityDescriptorSets(pxScene->pxRenderer, pxInstance, pxScene->pxEntities);
}

void Scene_Draw(struct xScene_t* pxScene, struct xInstance_t* pxInstance, struct xTimer_t* pxTimer) {
	xTimeInfo_t* pxTimeInfo = Renderer_GetTimeInfo(pxScene->pxRenderer);
	xViewProjection_t* pxViewProjection = Renderer_GetViewProjection(pxScene->pxRenderer);

	pxTimeInfo->fTime = Timer_GetTime(pxTimer);
	pxTimeInfo->fDeltaTime = Timer_GetDeltaTime(pxTimer);

	Orthographic_Projection(-15.0F, 15.0F, -15.0F, 15.0F, 0.001F, 100.0F, pxViewProjection->xProjection);

	xVec3_t xEye = { 0.0F, 0.0F, -1.0F };
	xVec3_t xCenter = { 0.0F, 0.0F, 0.0F };
	xVec3_t xUp = { 0.0F, 1.0F, 0.0F };
	View_LookAt(xEye, xCenter, xUp, pxViewProjection->xView);

	Renderer_Draw(pxScene->pxRenderer, pxInstance, pxScene->pxSwapChain, pxScene->pxEntities);
}
