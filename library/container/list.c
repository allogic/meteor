#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <container/list.h>

struct xNode_t {
	struct xNode_t* pxNext;
	uint32_t nIndex;
	void* pBuffer;
};

struct xList_t {
	uint32_t nValueSize;
	uint32_t nRelocCount;
	uint32_t nBufferSize;
	uint32_t nBufferCount;
	struct xNode_t* pxBuffer;
	uint32_t nBufferIndex;
	struct xNode_t* pxHead;
	struct xNode_t* pxCurr;
	struct xNode_t* pxTail;
	uint32_t nCount;
};

static void List_Reloc(struct xList_t* pxList) {
	struct xNode_t* pxStagingBuffer = (struct xNode_t*)calloc(pxList->nBufferCount + pxList->nRelocCount, sizeof(struct xList_t));

	for (uint32_t i = 0; i < pxList->nRelocCount; ++i) {
		pxStagingBuffer[pxList->nBufferCount + i].pBuffer = malloc(pxList->nValueSize);
	}

	memcpy(pxStagingBuffer, pxList->pxBuffer, pxList->nBufferCount);

	free(pxList->pxBuffer);

	pxList->pxBuffer = pxStagingBuffer;

	pxList->nBufferSize += pxList->nRelocCount * sizeof(struct xNode_t);
	pxList->nBufferCount += pxList->nRelocCount;
}

struct xList_t* List_Alloc(uint32_t nValueSize, uint32_t nRelocCount) {
	struct xList_t* pxList = (struct xList_t*)calloc(1, sizeof(struct xList_t));

	pxList->nValueSize = nValueSize;
	pxList->nRelocCount = nRelocCount;
	pxList->nBufferSize = nRelocCount * sizeof(struct xNode_t);
	pxList->nBufferCount = nRelocCount;
	pxList->pxBuffer = (struct xNode_t*)calloc(nRelocCount, sizeof(struct xNode_t));

	for (uint32_t i = 0; i < nRelocCount; ++i) {
		pxList->pxBuffer[i].pBuffer = malloc(nValueSize);
	}

	return pxList;
}

void List_Free(struct xList_t* pxList) {
	//if (pxList->pxHead) {
	//	pxList->pxCurr = pxList->pxHead;
	//	struct xNode_t* next;
	//	while (pxList->pxCurr != pxList->pxTail) {
	//		next = pxList->pxCurr->pxNext;
	//		free(pxList->pxCurr->pData);
	//		free(pxList->pxCurr);
	//		pxList->pxCurr = next;
	//	}
	//	free(pxList->pxTail->pData);
	//	free(pxList->pxTail);
	//}
	//free(pxList);
}

void List_Push(struct xList_t* pxList, void* pData) {
	struct xNode_t* pxNode = &pxList->pxBuffer[pxList->nBufferIndex];
	memcpy(pxNode->pBuffer, pData, pxList->nValueSize);

	if (pxList->pxHead == 0) {
		pxList->pxHead = pxNode;
		pxList->pxTail = pxList->pxHead;
	} else {
		pxList->pxTail->pxNext = pxNode;
		pxList->pxTail = pxList->pxTail->pxNext;
	}

	pxList->nBufferIndex += 1;
	pxList->nCount += 1;

	if (pxList->nBufferIndex >= pxList->nBufferCount) {
		printf("Reloc push\n");
		List_Reloc(pxList);
	}
}

void List_Pop(struct xList_t* pxList) {
#warning "Not yet implemented!"
}

uint32_t List_Count(struct xList_t* pxList) {
	return pxList->nCount;
}

void* List_Begin(struct xList_t* pxList) {
	pxList->pxCurr = pxList->pxHead;
	return (pxList->pxCurr) ? pxList->pxCurr->pBuffer : 0;
}

void* List_Next(struct xList_t* pxList) {
	pxList->pxCurr = pxList->pxCurr->pxNext;
	return (pxList->pxCurr) ? pxList->pxCurr->pBuffer : 0;
}
