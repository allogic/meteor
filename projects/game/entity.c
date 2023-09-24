#include <stdlib.h>
#include <string.h>

#include <common/macros.h>

#include <container/list.h>

#include <game/entity.h>

#define ENTITY_NAME_LENGTH 32

#define SETUP_COMPONENT_AND_COPY_IF(NIBBLE, INDEX, TYPE, POINTER) \
	if ((pxEntity->wMask & NIBBLE) == 0) { \
		pxEntity->wMask |= NIBBLE; \
		pxEntity->apComponents[INDEX] = calloc(1, sizeof(TYPE)); \
	} \
	if (POINTER) { \
		memcpy(pxEntity->apComponents[INDEX], POINTER, sizeof(TYPE)); \
	}

struct xEntity_t {
	char acName[ENTITY_NAME_LENGTH];
	struct xEntity_t* pxParent;
	struct xList_t* pxChildren;
	uint64_t wMask;
	void* apComponents[64];
};

struct xEntity_t* Entity_Alloc(const char* pcName, struct xEntity_t* pxParent) {
	struct xEntity_t* pxEntity = (struct xEntity_t*)calloc(1, sizeof(struct xEntity_t));

	uint32_t nNameLength = strlen(pcName);
	memcpy(pxEntity->acName, pcName, MIN(ENTITY_NAME_LENGTH - 1, nNameLength));
	
	pxEntity->acName[ENTITY_NAME_LENGTH - 1] = 0;
	pxEntity->pxParent = pxParent;
	pxEntity->pxChildren = List_Alloc(sizeof(struct xEntity_t*));

	return pxEntity;
}

void Entity_Free(struct xEntity_t* pxEntity) {
	if (pxEntity->wMask & COMPONENT_PIXELAFFECTOR_BIT) {
		free(pxEntity->apComponents[COMPONENT_PIXELAFFECTOR_IDX]);
	}

	if (pxEntity->wMask & COMPONENT_PIXELSYSTEM_BIT) {
		free(pxEntity->apComponents[COMPONENT_PIXELSYSTEM_IDX]);
	}

	if (pxEntity->wMask & COMPONENT_PARTICLEAFFECTOR_BIT) {
		free(pxEntity->apComponents[COMPONENT_PARTICLEAFFECTOR_IDX]);
	}

	if (pxEntity->wMask & COMPONENT_PARTICLESYSTEM_BIT) {
		free(pxEntity->apComponents[COMPONENT_PARTICLESYSTEM_IDX]);
	}

	if (pxEntity->wMask & COMPONENT_RENDERABLE_BIT) {
		free(pxEntity->apComponents[COMPONENT_RENDERABLE_IDX]);
	}

	if (pxEntity->wMask & COMPONENT_RIGIDBODY_BIT) {
		free(pxEntity->apComponents[COMPONENT_RIGIDBODY_IDX]);
	}

	if (pxEntity->wMask & COMPONENT_CAMERA_BIT) {
		free(pxEntity->apComponents[COMPONENT_CAMERA_IDX]);
	}

	if (pxEntity->wMask & COMPONENT_TRANSFORM_BIT) {
		free(pxEntity->apComponents[COMPONENT_TRANSFORM_IDX]);
	}

	List_Free(pxEntity->pxChildren);

	free(pxEntity);
}

const char* Entity_GetName(struct xEntity_t* pxEntity) {
	return pxEntity->acName;
}

uint64_t Entity_GetMask(struct xEntity_t* pxEntity) {
	return pxEntity->wMask;
}

xTransform_t* Entity_GetTransform(struct xEntity_t* pxEntity) {
	return pxEntity->apComponents[COMPONENT_TRANSFORM_IDX];
}

xCamera_t* Entity_GetCamera(struct xEntity_t* pxEntity) {
	return pxEntity->apComponents[COMPONENT_CAMERA_IDX];
}

xRigidBody_t* Entity_GetRigidbody(struct xEntity_t* pxEntity) {
	return pxEntity->apComponents[COMPONENT_RIGIDBODY_IDX];
}

