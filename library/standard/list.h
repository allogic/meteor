#ifndef METEOR_LIST_H
#define METEOR_LIST_H

#include <stdint.h>

struct xList_t;

struct xList_t* List_Alloc(void);
void List_Free(struct xList_t* pxList);

void List_Push(struct xList_t* pxList, void* pData, uint32_t nSize);
void List_Pop(struct xList_t* pxList);

uint32_t List_Count(struct xList_t* pxList);

void* List_Begin(struct xList_t* pxList);
void* List_Next(struct xList_t* pxList);

#endif
