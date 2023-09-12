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

static void Queue_Reloc(struct xQueue_t* pxQueue) {
	void* pStagingBuffer = calloc(pxQueue->nBufferCount + pxQueue->nRelocCount, pxQueue->nValueSize);

	memcpy(pStagingBuffer, pxQueue->pBuffer, pxQueue->nBufferSize);

	free(pxQueue->pBuffer);

	pxQueue->pBuffer = pStagingBuffer;

	uint32_t nReadCountDelta = pxQueue->nRelocCount;
	uint32_t nReadSizeDelta = pxQueue->nRelocCount * pxQueue->nValueSize;

	memcpy(((uint8_t*)pxQueue->pBuffer) + pxQueue->nReadOffset + nReadSizeDelta, ((uint8_t*)pxQueue->pBuffer) + pxQueue->nReadOffset, nReadSizeDelta);

	pxQueue->nReadIndex += nReadCountDelta;
	pxQueue->nReadOffset += nReadSizeDelta;

	pxQueue->nBufferSize += pxQueue->nRelocCount * pxQueue->nValueSize;
	pxQueue->nBufferCount += pxQueue->nRelocCount;
}

struct xQueue_t* Queue_Alloc(uint32_t nValueSize, uint32_t nRelocCount) {
	struct xQueue_t* pxQueue = (struct xQueue_t*)calloc(1, sizeof(struct xQueue_t));

	pxQueue->nValueSize = nValueSize;
	pxQueue->nRelocCount = nRelocCount;
	pxQueue->nBufferSize = nRelocCount * nValueSize;
	pxQueue->nBufferCount = nRelocCount;
	pxQueue->pBuffer = calloc(nRelocCount, nValueSize);

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

	if (pxQueue->nWriteIndex >= pxQueue->nBufferCount)
	{
		pxQueue->nWriteIndex = 0;
		pxQueue->nWriteOffset = 0;
	}

	if (pxQueue->nWriteIndex == pxQueue->nReadIndex) {
		Queue_Reloc(pxQueue);
	}
}

void Queue_Pop(struct xQueue_t* pxQueue, void* pData) {
	memcpy(pData, ((uint8_t*)pxQueue->pBuffer) + pxQueue->nReadOffset, pxQueue->nValueSize);

	pxQueue->nReadIndex += 1;
	pxQueue->nReadOffset += pxQueue->nValueSize;

	if (pxQueue->nReadIndex >= pxQueue->nBufferCount)
	{
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
