#include <stdlib.h>

#include <scene.h>

struct xScene_t* Scene_Alloc(void) {
    struct xScene_t* pxScene = (struct xScene_t*)calloc(1, sizeof(struct xScene_t));

    

    return pxScene;
}

void Scene_Free(struct xScene_t* pxScene) {
    free(pxScene);
}
