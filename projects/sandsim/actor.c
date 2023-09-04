#include <actor.h>

#include <standard/list.h>

struct xActor_t {
    const char* pcName;
    struct xActor_t* pxParent;
    struct xList_t* pxChildren;
    uint64_t wComponentMask;
    void* pComponents[64];
};