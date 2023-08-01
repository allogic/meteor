#ifndef LIST_H
#define LIST_H

struct xList_t;

struct xList_t* List_Alloc(void);
void List_Push(struct xList_t* pxList, void* pData, uint32_t nSize);
void List_Dump(struct xList_t* pxList);

#endif
