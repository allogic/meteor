#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <stacktrace.h>

#include <platform/nativewindow.h>

#include <vulkan/vkinstance.h>
#include <vulkan/vkswapchain.h>
#include <vulkan/vkrenderer.h>

int32_t main(void) {

#ifdef DEBUG
	StackTrace_Alloc();
#endif

	struct xNativeWindow_t* pxNativeWindow = NativeWindow_Alloc(WINDOW_NAME, 800, 600);

	struct xVkInstance_t* pxVkInstance = VkInstance_Alloc(pxNativeWindow);
	struct xVkSwapChain_t* pxVkSwapChain = VkSwapChain_Alloc(pxNativeWindow, pxVkInstance);
	struct xVkRenderer_t* pxVkRenderer = VkRenderer_Alloc(pxVkInstance, pxVkSwapChain);

	while (NativeWindow_ShouldNotClose(pxNativeWindow)) {
		NativeWindow_PollEvents(pxNativeWindow);

		VkRenderer_Draw(pxVkRenderer, pxVkInstance, pxVkSwapChain);
	}

	VkRenderer_Free(pxVkRenderer, pxVkInstance, pxVkSwapChain);
	VkSwapChain_Free(pxVkSwapChain, pxVkInstance);
	VkInstance_Free(pxVkInstance);

	NativeWindow_Free(pxNativeWindow);

#ifdef DEBUG
	StackTrace_Free();
#endif

	return 0;
}
