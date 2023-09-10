#ifndef GAME_ENTITY_H
#define GAME_ENTITY_H

#include <stdint.h>
#include <stdbool.h>

#include <game/component.h>

struct xEntity_t;

struct xEntity_t* Entity_Alloc(const char* pcName, struct xEntity_t* pxParent);
void Entity_Free(struct xEntity_t* pxEntity);

const char* Entity_GetName(struct xEntity_t* pxEntity);
uint64_t Entity_GetMask(struct xEntity_t* pxEntity);
xTransform_t* Entity_GetTransform(struct xEntity_t* pxEntity);
xCamera_t* Entity_GetCamera(struct xEntity_t* pxEntity);
xRigidBody_t* Entity_GetRigidbody(struct xEntity_t* pxEntity);
xRenderable_t* Entity_GetRenderable(struct xEntity_t* pxEntity);
xParticleSystem_t* Entity_GetParticleSystem(struct xEntity_t* pxEntity);

void Entity_SetName(struct xEntity_t* pxEntity, const char* pcName);
void Entity_SetTransform(struct xEntity_t* pxEntity, xTransform_t* pxTransform);
void Entity_SetCamera(struct xEntity_t* pxEntity, xCamera_t* pxCamera);
void Entity_SetRigidbody(struct xEntity_t* pxEntity, xRigidBody_t* pxRigidBody);
void Entity_SetRenderable(struct xEntity_t* pxEntity, xRenderable_t* pxRenderable);
void Entity_SetParticleSystem(struct xEntity_t* pxEntitiy, xParticleSystem_t* pxParticleSystem);

bool Entity_HasComponents(struct xEntity_t* pxEntity, uint64_t wMask);

#endif
