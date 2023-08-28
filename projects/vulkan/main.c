#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <debug/stacktrace.h>

#include <platform/nativewindow.h>

#include <vulkan/vkinstance.h>
#include <vulkan/vkswapchain.h>
#include <vulkan/vkrenderer.h>
#include <vulkan/vkvertex.h>
#include <vulkan/vkbuffer.h>
#include <vulkan/vkbuffervariants.h>

#include <vulkan/vulkan.h>

struct xVkVertex_t axVertices[4] = {
	{  { -0.5F, -0.5F, 0.0F }, { 0.0F, 0.0F }, { 1.0F, 0.0F, 0.0F, 1.0F } },
	{  {  0.5F, -0.5F, 0.0F }, { 0.0F, 0.0F }, { 0.0F, 1.0F, 0.0F, 1.0F } },
	{  {  0.5F,  0.5F, 0.0F }, { 0.0F, 0.0F }, { 0.0F, 0.0F, 1.0F, 1.0F } },
	{  { -0.5F,  0.5F, 0.0F }, { 0.0F, 0.0F }, { 1.0F, 1.0F, 1.0F, 1.0F } },
};

uint32_t anIndices[6] = {
	0, 1, 2, 2, 3, 0,
};

int32_t main(void) {

#ifdef DEBUG
	StackTrace_Alloc();
#endif

	NativeWindow_Alloc(WINDOW_NAME, 800, 600);

	struct xVkInstance_t* pxVkInstance = VkInstance_Alloc();
	struct xVkSwapChain_t* pxVkSwapChain = VkSwapChain_Alloc(pxVkInstance);
	struct xVkRenderer_t* pxVkRenderer = VkRenderer_Alloc(pxVkInstance, pxVkSwapChain);
	struct xVkBuffer_t* pxVkVertexBuffer = VkVertexBuffer_Alloc(pxVkInstance, axVertices, sizeof(struct xVkVertex_t) * 4);
	struct xVkBuffer_t* pxVkIndexBuffer = VkIndexBuffer_Alloc(pxVkInstance, anIndices, sizeof(uint32_t) * 6);

	while (NativeWindow_ShouldNotClose()) {
		NativeWindow_PollEvents();

		if (NativeWindow_HasResized()) {
			VkRenderer_Free(pxVkRenderer, pxVkInstance, pxVkSwapChain);
			VkSwapChain_Free(pxVkSwapChain, pxVkInstance);

			pxVkSwapChain = VkSwapChain_Alloc(pxVkInstance);
			pxVkRenderer = VkRenderer_Alloc(pxVkInstance, pxVkSwapChain);
		}

		VkRenderer_Draw(pxVkRenderer, pxVkInstance, pxVkSwapChain, pxVkVertexBuffer, pxVkIndexBuffer, 6);
	}

	VkBuffer_Free(pxVkIndexBuffer, pxVkInstance);
	VkBuffer_Free(pxVkVertexBuffer, pxVkInstance);
	VkRenderer_Free(pxVkRenderer, pxVkInstance, pxVkSwapChain);
	VkSwapChain_Free(pxVkSwapChain, pxVkInstance);
	VkInstance_Free(pxVkInstance);

	NativeWindow_Free();

#ifdef DEBUG
	StackTrace_Free();
#endif

	return 0;
}
