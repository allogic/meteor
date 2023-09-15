#ifndef METEOR_CONTAINER_VECTOR_H
#define METEOR_CONTAINER_VECTOR_H

#include <stdint.h>
#include <stdbool.h>

// TODO
#define VECTOR_DUMP(VECTOR, TYPE, NAME) \
    printf("%s\n", NAME); \
    printf("\tCount %u\n", Vector_Count(VECTOR)); \
    for (uint32_t i = 0; i < Vector_Count(VECTOR); ++i) { \
        printf("\t\t%p\n", *(TYPE*)Vector_At(VECTOR, i)); \
    }

struct xVector_t;

struct xVector_t* Vector_Alloc(uint32_t nValueSize, uint32_t nCapacity);
void Vector_Free(struct xVector_t* pxVector);

uint32_t Vector_Push(struct xVector_t* pxVector, void* pData);

void Vector_Resize(struct xVector_t* pxVector, uint32_t nCount);

void* Vector_At(struct xVector_t* pxVector, uint32_t nIndex);
void* Vector_Data(struct xVector_t* pxVector);

bool Vector_Empty(struct xVector_t* pxVector);
uint32_t Vector_Count(struct xVector_t* pxVector);

#endif
