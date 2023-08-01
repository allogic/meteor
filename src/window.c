#include <stdio.h>
#include <string.h>

#include "core.h"
#include "window.h"
#include "glad.h"

#ifdef OS_WINDOWS
#	include <windows.h>
#endif

#ifdef OS_LINUX
#	include <wayland-client.h>
#	include <wayland-egl.h>
#	include <EGL/egl.h>
#	include <EGL/eglext.h>
#endif

static bool s_bWindowShouldClose;

#ifdef OS_WINDOWS
typedef void*(*PFNWGLCREATECONTEXTPROC)(void*);
typedef int32_t(*PFNWGLMAKECURRENTPROC)(void*, void*);
typedef int32_t(*PFNWGLDELETECONTEXTPROC)(void*);

static PFNWGLCREATECONTEXTPROC s_pWglCreateContext;
static PFNWGLMAKECURRENTPROC s_pWglMakeCurrent;
static PFNWGLDELETECONTEXTPROC s_pWglDeleteContext;

static void* s_pxOpenGl32;
static void* s_pxWindow;
static void* s_pxDeviceContext;
static void* s_pWglContext;

static int64_t WndProc(HWND pxWindow, uint32_t nMessage, uint64_t mWParam, int64_t mLParam) {
	switch (nMessage) {
		case WM_CLOSE: {
			s_bWindowShouldClose = true;
		}
		break;
		case WM_MOUSEMOVE: {
            uint32_t nMouseX = LOWORD(mLParam);
            uint32_t nMouseY = HIWORD(mLParam);
			UNUSED(nMouseX);
			UNUSED(nMouseY);
		}
		break;
		default: {
			return DefWindowProc(pxWindow, nMessage, mWParam, mLParam);
		}
	}
	return 0;
}
#endif

#ifdef OS_LINUX
static struct wl_display* s_pxDisplay;
static struct wl_registry* s_pxRegistry;
static struct wl_compositor* s_pxCompositor;
static struct wl_region* s_pxRegion;
static struct wl_surface* s_pxSurface;
static struct wl_egl_window* s_pxEglWindow;

static EGLDisplay s_pxEglDisplay;
static EGLContext s_pxEglContext;
static EGLSurface s_pxEglSurface;

static void RegistryGlobal(void* pData, struct wl_registry* pxRegistry, uint32_t nId, const char* pcInterface, uint32_t nVersion) {
	UNUSED(pData);
	UNUSED(nId);
	UNUSED(nVersion);

	if (strcmp(pcInterface, "wl_compositor") == 0) {
		s_pxCompositor = wl_registry_bind(pxRegistry, nId, &wl_compositor_interface, 1);
	}
}

static void RegistryGlobalRemove(void* pData, struct wl_registry* pxRegistry, uint32_t nId) {
	UNUSED(pData);
	UNUSED(pxRegistry);
	UNUSED(nId);
}

static const struct wl_registry_listener s_xRgistryListener = {
	.global = RegistryGlobal,
	.global_remove = RegistryGlobalRemove,
};
#endif

