#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <stacktrace.h>

#include <platform/nativewindow.h>

#include <vulkan/vkinstance.h>
#include <vulkan/vkswapchain.h>
#include <vulkan/vkrenderer.h>
#include <vulkan/vkbuffer.h>
#include <vulkan/vkvertex.h>

#include <vulkan/vulkan.h>

struct xVkVertex_t axVertices[3] = {
	{  {  0.0F, -0.5F, 0.0F }, { 0.0F, 0.0F }, { 1.0F, 0.0F, 0.0F, 1.0F } },
	{  {  0.5F,  0.5F, 0.0F }, { 0.0F, 0.0F }, { 0.0F, 1.0F, 0.0F, 1.0F } },
	{  { -0.5F,  0.5F, 0.0F }, { 0.0F, 0.0F }, { 0.0F, 0.0F, 1.0F, 1.0F } },
};

int32_t main(void) {

#ifdef DEBUG
	StackTrace_Alloc();
#endif

	NativeWindow_Alloc(WINDOW_NAME, 800, 600);

	struct xVkInstance_t* pxVkInstance = VkInstance_Alloc();
	struct xVkSwapChain_t* pxVkSwapChain = VkSwapChain_Alloc(pxVkInstance);
	struct xVkRenderer_t* pxVkRenderer = VkRenderer_Alloc(pxVkInstance, pxVkSwapChain);
	struct xVkBuffer_t* pxVkVertexBuffer = VkVertexBuffer_Alloc(pxVkInstance, axVertices, 3);

	while (NativeWindow_ShouldNotClose()) {
		NativeWindow_PollEvents();

		if (NativeWindow_HasResized()) {
			VkRenderer_Free(pxVkRenderer, pxVkInstance, pxVkSwapChain);
			VkSwapChain_Free(pxVkSwapChain, pxVkInstance);

			pxVkSwapChain = VkSwapChain_Alloc(pxVkInstance);
			pxVkRenderer = VkRenderer_Alloc(pxVkInstance, pxVkSwapChain);
		}

		VkRenderer_Draw(pxVkRenderer, pxVkInstance, pxVkSwapChain, pxVkVertexBuffer);
	}

	VkVertexBuffer_Free(pxVkVertexBuffer, pxVkInstance);
	VkRenderer_Free(pxVkRenderer, pxVkInstance, pxVkSwapChain);
	VkSwapChain_Free(pxVkSwapChain, pxVkInstance);
	VkInstance_Free(pxVkInstance);

	NativeWindow_Free();

#ifdef DEBUG
	StackTrace_Free();
#endif

	return 0;
}
