#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <container/vector.h>

struct xVector_t {
	uint32_t nValueSize;
	uint32_t nCapacity;
	uint32_t nBufferSize;
	uint32_t nBufferCount;
	void* pBuffer;
	uint32_t nBufferIndex;
	uint32_t nBufferOffset;
};

static void Vector_Expand(struct xVector_t* pxVector) {
	uint32_t nNextBufferCount = pxVector->nBufferCount * 2;
	uint32_t nNextBufferSize = pxVector->nBufferSize * 2;

	void* pStagingBuffer = malloc(nNextBufferSize);

	memcpy(pStagingBuffer, pxVector->pBuffer, pxVector->nBufferSize);

	free(pxVector->pBuffer);

	pxVector->pBuffer = pStagingBuffer;
	pxVector->nBufferCount = nNextBufferCount;
	pxVector->nBufferSize = nNextBufferSize;
}

struct xVector_t* Vector_Alloc(uint32_t nValueSize, uint32_t nCapacity) {
	struct xVector_t* pxVector = (struct xVector_t*)calloc(1, sizeof(struct xVector_t));

	pxVector->nValueSize = nValueSize;
	pxVector->nCapacity = nCapacity;
	pxVector->nBufferSize = nCapacity * nValueSize;
	pxVector->nBufferCount = nCapacity;
	pxVector->pBuffer = malloc(nCapacity * nValueSize);

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
