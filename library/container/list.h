#ifndef METEOR_CONTAINER_LIST_H
#define METEOR_CONTAINER_LIST_H

#include <stdint.h>
#include <stdbool.h>

struct xList_t;

struct xList_t* List_Alloc(uint32_t nValueSize);
void List_Free(struct xList_t* pxList);

void* List_Push(struct xList_t* pxList, void* pData);
void List_Pop(struct xList_t* pxList, void* pData);

void* List_Remove(struct xList_t* pxList, void* pIter);

bool List_Empty(struct xList_t* pxList);
uint32_t List_Count(struct xList_t* pxList);

void* List_Begin(struct xList_t* pxList);
void* List_Value(void* pIter);
void* List_Next(void* pIter);

#endif