int32_t Window_Alloc(const char* pcWindowTitle, uint32_t nWidth, uint32_t nHeight) {
#ifdef OS_WINDOWS
	const char acClassName[] = "OpenGLWin32Class";

	WNDCLASSEX xWindowClassEx;
	PIXELFORMATDESCRIPTOR xPixelFormatDesc;

	memset(&xWindowClassEx, 0, sizeof(xWindowClassEx));
	xWindowClassEx.cbSize = sizeof(xWindowClassEx);
	xWindowClassEx.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	xWindowClassEx.lpfnWndProc = WndProc;
	xWindowClassEx.cbClsExtra = 0;
	xWindowClassEx.cbWndExtra = 0;
	xWindowClassEx.hInstance = GetModuleHandle(0);
	xWindowClassEx.hIcon = LoadIcon(0, IDI_APPLICATION);
	xWindowClassEx.hCursor = LoadCursor(0, IDC_ARROW);
	xWindowClassEx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	xWindowClassEx.lpszMenuName = 0;
	xWindowClassEx.lpszClassName = acClassName;
	xWindowClassEx.hIconSm = LoadIcon(0, IDI_APPLICATION);

	RegisterClassEx(&xWindowClassEx);

	s_pxWindow = CreateWindowEx(0, acClassName, pcWindowTitle, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, nWidth, nHeight, 0, 0, GetModuleHandle(0), 0);
	if (s_pxWindow == 0) {
		printf("Failed creating window\n");
		return -1;
	}

	s_pxDeviceContext = GetDC(s_pxWindow);
	if (s_pxDeviceContext == 0) {
		printf("Failed creating device context\n");
		return -1;
	}

	memset(&xPixelFormatDesc, 0, sizeof(xPixelFormatDesc));
	xPixelFormatDesc.nSize = sizeof(xPixelFormatDesc);
	xPixelFormatDesc.nVersion = 1;
	xPixelFormatDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	xPixelFormatDesc.iPixelType = PFD_TYPE_RGBA;
	xPixelFormatDesc.cColorBits = 32;
	xPixelFormatDesc.cDepthBits = 24;
	xPixelFormatDesc.iLayerType = PFD_MAIN_PLANE;

	int32_t nPixelFormat = ChoosePixelFormat(s_pxDeviceContext, &xPixelFormatDesc);
	SetPixelFormat(s_pxDeviceContext, nPixelFormat, &xPixelFormatDesc);

	s_pxOpenGl32 = LoadLibrary("opengl32.dll");
	s_pWglCreateContext = (PFNWGLCREATECONTEXTPROC)GetProcAddress(s_pxOpenGl32, "wglCreateContext");
	s_pWglMakeCurrent = (PFNWGLMAKECURRENTPROC)GetProcAddress(s_pxOpenGl32, "wglMakeCurrent");
	s_pWglDeleteContext = (PFNWGLDELETECONTEXTPROC)GetProcAddress(s_pxOpenGl32, "wglDeleteContext");

	s_pWglContext = s_pWglCreateContext(s_pxDeviceContext);
	if (s_pWglContext == 0) {
		printf("Failed creating WGL context\n");
		return -1;
	}

	if (s_pWglMakeCurrent(s_pxDeviceContext, s_pWglContext) == 0) {
		printf("Failed making context current\n");
		return -1;
	}

	gladLoadGL();

	ShowWindow(s_pxWindow, SW_SHOW);
	UpdateWindow(s_pxWindow);
#endif

#ifdef OS_LINUX
	s_pxDisplay = wl_display_connect(0);
	if (s_pxDisplay == 0) {
		printf("Failed creating wayland display\n");
		return -1;
	}

	s_pxRegistry = wl_display_get_registry(s_pxDisplay);
	if (s_pxRegistry == 0) {
		printf("Failed creating wayland registry\n");
		return -1;
	}

	wl_registry_add_listener(s_pxRegistry, &s_xRgistryListener, 0);

	wl_display_dispatch(s_pxDisplay);
	wl_display_roundtrip(s_pxDisplay);

	s_pxSurface = wl_compositor_create_surface(s_pxCompositor);
	if (s_pxSurface == 0) {
		printf("Failed creating wayland surface\n");
		return -1;
	}

	s_pxEglDisplay = eglGetDisplay(s_pxDisplay);
	if (s_pxEglDisplay == 0) {
		printf("Failed creating EGL display\n");
		return -1;
	}

	int32_t nMajor, nMinor;
	if (eglInitialize(s_pxEglDisplay, &nMajor, &nMinor) == 0) {
		printf("Failed initializing EGL\n");
		return -1;
	}

	printf("Found GL version %d.%d\n", nMajor, nMinor);

	EGLConfig xConfig;

	int32_t anConfigAttribs[] = {
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_NONE
	};

	int32_t nNumConfigs;

	if (eglChooseConfig(s_pxEglDisplay, anConfigAttribs, &xConfig, 1, &nNumConfigs) == 0) {
		printf("Failed choosing EGL config\n");
		return -1;
	}

	int32_t anContextAttribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	s_pxEglContext = eglCreateContext(s_pxEglDisplay, xConfig, EGL_NO_CONTEXT, anContextAttribs);
	if (s_pxEglContext == EGL_NO_CONTEXT) {
		printf("Failed creating EGL context\n");
		return -1;
	}

	s_pxRegion = wl_compositor_create_region(s_pxCompositor);

	wl_region_add(s_pxRegion, 0, 0, 800, 600);
	wl_surface_set_opaque_region(s_pxSurface, s_pxRegion);

	s_pxEglWindow = wl_egl_window_create(s_pxSurface, 800, 600);
	if (s_pxEglWindow == 0) {
		printf("Failed creating EGL window\n");
		return -1;
	}

	s_pxEglSurface = eglCreateWindowSurface(s_pxEglDisplay, xConfig, s_pxEglWindow, 0);
	if (s_pxEglSurface == EGL_NO_SURFACE) {
		printf("Failed creating EGL surface\n");
		return -1;
	}

	if (eglMakeCurrent(s_pxEglDisplay, s_pxEglSurface, s_pxEglSurface, s_pxEglContext) == 0) {
		printf("Failed making context current\n");
		return -1;
	}

	gladLoadGL();
#endif

	return 0;
}

bool Window_ShouldClose(void) {
	return s_bWindowShouldClose;
}

void Window_PollEvents(void) {
#ifdef OS_WINDOWS
	MSG xMsg;
	if (PeekMessage(&xMsg, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&xMsg);
		DispatchMessage(&xMsg);
	}
#endif

#ifdef OS_LINUX
	wl_display_dispatch_pending(s_pxDisplay);
	//while (wl_display_prepare_read(s_pxDisplay) != 0) {
	//	wl_display_dispatch_pending(s_pxDisplay);
	//}
	//wl_display_flush(s_pxDisplay);
	//wl_display_read_events(s_pxDisplay);
	//wl_display_dispatch_pending(s_pxDisplay);
#endif
}

void Window_SwapBuffers(void) {
#ifdef OS_WINDOWS
	SwapBuffers(s_pxDeviceContext);
#endif

#ifdef OS_LINUX
	eglSwapBuffers(s_pxEglDisplay, s_pxEglSurface);
#endif
}

void Window_Free(void) {
#ifdef OS_WINDOWS
	s_pWglMakeCurrent(0, 0);
	s_pWglDeleteContext(s_pWglContext);

	ReleaseDC(s_pxWindow, s_pxDeviceContext);

	DestroyWindow(s_pxWindow);

	FreeLibrary(s_pxOpenGl32);
#endif

#ifdef OS_LINUX
	eglMakeCurrent(s_pxEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroySurface(s_pxEglDisplay, s_pxEglSurface);

	wl_egl_window_destroy(s_pxEglWindow);

	eglDestroyContext(s_pxEglDisplay, s_pxEglContext);

	wl_surface_destroy(s_pxSurface);
	wl_compositor_destroy(s_pxCompositor);
	wl_registry_destroy(s_pxRegistry);
	wl_display_disconnect(s_pxDisplay);
#endif
}