xRenderable_t* Entity_GetRenderable(struct xEntity_t* pxEntity) {
	return pxEntity->apComponents[COMPONENT_RENDERABLE_IDX];
}

xParticleSystem_t* Entity_GetParticleSystem(struct xEntity_t* pxEntity) {
	return pxEntity->apComponents[COMPONENT_PARTICLESYSTEM_IDX];
}

xParticleAffector_t* Entity_GetParticleAffector(struct xEntity_t* pxEntity) {
	return pxEntity->apComponents[COMPONENT_PARTICLEAFFECTOR_IDX];
}

xPixelSystem_t* Entity_GetPixelSystem(struct xEntity_t* pxEntity) {
	return pxEntity->apComponents[COMPONENT_PIXELSYSTEM_IDX];
}

xPixelAffector_t* Entity_GetPixelAffector(struct xEntity_t* pxEntity) {
	return pxEntity->apComponents[COMPONENT_PIXELAFFECTOR_IDX];
}

void Entity_SetName(struct xEntity_t* pxEntity, const char* pcName) {
	memset(pxEntity->acName, 0, ENTITY_NAME_LENGTH);
	uint32_t nNameLength = strlen(pcName);
	memcpy(pxEntity->acName, pcName, MIN(ENTITY_NAME_LENGTH - 1, nNameLength));
	pxEntity->acName[ENTITY_NAME_LENGTH - 1] = 0;
}

void Entity_SetTransform(struct xEntity_t* pxEntity, xTransform_t* pxTransform) {
	SETUP_COMPONENT_AND_COPY_IF(COMPONENT_TRANSFORM_BIT, COMPONENT_TRANSFORM_IDX, xTransform_t, pxTransform)
}

void Entity_SetCamera(struct xEntity_t* pxEntity, xCamera_t* pxCamera) {
	SETUP_COMPONENT_AND_COPY_IF(COMPONENT_CAMERA_BIT, COMPONENT_CAMERA_IDX, xCamera_t, pxCamera)
}

void Entity_SetRigidbody(struct xEntity_t* pxEntity, xRigidBody_t* pxRigidBody) {
	SETUP_COMPONENT_AND_COPY_IF(COMPONENT_RIGIDBODY_BIT, COMPONENT_RIGIDBODY_IDX, xRigidBody_t, pxRigidBody)
}

void Entity_SetRenderable(struct xEntity_t* pxEntity, xRenderable_t* pxRenderable) {
	SETUP_COMPONENT_AND_COPY_IF(COMPONENT_RENDERABLE_BIT, COMPONENT_RENDERABLE_IDX, xRenderable_t, pxRenderable)
}

void Entity_SetParticleSystem(struct xEntity_t* pxEntity, xParticleSystem_t* pxParticleSystem) {
	SETUP_COMPONENT_AND_COPY_IF(COMPONENT_PARTICLESYSTEM_BIT, COMPONENT_PARTICLESYSTEM_IDX, xParticleSystem_t, pxParticleSystem)
}

void Entity_SetParticleAffector(struct xEntity_t* pxEntity, xParticleAffector_t* pxParticleAffector) {
	SETUP_COMPONENT_AND_COPY_IF(COMPONENT_PARTICLEAFFECTOR_BIT, COMPONENT_PARTICLEAFFECTOR_IDX, xParticleAffector_t, pxParticleAffector)
}

void Entity_SetPixelSystem(struct xEntity_t* pxEntity, xPixelSystem_t* pxPixelSystem) {
	SETUP_COMPONENT_AND_COPY_IF(COMPONENT_PIXELSYSTEM_BIT, COMPONENT_PIXELSYSTEM_IDX, xPixelSystem_t, pxPixelSystem)
}

void Entity_SetPixelAffector(struct xEntity_t* pxEntity, xPixelAffector_t* pxPixelAffector) {
	SETUP_COMPONENT_AND_COPY_IF(COMPONENT_PIXELAFFECTOR_BIT, COMPONENT_PIXELAFFECTOR_IDX, xPixelAffector_t, pxPixelAffector)
}

bool Entity_HasComponents(struct xEntity_t* pxEntity, uint64_t wMask) {
	return (pxEntity->wMask & wMask) == wMask;
}
