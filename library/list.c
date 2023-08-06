#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "list.h"

struct xNode_t {
	struct xNode_t* pxNext;
	void* pData;
};

struct xList_t {
	struct xNode_t* pxHead;
	struct xNode_t* pxCurr;
	struct xNode_t* pxTail;
	uint32_t nCount;
};

struct xList_t* List_Alloc(void) {
	return calloc(sizeof(struct xList_t), 1);
}

void List_Free(struct xList_t* pxList) {
	if (pxList->pxHead) {
		pxList->pxCurr = pxList->pxHead;
		struct xNode_t* next;
		while (pxList->pxCurr != pxList->pxTail) {
			next = pxList->pxCurr->pxNext;
			free(pxList->pxCurr->pData);
			free(pxList->pxCurr);
			pxList->pxCurr = next;
		}
		free(pxList->pxTail->pData);
		free(pxList->pxTail);
	}
	free(pxList);
}

void List_Push(struct xList_t* pxList, void* pData, uint32_t nSize) {
	if (pxList->pxHead == 0) {
		pxList->pxHead = calloc(sizeof(struct xNode_t), 1);
		pxList->pxHead->pData = malloc(nSize);
		memcpy(pxList->pxHead->pData, pData, nSize);
		pxList->pxTail = pxList->pxHead;
	} else {
		pxList->pxTail->pxNext = calloc(sizeof(struct xNode_t), 1);
		pxList->pxTail->pxNext->pData = malloc(nSize);
		memcpy(pxList->pxTail->pxNext->pData, pData, nSize);
		pxList->pxTail = pxList->pxTail->pxNext;
	}
	pxList->nCount++;
}

uint32_t List_Count(struct xList_t* pxList) {
	return pxList->nCount;
}

void* List_Begin(struct xList_t* pxList) {
	pxList->pxCurr = pxList->pxHead;
	return (pxList->pxCurr) ? pxList->pxCurr->pData : 0;
}

void* List_Next(struct xList_t* pxList) {
	pxList->pxCurr = pxList->pxCurr->pxNext;
	return (pxList->pxCurr) ? pxList->pxCurr->pData : 0;
}
