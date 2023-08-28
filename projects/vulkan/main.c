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

	NativeWindow_Alloc(WINDOW_NAME, 800, 600);

	struct xVkInstance_t* pxVkInstance = VkInstance_Alloc();
	struct xVkSwapChain_t* pxVkSwapChain = VkSwapChain_Alloc(pxVkInstance);
	struct xVkRenderer_t* pxVkRenderer = VkRenderer_Alloc(pxVkInstance, pxVkSwapChain);

	while (NativeWindow_ShouldNotClose()) {
		NativeWindow_PollEvents();

		if (NativeWindow_HasResized()) {
			VkRenderer_Free(pxVkRenderer, pxVkInstance, pxVkSwapChain);
			VkSwapChain_Free(pxVkSwapChain, pxVkInstance);

			pxVkSwapChain = VkSwapChain_Alloc(pxVkInstance);
			pxVkRenderer = VkRenderer_Alloc(pxVkInstance, pxVkSwapChain);
		}

		VkRenderer_Draw(pxVkRenderer, pxVkInstance, pxVkSwapChain);
	}

	VkRenderer_Free(pxVkRenderer, pxVkInstance, pxVkSwapChain);
	VkSwapChain_Free(pxVkSwapChain, pxVkInstance);
	VkInstance_Free(pxVkInstance);

	NativeWindow_Free();

#ifdef DEBUG
	StackTrace_Free();
#endif

	return 0;
}
