#include <stdlib.h>
#include <string.h>

#include <common/macros.h>

#include <container/list.h>

#include <game/entity.h>

#define ENTITY_NAME_LENGTH 32

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
	pxEntity->pxChildren = List_Alloc();

	return pxEntity;
}

void Entity_Free(struct xEntity_t* pxEntity) {
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

void Entity_SetName(struct xEntity_t* pxEntity, const char* pcName) {
	memset(pxEntity->acName, 0, ENTITY_NAME_LENGTH);
	uint32_t nNameLength = strlen(pcName);
	memcpy(pxEntity->acName, pcName, MIN(ENTITY_NAME_LENGTH - 1, nNameLength));
	pxEntity->acName[ENTITY_NAME_LENGTH - 1] = 0;
}

void Entity_SetTransform(struct xEntity_t* pxEntity, xTransform_t* pxTransform) {
	if ((pxEntity->wMask & COMPONENT_TRANSFORM_BIT) == 0) {
		pxEntity->wMask |= COMPONENT_TRANSFORM_BIT;
		pxEntity->apComponents[COMPONENT_TRANSFORM_IDX] = calloc(1, sizeof(xTransform_t));
	}

	if (pxTransform) {
		memcpy(pxEntity->apComponents[COMPONENT_TRANSFORM_IDX], pxTransform, sizeof(xTransform_t));
	}
}

void Entity_SetCamera(struct xEntity_t* pxEntity, xCamera_t* pxCamera) {
	if ((pxEntity->wMask & COMPONENT_CAMERA_BIT) == 0) {
		pxEntity->wMask |= COMPONENT_CAMERA_BIT;
		pxEntity->apComponents[COMPONENT_CAMERA_IDX] = calloc(1, sizeof(xCamera_t));
	}

	if (pxCamera) {
		memcpy(pxEntity->apComponents[COMPONENT_CAMERA_IDX], pxCamera, sizeof(xCamera_t));
	}
}

void Entity_SetRigidbody(struct xEntity_t* pxEntity, xRigidBody_t* pxRigidBody) {
	if ((pxEntity->wMask & COMPONENT_RIGIDBODY_BIT) == 0) {
		pxEntity->wMask |= COMPONENT_RIGIDBODY_BIT;
		pxEntity->apComponents[COMPONENT_RIGIDBODY_IDX] = calloc(1, sizeof(xRigidBody_t));
	}

	if (pxRigidBody) {
		memcpy(pxEntity->apComponents[COMPONENT_RIGIDBODY_IDX], pxRigidBody, sizeof(xRigidBody_t));
	}
}

void Entity_SetRenderable(struct xEntity_t* pxEntity, xRenderable_t* pxRenderable) {
	if ((pxEntity->wMask & COMPONENT_RENDERABLE_BIT) == 0) {
		pxEntity->wMask |= COMPONENT_RENDERABLE_BIT;
		pxEntity->apComponents[COMPONENT_RENDERABLE_IDX] = calloc(1, sizeof(xRenderable_t));
	}

	if (pxRenderable) {
		memcpy(pxEntity->apComponents[COMPONENT_RENDERABLE_IDX], pxRenderable, sizeof(xRenderable_t));
	}
}

void Entity_SetParticleSystem(struct xEntity_t* pxEntity, xParticleSystem_t* pxParticleSystem) {
	if ((pxEntity->wMask & COMPONENT_PARTICLESYSTEM_BIT) == 0) {
		pxEntity->wMask |= COMPONENT_PARTICLESYSTEM_BIT;
		pxEntity->apComponents[COMPONENT_PARTICLESYSTEM_IDX] = calloc(1, sizeof(xParticleSystem_t));
	}

	if (pxParticleSystem) {
		memcpy(pxEntity->apComponents[COMPONENT_PARTICLESYSTEM_IDX], pxParticleSystem, sizeof(xParticleSystem_t));
	}
}

bool Entity_HasComponents(struct xEntity_t* pxEntity, uint64_t wMask) {
	return (pxEntity->wMask & wMask) == wMask;
}
