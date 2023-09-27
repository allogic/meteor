#include <stdlib.h>
#include <string.h>

#include <common/macros.h>

#include <container/list.h>

#include <vulkan/instance.h>
#include <vulkan/buffer.h>
#include <vulkan/image.h>

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
	void* pEntityIter;
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

void Entity_Free(struct xEntity_t* pxEntity, struct xInstance_t* pxInstance) {
	if (pxEntity->wMask & COMPONENT_PIXELAFFECTOR_BIT) {
		xParticleAffector_t* pxParticleAffector = (xParticleAffector_t*)pxEntity->apComponents[COMPONENT_PIXELAFFECTOR_IDX];

		free(pxParticleAffector);
	}

	if (pxEntity->wMask & COMPONENT_PIXELSYSTEM_BIT) {
		xPixelSystem_t* pxPixelSystem = pxEntity->apComponents[COMPONENT_PIXELSYSTEM_IDX];

		if (pxPixelSystem->pxColorImage && !pxPixelSystem->bSharedColorImage) {
			Image_Free(pxPixelSystem->pxColorImage, pxInstance);
		}

		if (pxPixelSystem->pxColorImageN && !pxPixelSystem->bSharedColorImageN) {
			Image_Free(pxPixelSystem->pxColorImageN, pxInstance);
		}

		if (pxPixelSystem->pxColorImageS && !pxPixelSystem->bSharedColorImageS) {
			Image_Free(pxPixelSystem->pxColorImageS, pxInstance);
		}

		if (pxPixelSystem->pxColorImageW && !pxPixelSystem->bSharedColorImageW) {
			Image_Free(pxPixelSystem->pxColorImageW, pxInstance);
		}

		if (pxPixelSystem->pxColorImageE && !pxPixelSystem->bSharedColorImageE) {
			Image_Free(pxPixelSystem->pxColorImageE, pxInstance);
		}

		if (pxPixelSystem->pxStateImage && !pxPixelSystem->bSharedStateImage) {
			Image_Free(pxPixelSystem->pxStateImage, pxInstance);
		}

		if (pxPixelSystem->pxStateImageN && !pxPixelSystem->bSharedStateImageN) {
			Image_Free(pxPixelSystem->pxStateImageN, pxInstance);
		}

		if (pxPixelSystem->pxStateImageS && !pxPixelSystem->bSharedStateImageS) {
			Image_Free(pxPixelSystem->pxStateImageS, pxInstance);
		}

		if (pxPixelSystem->pxStateImageW && !pxPixelSystem->bSharedStateImageW) {
			Image_Free(pxPixelSystem->pxStateImageW, pxInstance);
		}

		if (pxPixelSystem->pxStateImageE && !pxPixelSystem->bSharedStateImageE) {
			Image_Free(pxPixelSystem->pxStateImageE, pxInstance);
		}

		free(pxPixelSystem);
	}

	if (pxEntity->wMask & COMPONENT_PARTICLEAFFECTOR_BIT) {
		xPixelAffector_t* pxPixelAffector = (xPixelAffector_t*)pxEntity->apComponents[COMPONENT_PARTICLEAFFECTOR_IDX];

		free(pxPixelAffector);
	}

	if (pxEntity->wMask & COMPONENT_PARTICLESYSTEM_BIT) {
		xParticleSystem_t* pxParticleSystem = pxEntity->apComponents[COMPONENT_PARTICLESYSTEM_IDX];

		if (pxParticleSystem->pxBehaviourBuffer && !pxParticleSystem->bSharedBehaviourBuffer) {
			Buffer_Free(pxParticleSystem->pxBehaviourBuffer, pxInstance);
		}
		
		if (pxParticleSystem->pxParticleBuffer && !pxParticleSystem->bSharedParticleBuffer) {
			Buffer_Free(pxParticleSystem->pxParticleBuffer, pxInstance);
		}

		free(pxParticleSystem);
	}

	if (pxEntity->wMask & COMPONENT_RENDERABLE_BIT) {
		xRenderable_t* pxRenderable = (xRenderable_t*)pxEntity->apComponents[COMPONENT_RENDERABLE_IDX];

		if (pxRenderable->pxVertexBuffer && !pxRenderable->bSharedVertexBuffer) {
			Buffer_Free(pxRenderable->pxVertexBuffer, pxInstance);
		}

		if (pxRenderable->pxIndexBuffer && !pxRenderable->bSharedIndexBuffer) {
			Buffer_Free(pxRenderable->pxIndexBuffer, pxInstance);
		}

		if (pxRenderable->pxColorImage && !pxRenderable->bSharedColorImage) {
			Image_Free(pxRenderable->pxColorImage, pxInstance);
		}

		free(pxRenderable);
	}

	if (pxEntity->wMask & COMPONENT_RIGIDBODY_BIT) {
		xRigidBody_t* pxRigidBody = (xRigidBody_t*)pxEntity->apComponents[COMPONENT_RIGIDBODY_IDX];

		free(pxRigidBody);
	}

	if (pxEntity->wMask & COMPONENT_CAMERA_BIT) {
		xCamera_t* pxCamera = (xCamera_t*)pxEntity->apComponents[COMPONENT_CAMERA_IDX];

		free(pxCamera);
	}

	if (pxEntity->wMask & COMPONENT_TRANSFORM_BIT) {
		xTransform_t* pxTransform = (xTransform_t*)pxEntity->apComponents[COMPONENT_TRANSFORM_IDX];

		free(pxTransform);
	}

	List_Free(pxEntity->pxChildren);

	free(pxEntity);
}

void* Entity_GetEntityIter(struct xEntity_t* pxEntity) {
	return pxEntity->pEntityIter;
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

void Entity_SetEntityIter(struct xEntity_t* pxEntity, void* pEntityIter) {
	pxEntity->pEntityIter = pEntityIter;
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
