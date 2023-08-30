#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <common/timer.h>

#include <debug/stacktrace.h>

#include <math/vector.h>
#include <math/matrix.h>
#include <math/orthographic.h>
#include <math/view.h>

#include <platform/nativewindow.h>

#include <vulkan/vkinstance.h>
#include <vulkan/vkswapchain.h>
#include <vulkan/vkrenderer.h>
#include <vulkan/vkuniform.h>
#include <vulkan/vkvertex.h>
#include <vulkan/vkbuffer.h>
#include <vulkan/vkbuffervariants.h>

xVertex_t axVertices[4] = {
	{ { -0.5F, -0.5F, 0.0F }, { 0.0F, 0.0F }, { 1.0F, 0.0F, 0.0F, 1.0F } },
	{ {  0.5F, -0.5F, 0.0F }, { 0.0F, 0.0F }, { 0.0F, 1.0F, 0.0F, 1.0F } },
	{ {  0.5F,  0.5F, 0.0F }, { 0.0F, 0.0F }, { 0.0F, 0.0F, 1.0F, 1.0F } },
	{ { -0.5F,  0.5F, 0.0F }, { 0.0F, 0.0F }, { 1.0F, 1.0F, 1.0F, 1.0F } },
};

uint32_t anIndices[6] = {
	0, 1, 2, 2, 3, 0,
};

static xModelViewProjection_t s_xMvp = { MAT4_IDENTITY, MAT4_IDENTITY, MAT4_IDENTITY };

int32_t main(void) {

#ifdef DEBUG
	StackTrace_Alloc();
#endif

	NativeWindow_Alloc(WINDOW_NAME, 800, 600);

	struct xTimer_t* pxTimer = Timer_Alloc();

	struct xVkInstance_t* pxVkInstance = VkInstance_Alloc();
	struct xVkSwapChain_t* pxVkSwapChain = VkSwapChain_Alloc(pxVkInstance);
	struct xVkBuffer_t* pxVkVertexBuffer = VkVertexBuffer_Alloc(pxVkInstance, axVertices, sizeof(xVertex_t) * 4);
	struct xVkBuffer_t* pxVkIndexBuffer = VkIndexBuffer_Alloc(pxVkInstance, anIndices, sizeof(uint32_t) * 6);
	struct xVkRenderer_t* pxVkRenderer = VkRenderer_Alloc(pxVkInstance, pxVkSwapChain);

	Timer_Start(pxTimer);

	while (NativeWindow_ShouldNotClose()) {
		NativeWindow_PollEvents();

		Timer_Measure(pxTimer);

		if (NativeWindow_HasResized()) {
			VkInstance_WaitIdle(pxVkInstance);

			VkRenderer_Free(pxVkRenderer, pxVkInstance);
			VkSwapChain_Free(pxVkSwapChain, pxVkInstance);

			pxVkSwapChain = VkSwapChain_Alloc(pxVkInstance);
			pxVkRenderer = VkRenderer_Alloc(pxVkInstance, pxVkSwapChain);
		}

		Orthographic_Projection(-2.0F, 2.0F, -2.0F, 2.0F, 0.001F, 100.0F, s_xMvp.xProjection);

		xVec3_t xEye = { 0.0F, 0.0F, -1.0F };
		xVec3_t xCenter = { 0.0F, 0.0F, 0.0F };
		xVec3_t xUp = { 0.0F, 1.0F, 0.0F };
		View_LookAt(xEye, xCenter, xUp, s_xMvp.xView);

		xVec3_t xPosition = { sinf(Timer_GetTime(pxTimer)), cosf(Timer_GetTime(pxTimer)), 0.0F };
		Matrix_SetPosition(s_xMvp.xModel, xPosition);

		VkRenderer_UpdateModelViewProjection(pxVkRenderer, &s_xMvp);
		VkRenderer_Draw(pxVkRenderer, pxVkInstance, pxVkSwapChain, pxVkVertexBuffer, pxVkIndexBuffer, 6);
	}

	VkInstance_WaitIdle(pxVkInstance);

	VkRenderer_Free(pxVkRenderer, pxVkInstance);
	VkBuffer_Free(pxVkIndexBuffer, pxVkInstance);
	VkBuffer_Free(pxVkVertexBuffer, pxVkInstance);
	VkSwapChain_Free(pxVkSwapChain, pxVkInstance);
	VkInstance_Free(pxVkInstance);

	Timer_Free(pxTimer);

	NativeWindow_Free();

#ifdef DEBUG
	StackTrace_Free();
#endif

	return 0;
}
