#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <container/queue.h>

struct xQueue_t {
	uint32_t nValueSize;
	uint32_t nRelocCount;
	uint32_t nBufferSize;
	uint32_t nBufferCount;
	void* pBuffer;
	uint32_t nWriteIndex;
	uint32_t nWriteOffset;
	uint32_t nReadIndex;
	uint32_t nReadOffset;
};

static void Queue_Expand(struct xQueue_t* pxQueue) {
	void* pStagingBuffer = malloc((pxQueue->nBufferCount + pxQueue->nRelocCount) * pxQueue->nValueSize);

	memcpy(pStagingBuffer, pxQueue->pBuffer, pxQueue->nBufferSize);

	free(pxQueue->pBuffer);

	pxQueue->pBuffer = pStagingBuffer;

	uint32_t* pnSrc = ((uint32_t*)pxQueue->pBuffer) + pxQueue->nBufferCount - 1;
	uint32_t* pnDst = ((uint32_t*)pxQueue->pBuffer) + pxQueue->nBufferCount + pxQueue->nRelocCount - 1;

	for (uint32_t i = 0; i < (pxQueue->nBufferCount - pxQueue->nReadIndex); ++i) {
		*(pnDst - i) = *(pnSrc - i);
	}

	pxQueue->nReadIndex += pxQueue->nRelocCount;
	pxQueue->nReadOffset += pxQueue->nRelocCount * pxQueue->nValueSize;

	pxQueue->nBufferCount += pxQueue->nRelocCount;
	pxQueue->nBufferSize += pxQueue->nRelocCount * pxQueue->nValueSize;
}

struct xQueue_t* Queue_Alloc(uint32_t nValueSize, uint32_t nRelocCount) {
	struct xQueue_t* pxQueue = (struct xQueue_t*)calloc(1, sizeof(struct xQueue_t));

	pxQueue->nValueSize = nValueSize;
	pxQueue->nRelocCount = nRelocCount;
	pxQueue->nBufferSize = nRelocCount * nValueSize;
	pxQueue->nBufferCount = nRelocCount;
	pxQueue->pBuffer = malloc(nRelocCount * nValueSize);

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

void Queue_Dump(struct xQueue_t* pxQueue) {
	printf("nValueSize:%u\n", pxQueue->nValueSize);
	printf("nRelocCount:%u\n", pxQueue->nRelocCount);
	printf("nBufferSize:%u\n", pxQueue->nBufferSize);
	printf("nBufferCount:%u\n", pxQueue->nBufferCount);
	printf("nWriteIndex:%u\n", pxQueue->nWriteIndex);
	printf("nWriteOffset:%u\n", pxQueue->nWriteOffset);
	printf("nReadIndex:%u\n", pxQueue->nReadIndex);
	printf("nReadOffset:%u\n", pxQueue->nReadOffset);
}
