#ifndef GAME_SCENE_H
#define GAME_SCENE_H

struct xInstance_t;
struct xScene_t;
struct xEntity_t;
struct xTimer_t;

struct xScene_t* Scene_Alloc(struct xInstance_t* pxInstance);
void Scene_Free(struct xScene_t* pxScene, struct xInstance_t* pxInstance);

struct xEntity_t* Scene_CreateEntity(struct xScene_t* pxScene, const char* pcName, struct xEntity_t* pxParent);
void Scene_DestroyEntity(struct xScene_t* pxScene, struct xEntity_t* pxEntity);

void Scene_Resize(struct xScene_t* pxScene, struct xInstance_t* pxInstance);
void Scene_Draw(struct xScene_t* pxScene, struct xInstance_t* pxInstance, struct xTimer_t* pxTimer);

#endif
