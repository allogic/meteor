#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <standard/queue.h>

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

	// [ooxxxxxx]
	// [ooxxxxxx........]
	//    R
	//    W
	// [oo........xxxxxx]
	//    W       R

	// [xxxx]
	// [xxxx....]
	//  R
	//  W
	// [....xxxx]
	//  W   R
	// [XXXXXXXX....]
	//      R
	//      W
	// [XXXX....XXXX]
	//      W   R

	uint32_t nBufferCountDelta = pxQueue->nRelocCount;
	uint32_t nBufferSizeDelta = pxQueue->nRelocCount * pxQueue->nValueSize;

	//uint32_t nNextReadIndex = pxQueue->nBufferCount + pxQueue->nReadIndex;
	//uint32_t nNextReadOffset = pxQueue->nBufferSize + pxQueue->nReadOffset;

	printf("ReadDeltaOffset:%u\n", nReadDeltaOffset);
	//printf("NextReadIndex:%u NextReadOffset:%u\n", nNextReadIndex, nNextReadOffset);

	memcpy(((uint8_t*)pxQueue->pBuffer) + pxQueue->nReadOffset + nReadDeltaOffset, ((uint8_t*)pxQueue->pBuffer) + pxQueue->nReadOffset, nReadDeltaOffset);

	//pxQueue->nReadIndex = nNextReadIndex;
	pxQueue->nReadOffset = nNextReadOffset;

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

uint32_t Queue_GetReadIndex(struct xQueue_t* pxQueue) {
	return pxQueue->nReadIndex;
}

uint32_t Queue_GetReadOffset(struct xQueue_t* pxQueue) {
	return pxQueue->nReadOffset;
}

uint32_t Queue_GetWriteIndex(struct xQueue_t* pxQueue) {
	return pxQueue->nWriteIndex;
}

uint32_t Queue_GetWriteOffset(struct xQueue_t* pxQueue) {
	return pxQueue->nWriteOffset;
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
		printf("Reloc push\n");
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

	if (pxQueue->nReadIndex == pxQueue->nWriteIndex) {
		printf("Reloc pop\n");
	}
}

bool Queue_Empty(struct xQueue_t* pxQueue) {
	return pxQueue->nReadIndex == pxQueue->nWriteIndex;
}

uint32_t Queue_Count(struct xQueue_t* pxQueue) {
	return (pxQueue->nWriteIndex + pxQueue->nBufferCount - pxQueue->nReadIndex) % pxQueue->nBufferCount;
}

void Queue_Dump(struct xQueue_t* pxQueue) {
	for (uint32_t i = 0; i < pxQueue->nBufferCount; i++) {
		printf("Index:%u Value:%u\n", i, *(((uint8_t*)pxQueue->pBuffer) + i * pxQueue->nValueSize));
	}
}
