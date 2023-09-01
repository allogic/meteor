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

#include <vulkan/instance.h>
#include <vulkan/swapchain.h>
#include <vulkan/renderer.h>
#include <vulkan/uniform.h>
#include <vulkan/vertex.h>
#include <vulkan/buffer.h>
#include <vulkan/buffervariance.h>
#include <vulkan/image.h>
#include <vulkan/imagevariance.h>

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

	struct xInstance_t* pxInstance = VkInstance_Alloc();
	struct xSwapChain_t* pxSwapChain = VkSwapChain_Alloc(pxInstance);
	struct xBuffer_t* pxVertexBuffer = VkVertexBuffer_Alloc(pxInstance, axVertices, sizeof(xVertex_t) * 4);
	struct xBuffer_t* pxIndexBuffer = VkIndexBuffer_Alloc(pxInstance, anIndices, sizeof(uint32_t) * 6);
	struct xImage_t* pxTextureImage = VkTextureImage_Alloc(pxInstance, "../test.bmp");
	struct xRenderer_t* pxRenderer = VkRenderer_Alloc(pxInstance, pxSwapChain);

	Timer_Start(pxTimer);

	while (NativeWindow_ShouldNotClose()) {
		NativeWindow_PollEvents();

		Timer_Measure(pxTimer);

		if (NativeWindow_HasResized()) {
			VkInstance_WaitIdle(pxInstance);

			VkRenderer_Free(pxRenderer, pxInstance);
			VkSwapChain_Free(pxSwapChain, pxInstance);

			pxSwapChain = VkSwapChain_Alloc(pxInstance);
			pxRenderer = VkRenderer_Alloc(pxInstance, pxSwapChain);
		}

		Orthographic_Projection(-2.0F, 2.0F, -2.0F, 2.0F, 0.001F, 100.0F, s_xMvp.xProjection);

		xVec3_t xEye = { 0.0F, 0.0F, -1.0F };
		xVec3_t xCenter = { 0.0F, 0.0F, 0.0F };
		xVec3_t xUp = { 0.0F, 1.0F, 0.0F };
		View_LookAt(xEye, xCenter, xUp, s_xMvp.xView);

		xVec3_t xPosition = { sinf(Timer_GetTime(pxTimer)), cosf(Timer_GetTime(pxTimer)), 0.0F };
		Matrix_SetPosition(s_xMvp.xModel, xPosition);

		VkRenderer_UpdateModelViewProjection(pxRenderer, &s_xMvp);
		VkRenderer_Draw(pxRenderer, pxInstance, pxSwapChain, pxVertexBuffer, pxIndexBuffer, 6);
	}

	VkInstance_WaitIdle(pxInstance);

	VkRenderer_Free(pxRenderer, pxInstance);
	VkImage_Free(pxTextureImage, pxInstance);
	VkBuffer_Free(pxIndexBuffer, pxInstance);
	VkBuffer_Free(pxVertexBuffer, pxInstance);
	VkSwapChain_Free(pxSwapChain, pxInstance);
	VkInstance_Free(pxInstance);

	Timer_Free(pxTimer);

	NativeWindow_Free();

#ifdef DEBUG
	StackTrace_Free();
#endif

	return 0;
}
