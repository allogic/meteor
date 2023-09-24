#ifndef METEOR_CONTAINER_QUEUE_H
#define METEOR_CONTAINER_QUEUE_H

#include <stdint.h>
#include <stdbool.h>

struct xQueue_t;

struct xQueue_t* Queue_Alloc(uint32_t nValueSize);
void Queue_Free(struct xQueue_t* pxQueue);

void Queue_Push(struct xQueue_t* pxQueue, void* pData);
void Queue_Pop(struct xQueue_t* pxQueue, void* pData);

bool Queue_Empty(struct xQueue_t* pxQueue);
uint32_t Queue_Count(struct xQueue_t* pxQueue);

#endif
