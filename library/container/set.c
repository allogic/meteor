#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <container/set.h>

#define DEFAULT_CAPACITY 16

struct xSet_t {
	uint32_t nValueSize;
	uint32_t nBufferSize;
	uint32_t nBufferCount;
	void* pBuffer;
	uint32_t nBufferIndex;
	uint32_t nBufferOffset;
};

static void Set_Expand(struct xSet_t* pxSet) {
	uint32_t nNextBufferCount = pxSet->nBufferCount * 2;
	uint32_t nNextBufferSize = pxSet->nBufferSize * 2;

	pxSet->pBuffer = realloc(pxSet->pBuffer, nNextBufferSize);
	pxSet->nBufferCount = nNextBufferCount;
	pxSet->nBufferSize = nNextBufferSize;
}

struct xSet_t* Set_Alloc(uint32_t nValueSize) {
	struct xSet_t* pxSet = (struct xSet_t*)calloc(1, sizeof(struct xSet_t));

	pxSet->nValueSize = nValueSize;
	pxSet->nBufferSize = DEFAULT_CAPACITY * nValueSize;
	pxSet->nBufferCount = DEFAULT_CAPACITY;
	pxSet->pBuffer = malloc(DEFAULT_CAPACITY * nValueSize);

	return pxSet;
}

void Set_Free(struct xSet_t* pxSet) {
	free(pxSet->pBuffer);
	free(pxSet);
}

uint32_t Set_Push(struct xSet_t* pxSet, void* pData) {
	uint32_t nIndex = 0;

	void* pValue = Set_Find(pxSet, pData, &nIndex);

	if (pValue) {
		return nIndex;
	} else {
		nIndex = pxSet->nBufferIndex;

		memcpy(((uint8_t*)pxSet->pBuffer) + pxSet->nBufferOffset, pData, pxSet->nValueSize);

		pxSet->nBufferIndex += 1;
		pxSet->nBufferOffset += pxSet->nValueSize;

		if (pxSet->nBufferIndex >= pxSet->nBufferCount) {
			Set_Expand(pxSet);
		}

		return nIndex;
	}
}

void Set_Clear(struct xSet_t* pxSet) {
	pxSet->nBufferIndex = 0;
	pxSet->nBufferOffset = 0;
}

void* Set_At(struct xSet_t* pxSet, uint32_t nIndex) {
	return ((uint8_t*)pxSet->pBuffer) + (nIndex * pxSet->nValueSize);
}

void* Set_Data(struct xSet_t* pxSet) {
	return pxSet->pBuffer;
}

void* Set_Find(struct xSet_t* pxSet, void* pData, uint32_t* pnIndex) {
	uint32_t nOffset = 0;

	while (nOffset < pxSet->nBufferOffset) {
		if (memcmp(((uint8_t*)pxSet->pBuffer) + nOffset, pData, pxSet->nValueSize) == 0) {
			return ((uint8_t*)pxSet->pBuffer) + nOffset;
		}

		nOffset += pxSet->nValueSize;

		if (pnIndex) {
			(*pnIndex) += 1;
		}
	}

	return 0;
}

bool Set_Empty(struct xSet_t* pxSet) {
	return pxSet->nBufferIndex == 0;
}

uint32_t Set_Count(struct xSet_t* pxSet) {
	return pxSet->nBufferIndex;
}

uint32_t Set_Size(struct xSet_t* pxSet) {
	return pxSet->nBufferIndex * pxSet->nValueSize;
}
