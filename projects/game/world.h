#ifndef GAME_WORLD_H
#define GAME_WORLD_H

#include <stdint.h>

struct xInstance_t;
struct xScene_t;
struct xWorld_t;
struct xRenderer_t;
struct xTimer_t;

struct xWorld_t* World_Alloc(struct xInstance_t* pxInstance, struct xScene_t* pxScene);
void World_Free(struct xWorld_t* pxWorld, struct xInstance_t* pxInstance, struct xScene_t* pxScene);

void World_Update(struct xWorld_t* pxWorld, struct xRenderer_t* pxRenderer, struct xTimer_t* pxTimer);

#endif
