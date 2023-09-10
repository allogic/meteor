#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/timer.h>

#include <standard/list.h>

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

struct xScene_t {
	struct xList_t* pxEntities;
	struct xSwapChain_t* pxSwapChain;
	struct xRenderer_t* pxRenderer;
};

struct xScene_t* Scene_Alloc(struct xInstance_t* pxInstance) {
	struct xScene_t* pxScene = (struct xScene_t*)calloc(1, sizeof(struct xScene_t));

	pxScene->pxEntities = List_Alloc();
	pxScene->pxSwapChain = SwapChain_Alloc(pxInstance);
	pxScene->pxRenderer = Renderer_Alloc(pxInstance, pxScene->pxSwapChain);

	return pxScene;
}

void Scene_Free(struct xScene_t* pxScene, struct xInstance_t* pxInstance) {
	// TODO: Destroy entities..

	Renderer_Free(pxScene->pxRenderer, pxInstance);
	SwapChain_Free(pxScene->pxSwapChain, pxInstance);
	List_Free(pxScene->pxEntities);

	free(pxScene);
}

struct xEntity_t* Scene_CreateEntity(struct xScene_t* pxScene, const char* pcName, struct xEntity_t* pxParent) {
	struct xEntity_t* pxEntity = Entity_Alloc(pcName, pxParent);

	List_Push(pxScene->pxEntities, &pxEntity, sizeof(struct xEntity_t*));

	return pxEntity;
}

void Scene_DestroyEntity(struct xScene_t* pxScene, struct xEntity_t* pxEntity) {

}

void Scene_Resize(struct xScene_t* pxScene, struct xInstance_t* pxInstance) {
	Renderer_Free(pxScene->pxRenderer, pxInstance);
	SwapChain_Free(pxScene->pxSwapChain, pxInstance);

	pxScene->pxSwapChain = SwapChain_Alloc(pxInstance);
	pxScene->pxRenderer = Renderer_Alloc(pxInstance, pxScene->pxSwapChain);
}

void Scene_Draw(struct xScene_t* pxScene, struct xInstance_t* pxInstance, struct xTimer_t* pxTimer) {
	xTimeInfo_t* pxTimeInfo = Renderer_GetTimeInfo(pxScene->pxRenderer);
	xViewProjection_t* pxViewProjection = Renderer_GetViewProjection(pxScene->pxRenderer);

	pxTimeInfo->fTime = Timer_GetTime(pxTimer);
	pxTimeInfo->fDeltaTime = Timer_GetDeltaTime(pxTimer);

	Orthographic_Projection(-50.0F, 50.0F, -50.0F, 50.0F, 0.001F, 100.0F, pxViewProjection->xProjection);

	xVec3_t xEye = { 0.0F, 0.0F, -1.0F };
	xVec3_t xCenter = { 0.0F, 0.0F, 0.0F };
	xVec3_t xUp = { 0.0F, 1.0F, 0.0F };
	View_LookAt(xEye, xCenter, xUp, pxViewProjection->xView);

	Renderer_Draw(pxScene->pxRenderer, pxInstance, pxScene->pxSwapChain, pxScene->pxEntities);
}
