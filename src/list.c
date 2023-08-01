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
};

struct xList_t* List_Alloc(void) {
	struct xList_t* pxList = malloc(sizeof(struct xList_t));
	pxList->pxHead = 0;
	pxList->pxCurr = 0;
	pxList->pxTail = 0;
	return pxList;
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
}

void List_Dump(struct xList_t* pxList) {
	if (pxList->pxHead == pxList->pxTail) {

	} else {
		pxList->pxCurr = pxList->pxHead->pxNext;
		while (pxList->pxCurr != pxList->pxHead) {
			printf("%p %p\n", (void*)pxList->pxCurr, pxList->pxCurr->pData);
			pxList->pxCurr = pxList->pxCurr->pxNext;
		}
	}
}
