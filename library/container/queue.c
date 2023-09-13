#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <container/queue.h>

struct xQueue_t {
	uint32_t nValueSize;
	uint32_t nCapacity;
	uint32_t nBufferSize;
	uint32_t nBufferCount;
	void* pBuffer;
	uint32_t nWriteIndex;
	uint32_t nWriteOffset;
	uint32_t nReadIndex;
	uint32_t nReadOffset;
};

static void Queue_Expand(struct xQueue_t* pxQueue) {
	uint32_t nNextBufferCount = pxQueue->nBufferCount * 2;
	uint32_t nNextBufferSize = pxQueue->nBufferSize * 2;

	void* pStagingBuffer = malloc(nNextBufferSize);

	memcpy(pStagingBuffer, pxQueue->pBuffer, pxQueue->nBufferSize);

	free(pxQueue->pBuffer);

	pxQueue->pBuffer = pStagingBuffer;

	uint32_t* pnSrc = ((uint32_t*)pxQueue->pBuffer) + pxQueue->nBufferCount - 1;
	uint32_t* pnDst = ((uint32_t*)pxQueue->pBuffer) + nNextBufferCount - 1;

	for (uint32_t i = 0; i < (pxQueue->nBufferCount - pxQueue->nReadIndex); ++i) {
		*(pnDst - i) = *(pnSrc - i);
	}

	pxQueue->nReadIndex += pxQueue->nBufferCount;
	pxQueue->nReadOffset += pxQueue->nBufferSize;
	pxQueue->nBufferCount = nNextBufferCount;
	pxQueue->nBufferSize = nNextBufferSize;
}

struct xQueue_t* Queue_Alloc(uint32_t nValueSize, uint32_t nCapacity) {
	struct xQueue_t* pxQueue = (struct xQueue_t*)calloc(1, sizeof(struct xQueue_t));

	pxQueue->nValueSize = nValueSize;
	pxQueue->nCapacity = nCapacity;
	pxQueue->nBufferSize = nCapacity * nValueSize;
	pxQueue->nBufferCount = nCapacity;
	pxQueue->pBuffer = malloc(nCapacity * nValueSize);

	return pxQueue;
}

void Queue_Free(struct xQueue_t* pxQueue) {
	free(pxQueue->pBuffer);
	free(pxQueue);
}

void Queue_Push(struct xQueue_t* pxQueue, void* pData) {
	memcpy(((uint8_t*)pxQueue->pBuffer) + pxQueue->nWriteOffset, pData, pxQueue->nValueSize);

	pxQueue->nWriteIndex += 1;
	pxQueue->nWriteOffset += pxQueue->nValueSize;

	if (pxQueue->nWriteIndex >= pxQueue->nBufferCount) {
		pxQueue->nWriteIndex = 0;
		pxQueue->nWriteOffset = 0;
	}

	if (pxQueue->nWriteIndex == pxQueue->nReadIndex) {
		Queue_Expand(pxQueue);
	}
}

void Queue_Pop(struct xQueue_t* pxQueue, void* pData) {
	memcpy(pData, ((uint8_t*)pxQueue->pBuffer) + pxQueue->nReadOffset, pxQueue->nValueSize);

	pxQueue->nReadIndex += 1;
	pxQueue->nReadOffset += pxQueue->nValueSize;

	if (pxQueue->nReadIndex >= pxQueue->nBufferCount) {
		pxQueue->nReadIndex = 0;
		pxQueue->nReadOffset = 0;
	}
}

bool Queue_Empty(struct xQueue_t* pxQueue) {
	return pxQueue->nReadIndex == pxQueue->nWriteIndex;
}

uint32_t Queue_Count(struct xQueue_t* pxQueue) {
	return (pxQueue->nWriteIndex + pxQueue->nBufferCount - pxQueue->nReadIndex) % pxQueue->nBufferCount;
}
