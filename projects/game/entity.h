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
xParticleAffector_t* Entity_GetParticleAffector(struct xEntity_t* pxEntity);
xPixelSystem_t* Entity_GetPixelSystem(struct xEntity_t* pxEntity);
xPixelAffector_t* Entity_GetPixelAffector(struct xEntity_t* pxEntity);

void Entity_SetName(struct xEntity_t* pxEntity, const char* pcName);
void Entity_SetTransform(struct xEntity_t* pxEntity, xTransform_t* pxTransform);
void Entity_SetCamera(struct xEntity_t* pxEntity, xCamera_t* pxCamera);
void Entity_SetRigidbody(struct xEntity_t* pxEntity, xRigidBody_t* pxRigidBody);
void Entity_SetRenderable(struct xEntity_t* pxEntity, xRenderable_t* pxRenderable);
void Entity_SetParticleSystem(struct xEntity_t* pxEntitiy, xParticleSystem_t* pxParticleSystem);
void Entity_SetParticleAffector(struct xEntity_t* pxEntitiy, xParticleAffector_t* pxParticleAffector);
void Entity_SetPixelSystem(struct xEntity_t* pxEntity, xPixelSystem_t* pxPixelSystem);
void Entity_SetPixelAffector(struct xEntity_t* pxEntity, xPixelAffector_t* pxPixelAffector);

bool Entity_HasComponents(struct xEntity_t* pxEntity, uint64_t wMask);

#endif
