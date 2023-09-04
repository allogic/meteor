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
#include <vulkan/uniform.h>
#include <vulkan/vertex.h>
#include <vulkan/buffer.h>
#include <vulkan/buffervariance.h>
#include <vulkan/image.h>
#include <vulkan/imagevariance.h>

#include <sandsim/renderer.h>

xVertex_t axVertices[4] = {
	{ { -0.5F, -0.5F, 0.0F }, { 1.0F, 0.0F }, { 1.0F, 0.0F, 0.0F, 1.0F } },
	{ {  0.5F, -0.5F, 0.0F }, { 0.0F, 0.0F }, { 0.0F, 1.0F, 0.0F, 1.0F } },
	{ {  0.5F,  0.5F, 0.0F }, { 0.0F, 1.0F }, { 0.0F, 0.0F, 1.0F, 1.0F } },
	{ { -0.5F,  0.5F, 0.0F }, { 1.0F, 1.0F }, { 1.0F, 1.0F, 1.0F, 1.0F } },
};

uint32_t anIndices[6] = {
	0, 1, 2, 2, 3, 0,
};

static xTimeInfo_t s_xTimeInfo = { 0.0F, 0.0F };
static xModelViewProjection_t s_xModelViewProjection = { MAT4_IDENTITY, MAT4_IDENTITY, MAT4_IDENTITY };

int32_t main(void) {

#ifdef DEBUG
	StackTrace_Alloc();
#endif

	NativeWindow_Alloc(WINDOW_NAME, 800, 600);

	struct xTimer_t* pxTimer = Timer_Alloc();

	struct xInstance_t* pxInstance = Instance_Alloc();
	struct xSwapChain_t* pxSwapChain = SwapChain_Alloc(pxInstance);
	struct xBuffer_t* pxVertexBuffer = VertexBuffer_Alloc(pxInstance, axVertices, sizeof(xVertex_t) * 4);
	struct xBuffer_t* pxIndexBuffer = IndexBuffer_Alloc(pxInstance, anIndices, sizeof(uint32_t) * 6);
	struct xImage_t* pxStandardImage = StandardImage_Alloc(pxInstance, "test.bmp");
	struct xRenderer_t* pxRenderer = Renderer_Alloc(pxInstance, pxSwapChain, pxStandardImage);

	Timer_Start(pxTimer);

	while (NativeWindow_ShouldNotClose()) {
		NativeWindow_PollEvents();

		Timer_Measure(pxTimer);

		if (NativeWindow_HasResized()) {
			Instance_WaitIdle(pxInstance);

			Renderer_Free(pxRenderer, pxInstance);
			SwapChain_Free(pxSwapChain, pxInstance);

			pxSwapChain = SwapChain_Alloc(pxInstance);
			pxRenderer = Renderer_Alloc(pxInstance, pxSwapChain, pxStandardImage);
		}

		Orthographic_Projection(-2.0F, 2.0F, -2.0F, 2.0F, 0.001F, 100.0F, s_xModelViewProjection.xProjection);

		xVec3_t xEye = { 0.0F, 0.0F, -1.0F };
		xVec3_t xCenter = { 0.0F, 0.0F, 0.0F };
		xVec3_t xUp = { 0.0F, 1.0F, 0.0F };
		View_LookAt(xEye, xCenter, xUp, s_xModelViewProjection.xView);

		xVec3_t xPosition = { sinf(Timer_GetTime(pxTimer)), cosf(Timer_GetTime(pxTimer)), 0.0F };
		Matrix_SetPosition(s_xModelViewProjection.xModel, xPosition);

		Renderer_UpdateModelViewProjection(pxRenderer, &s_xModelViewProjection); // TODO
		Renderer_Draw(pxRenderer, pxInstance, pxSwapChain, pxVertexBuffer, pxIndexBuffer, 6, pxStandardImage, &s_xTimeInfo);
	}

	Instance_WaitIdle(pxInstance);

	Renderer_Free(pxRenderer, pxInstance);
	Image_Free(pxStandardImage, pxInstance);
	Buffer_Free(pxIndexBuffer, pxInstance);
	Buffer_Free(pxVertexBuffer, pxInstance);
	SwapChain_Free(pxSwapChain, pxInstance);
	Instance_Free(pxInstance);

	Timer_Free(pxTimer);

	NativeWindow_Free();

#ifdef DEBUG
	StackTrace_Free();
#endif

	return 0;
}
