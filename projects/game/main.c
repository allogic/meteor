#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <common/macros.h>
#include <common/timer.h>

#include <debug/stacktrace.h>

#include <math/vector.h>

#include <platform/nativewindow.h>

#include <vulkan/instance.h>
#include <vulkan/vertex.h>
#include <vulkan/buffer.h>
#include <vulkan/buffervariance.h>

#include <game/assets.h>
#include <game/scene.h>
#include <game/entity.h>
#include <game/component.h>
#include <game/storage.h>

ASSET_IMAGE_ARRAY_BEGIN
//	ASSET_IMAGE_ARRAY_ENTRY("test", "assets/test.bmp")
//	ASSET_IMAGE_ARRAY_ENTRY("chunk", "assets/chunk.bmp")
ASSET_IMAGE_ARRAY_END

int32_t main(void) {

#ifdef DEBUG
	StackTrace_Alloc();
#endif

	NativeWindow_Alloc(WINDOW_NAME, 1200, 1200);

	struct xTimer_t* pxTimer = Timer_Alloc();

	struct xInstance_t* pxInstance = Instance_Alloc();

	Assets_Alloc(pxInstance);

	struct xScene_t* pxScene = Scene_Alloc(pxInstance);

	Timer_Start(pxTimer);

	while (!NativeWindow_ShouldClose()) {
		NativeWindow_PollEvents();

		Timer_Measure(pxTimer);

		if (NativeWindow_HasResized()) {
			Instance_WaitIdle(pxInstance);

			Scene_Resize(pxScene, pxInstance);
		}

		Scene_Draw(pxScene, pxInstance, pxTimer);
	}

	Instance_WaitIdle(pxInstance);

	Scene_Free(pxScene, pxInstance);

	Assets_Free(pxInstance);

	Instance_Free(pxInstance);

	Timer_Free(pxTimer);

	NativeWindow_Free();

#ifdef DEBUG
	StackTrace_Free();
#endif

	return 0;
}
