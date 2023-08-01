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
	struct xList_t* pxList = malloc(sizeof(struct xList_t));
	pxList->pxHead = 0;
	pxList->pxCurr = 0;
	pxList->pxTail = 0;
	return pxList;
}

void List_Free(struct xList_t* pxList) {
	if (pxList->pxHead == 0 || pxList->pxTail == 0) {

	} else if (pxList->pxHead == pxList->pxTail) {
		free(pxList->pxHead->pData);
		free(pxList->pxHead);
	} else {
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
		pxList->pxHead = malloc(sizeof(struct xNode_t));
		pxList->pxHead->pData = malloc(nSize);
		memcpy(pxList->pxHead->pData, pData, nSize);
		pxList->pxTail = pxList->pxHead;
	} else {
		pxList->pxTail->pxNext = malloc(sizeof(struct xNode_t));
		pxList->pxTail->pxNext->pData = malloc(nSize);
		memcpy(pxList->pxTail->pxNext->pData, pData, nSize);
		pxList->pxTail = pxList->pxTail->pxNext;
	}
	pxList->nCount++;
}

uint32_t List_Count(struct xList_t* pxList) {
	return pxList->nCount;
}

void List_Dump(struct xList_t* pxList, void(*pPrint)(void* pData)) {
	if (pxList->pxHead == 0 || pxList->pxTail == 0) {

	} else if (pxList->pxHead == pxList->pxTail) {
		pPrint(pxList->pxHead->pData);
	} else {
		pxList->pxCurr = pxList->pxHead;
		while (pxList->pxCurr != pxList->pxTail) {
			pPrint(pxList->pxCurr->pData);
			pxList->pxCurr = pxList->pxCurr->pxNext;
		}
		pPrint(pxList->pxTail->pData);
	}
}

void List_PrintInt(void* pData) {
	printf("%d\n", *(int32_t*)pData);
}

void List_PrintString(void* pData) {
	printf("%s\n", (const char*)pData);
}
