#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <container/list.h>

struct xNode_t {
	struct xNode_t* pxPrev;
	struct xNode_t* pxNext;
	void* pBuffer;
};

struct xList_t {
	struct xNode_t* pxHead;
	struct xNode_t* pxCurr;
	struct xNode_t* pxTail;
	uint32_t nValueSize;
	uint32_t nNodeCount;
};

struct xList_t* List_Alloc(uint32_t nValueSize) {
	struct xList_t* pxList = (struct xList_t*)calloc(1, sizeof(struct xList_t));

	pxList->nValueSize = nValueSize;

	return pxList;
}

void List_Free(struct xList_t* pxList) {
	pxList->pxCurr = pxList->pxHead;

	struct xNode_t* pxNext;
	while (pxList->pxCurr) {
		pxNext = pxList->pxCurr->pxNext;

		free(pxList->pxCurr->pBuffer);
		free(pxList->pxCurr);

		pxList->pxCurr = pxNext;
	}

	free(pxList);
}

void* List_Add(struct xList_t* pxList, void* pData) {
	struct xNode_t* pxNode = (struct xNode_t*)calloc(1, sizeof(struct xNode_t));

	pxNode->pBuffer = malloc(pxList->nValueSize);

	if (pData) {
		memcpy(pxNode->pBuffer, pData, pxList->nValueSize);
	}

	if (pxList->pxHead) {
		pxNode->pxPrev = pxList->pxTail;

		pxList->pxTail->pxNext = pxNode;
		pxList->pxTail = pxNode;
	} else {
		pxList->pxHead = pxNode;
		pxList->pxTail = pxNode;
	}

	pxList->nNodeCount += 1;

	return pxNode;
}

void* List_Remove(struct xList_t* pxList, void* pIter) {
	if (pxList->pxHead == pIter) {
		struct xNode_t* pxNext = pxList->pxHead->pxNext;

		free(pxList->pxHead->pBuffer);
		free(pxList->pxHead);

		pxList->pxHead = pxNext;

		pxList->nNodeCount -= 1;

		return pxList->pxHead;
	} else if (pxList->pxTail == pIter) {
		struct xNode_t* pxPrev = pxList->pxTail->pxPrev;

		pxPrev->pxNext = 0;

		free(pxList->pxTail->pBuffer);
		free(pxList->pxTail);

		pxList->pxTail = pxPrev;

		pxList->nNodeCount -= 1;

		return pxList->pxTail;
	} else {
		pxList->pxCurr = pxList->pxHead->pxNext;

		struct xNode_t* pxNext;
		while (pxList->pxCurr) {
			pxNext = pxList->pxCurr->pxNext;

			if (pxList->pxCurr == pIter) {
				pxList->pxCurr->pxPrev->pxNext = pxNext;

				free(pxList->pxCurr->pBuffer);
				free(pxList->pxCurr);

				pxList->nNodeCount -= 1;

				return pxNext;
			}

			pxList->pxCurr = pxNext;
		}
	}

	return pxList->pxTail;
}

bool List_Empty(struct xList_t* pxList) {
	return pxList->nNodeCount == 0;
}

uint32_t List_Count(struct xList_t* pxList) {
	return pxList->nNodeCount;
}

void* List_Begin(struct xList_t* pxList) {
	return pxList->pxHead;
}

void* List_Value(void* pIter) {
	return ((struct xNode_t*)pIter)->pBuffer;
}

void* List_Next(void* pIter) {
	return ((struct xNode_t*)pIter)->pxNext;
}
