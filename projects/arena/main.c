#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <container/list.h>
#include <container/vector.h>

#include <debug/stacktrace.h>

struct xEntity_t {
	void* pEntityIter;
	void* pUsedIndexIter;
};

static struct xList_t* s_xEntities;
static struct xList_t* s_xUsedIndices;
static struct xList_t* s_xFreeIndices;

static struct xVector_t* s_xTransforms;

static struct xEntity_t* Insert(void) {
	if (!List_Empty(s_xFreeIndices)) {
		// Remove free transform index
		void* pFreeIndexIter = List_Begin(s_xFreeIndices);
		uint32_t nTransformIndex = *(uint32_t*)List_Value(pFreeIndexIter);
		List_Remove(s_xFreeIndices, pFreeIndexIter);

		// Transform index is now in use
		void* pUsedIndexIter = List_Add(s_xUsedIndices, &nTransformIndex);

		// Create new entity
		void* pEntityIter = List_Add(s_xEntities, 0);
		struct xEntity_t* pxEntity = (struct xEntity_t*)List_Value(pEntityIter);
		pxEntity->pEntityIter = pEntityIter;
		pxEntity->pUsedIndexIter = pUsedIndexIter;

		return pxEntity;
	}

	return 0;
}

static void Remove(struct xEntity_t* pxEntity) {
	// Remove used transform index
	uint32_t nTransformIndex = *(uint32_t*)List_Value(pxEntity->pUsedIndexIter);
	List_Remove(s_xUsedIndices, pxEntity->pUsedIndexIter);

	// Transform index is not in use anymore
	List_Add(s_xFreeIndices, &nTransformIndex);

	// Remove entity
	List_Remove(s_xEntities, pxEntity->pEntityIter);
}

static void Dump(void) {
	printf("================================\n");

	printf("Free Indices\n");
	void* pFreeIndexIter = List_Begin(s_xFreeIndices);
	while (pFreeIndexIter) {
		printf("  %u\n", *(uint32_t*)List_Value(pFreeIndexIter));
		pFreeIndexIter = List_Next(pFreeIndexIter);
	}

	printf("Used Indices\n");
	void* pUsedIndexIter = List_Begin(s_xUsedIndices);
	while (pUsedIndexIter) {
		printf("  %u\n", *(uint32_t*)List_Value(pUsedIndexIter));
		pUsedIndexIter = List_Next(pUsedIndexIter);
	}

	printf("Entities\n");
	void* pEntityIter = List_Begin(s_xEntities);
	while (pEntityIter) {
		struct xEntity_t* pxEntity = (struct xEntity_t*)List_Value(pEntityIter);
		printf("  %p\n", pxEntity->pEntityIter);
		printf("  %u\n", *(uint32_t*)List_Value(pxEntity->pUsedIndexIter));
		pEntityIter = List_Next(pEntityIter);
	}
}

int32_t main(void) {

#ifdef DEBUG
	StackTrace_Alloc();
#endif

	s_xEntities = List_Alloc(sizeof(struct xEntity_t));
	s_xUsedIndices = List_Alloc(sizeof(uint32_t));
	s_xFreeIndices = List_Alloc(sizeof(uint32_t));

	s_xTransforms = Vector_Alloc(sizeof(uint32_t));

	for (uint32_t i = 0; i < 4; ++i) {
		List_Add(s_xFreeIndices, &i);
	}

	Dump();

	struct xEntity_t* pxEntity0 = Insert();
	struct xEntity_t* pxEntity1 = Insert();
	struct xEntity_t* pxEntity2 = Insert();
	struct xEntity_t* pxEntity3 = Insert();

	Dump();

	Remove(pxEntity0);
	Remove(pxEntity3);

	Dump();

	pxEntity0 = Insert();
	pxEntity3 = Insert();

	Dump();

	Remove(pxEntity0);
	Remove(pxEntity1);
	Remove(pxEntity2);
	Remove(pxEntity3);

	Dump();

	Vector_Free(s_xTransforms);

	List_Free(s_xFreeIndices);
	List_Free(s_xUsedIndices);
	List_Free(s_xEntities);

#ifdef DEBUG
	StackTrace_Free();
#endif

	return 0;
}
