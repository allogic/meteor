#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <container/vector.h>

struct xVector_t {
	uint32_t nValueSize;
	uint32_t nRelocCount;
	uint32_t nBufferSize;
	uint32_t nBufferCount;
	void* pBuffer;
	uint32_t nBufferIndex;
	uint32_t nBufferOffset;
};

static void Vector_Expand(struct xVector_t* pxVector) {
	void* pStagingBuffer = malloc((pxVector->nBufferCount + pxVector->nRelocCount) * pxVector->nValueSize);

	memcpy(pStagingBuffer, pxVector->pBuffer, pxVector->nBufferSize);

	free(pxVector->pBuffer);

	pxVector->pBuffer = pStagingBuffer;

	pxVector->nBufferCount += pxVector->nRelocCount;
	pxVector->nBufferSize += pxVector->nRelocCount * pxVector->nValueSize;
}

struct xVector_t* Vector_Alloc(uint32_t nValueSize, uint32_t nRelocCount) {
	struct xVector_t* pxVector = (struct xVector_t*)calloc(1, sizeof(struct xVector_t));

	pxVector->nValueSize = nValueSize;
	pxVector->nRelocCount = nRelocCount;
	pxVector->nBufferSize = nRelocCount * nValueSize;
	pxVector->nBufferCount = nRelocCount;
	pxVector->pBuffer = malloc(nRelocCount * nValueSize);

	return pxVector;
}

void Vector_Free(struct xVector_t* pxVector) {
	free(pxVector->pBuffer);
	free(pxVector);
}

uint32_t Vector_Push(struct xVector_t* pxVector, void* pData) {
	uint32_t nIndex = pxVector->nBufferIndex;

	memcpy(((uint8_t*)pxVector->pBuffer) + pxVector->nBufferOffset, pData, pxVector->nValueSize);

	pxVector->nBufferIndex += 1;
	pxVector->nBufferOffset += pxVector->nValueSize;

	if (pxVector->nBufferIndex >= pxVector->nBufferCount) {
		Vector_Expand(pxVector);
	}

	return nIndex;
}

void* Vector_Data(struct xVector_t* pxVector) {
	return pxVector->pBuffer;
}

bool Vector_Empty(struct xVector_t* pxVector) {
	return pxVector->nBufferIndex == 0;
}

uint32_t Vector_Count(struct xVector_t* pxVector) {
	return pxVector->nBufferIndex;
}
