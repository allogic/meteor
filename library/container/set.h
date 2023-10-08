#ifndef METEOR_CONTAINER_SET_H
#define METEOR_CONTAINER_SET_H

#include <stdint.h>
#include <stdbool.h>

struct xSet_t;

struct xSet_t* Set_Alloc(uint32_t nValueSize);
void Set_Free(struct xSet_t* pxSet);

uint32_t Set_Push(struct xSet_t* pxSet, void* pData);

void Set_Clear(struct xSet_t* pxSet);

void* Set_At(struct xSet_t* pxSet, uint32_t nIndex);
void* Set_Data(struct xSet_t* pxSet);

void* Set_Find(struct xSet_t* pxSet, void* pData, uint32_t* pnIndex);

bool Set_Empty(struct xSet_t* pxSet);
uint32_t Set_Count(struct xSet_t* pxSet);
uint32_t Set_Size(struct xSet_t* pxSet);

#endif
