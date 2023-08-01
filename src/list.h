#ifndef LIST_H
#define LIST_H

struct xList_t;

struct xList_t* List_Alloc(void);
void List_Free(struct xList_t* pxList);
void List_Push(struct xList_t* pxList, void* pData, uint32_t nSize);
uint32_t List_Count(struct xList_t* pxList);
void List_Dump(struct xList_t* pxList, void(*pPrint)(void*));

void List_PrintInt(void* pData);
void List_PrintString(void* pData);

#endif
