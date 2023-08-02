#include <stdio.h>
#include <string.h>

#include "macros.h"
#include "window.h"
#include "glad.h"

#ifdef OS_WINDOWS
#	include <windows.h>
#endif

#ifdef OS_LINUX
#	include <wayland-client.h>
#	include <wayland-client-protocol.h>
#	include <wayland-egl.h>
#	include <EGL/egl.h>
#	include <EGL/eglext.h>
#	include "xdgshell.h"
#endif

struct xWindow_t {
	bool bShouldClose;
	uint32_t nWindowWidth;
	uint32_t nWindowHeight;
	uint32_t nMouseX;
	uint32_t nMouseY;
};

static struct xWindow_t s_xWindow;

#ifdef OS_WINDOWS
typedef HGLRC(*PFNWGLCREATECONTEXTPROC)(HDC);
typedef BOOL(*PFNWGLMAKECURRENTPROC)(HDC, HGLRC);
typedef BOOL(*PFNWGLDELETECONTEXTPROC)(HGLRC);

static PFNWGLCREATECONTEXTPROC s_pWglCreateContext;
static PFNWGLMAKECURRENTPROC s_pWglMakeCurrent;
static PFNWGLDELETECONTEXTPROC s_pWglDeleteContext;

static HMODULE s_hOpenGl32;
static HWND s_hWindow;
static HDC s_hDeviceContext;
static HGLRC s_hWglContext;

static LRESULT WndProc(HWND hWnd, UINT nMessage, WPARAM wParam, LPARAM lParam) {
	switch (nMessage) {
		case WM_CLOSE: {
			s_xWindow.bShouldClose = true;
		}
		break;
		case WM_MOUSEMOVE: {
			s_xWindow.nMouseX = LOWORD(lParam);
			s_xWindow.nMouseY = HIWORD(lParam);
		}
		break;
		default: {
			return DefWindowProc(hWnd, nMessage, wParam, lParam);
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
static struct xdg_wm_base* s_pxXdgWmBase;
static struct xdg_surface* s_pxXdgSurface;
static struct xdg_toplevel* s_pxXdgTopLevel;
static struct wl_egl_window* s_pxEglWindow;

static EGLDisplay s_pxEglDisplay;
static EGLContext s_pxEglContext;
static EGLSurface s_pxEglSurface;

static void RegistryGlobal(void* pData, struct wl_registry* pxRegistry, uint32_t nId, const char* pcInterface, uint32_t nVersion) {
	UNUSED(pData);
	UNUSED(nVersion);

	if (strcmp(pcInterface, "wl_compositor") == 0) {
		s_pxCompositor = wl_registry_bind(pxRegistry, nId, &wl_compositor_interface, 1);
	} else if (strcmp(pcInterface, "xdg_wm_base") == 0) {
		s_pxXdgWmBase = wl_registry_bind(pxRegistry, nId, &xdg_wm_base_interface, 1);
	}
}

static void RegistryGlobalRemove(void* pData, struct wl_registry* pxRegistry, uint32_t nId) {
	UNUSED(pData);
	UNUSED(pxRegistry);
	UNUSED(nId);
}

//void ShellSurfacePing(void* pData, struct wl_shell_surface* pxShellSurface, uint32_t nSerial) {
//	UNUSED(pData);
//
//	wl_shell_surface_pong(pxShellSurface, nSerial);
//}

//void ShellSurfaceConfigure(void* pData, struct wl_shell_surface* pxShellSurface, uint32_t nEdges, int32_t nWidth, int32_t nHeight) {
//	UNUSED(pxShellSurface);
//	UNUSED(nEdges);
//
//	struct window* pxWindow = pData;
//	wl_egl_window_resize(s_pxEglWindow, nWidth, nHeight, 0, 0);
//}

//void ShellSurfacePopupDone(void* pData, struct wl_shell_surface* pxShellSurface) {
//	UNUSED(pData);
//	UNUSED(pxShellSurface);
//}

static const struct wl_registry_listener s_xRgistryListener = {
	.global = RegistryGlobal,
	.global_remove = RegistryGlobalRemove,
};

//static struct wl_shell_surface_listener s_xShellSurfaceListener = {
//	.ping = ShellSurfacePing,
//	.configure = ShellSurfaceConfigure,
//	.popup_done = ShellSurfacePopupDone,
//};
#endif

struct xWindow_t* Window_Alloc(const char* pcWindowTitle, uint32_t nWidth, uint32_t nHeight) {
#ifdef OS_WINDOWS
	const char acClassName[] = "OpenGLWin32Class";

	HMODULE hInstance = GetModuleHandle(0);

	WNDCLASSEX xWindowClassEx;
	PIXELFORMATDESCRIPTOR xPixelFormatDesc;

	memset(&xWindowClassEx, 0, sizeof(xWindowClassEx));
	xWindowClassEx.cbSize = sizeof(xWindowClassEx);
	xWindowClassEx.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	xWindowClassEx.lpfnWndProc = WndProc;
	xWindowClassEx.cbClsExtra = 0;
	xWindowClassEx.cbWndExtra = 0;
	xWindowClassEx.hInstance = hInstance;
	xWindowClassEx.hIcon = LoadIcon(0, IDI_APPLICATION);
	xWindowClassEx.hCursor = LoadCursor(0, IDC_ARROW);
	xWindowClassEx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	xWindowClassEx.lpszMenuName = 0;
	xWindowClassEx.lpszClassName = acClassName;
	xWindowClassEx.hIconSm = LoadIcon(0, IDI_APPLICATION);

	RegisterClassEx(&xWindowClassEx);

	s_hWindow = CreateWindowEx(0, acClassName, pcWindowTitle, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, nWidth, nHeight, 0, 0, hInstance, 0);
	if (s_hWindow == 0) {
		printf("Failed creating window\n");
		return 0;
	}

	s_hDeviceContext = GetDC(s_hWindow);
	if (s_hDeviceContext == 0) {
		printf("Failed creating device context\n");
		return 0;
	}

	memset(&xPixelFormatDesc, 0, sizeof(xPixelFormatDesc));
	xPixelFormatDesc.nSize = sizeof(xPixelFormatDesc);
	xPixelFormatDesc.nVersion = 1;
	xPixelFormatDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	xPixelFormatDesc.iPixelType = PFD_TYPE_RGBA;
	xPixelFormatDesc.cColorBits = 32;
	xPixelFormatDesc.cDepthBits = 24;
	xPixelFormatDesc.iLayerType = PFD_MAIN_PLANE;

	INT nPixelFormat = ChoosePixelFormat(s_hDeviceContext, &xPixelFormatDesc);
	SetPixelFormat(s_hDeviceContext, nPixelFormat, &xPixelFormatDesc);

	s_hOpenGl32 = LoadLibrary("opengl32.dll");
	s_pWglCreateContext = (PFNWGLCREATECONTEXTPROC)GetProcAddress(s_hOpenGl32, "wglCreateContext");
	s_pWglMakeCurrent = (PFNWGLMAKECURRENTPROC)GetProcAddress(s_hOpenGl32, "wglMakeCurrent");
	s_pWglDeleteContext = (PFNWGLDELETECONTEXTPROC)GetProcAddress(s_hOpenGl32, "wglDeleteContext");

	s_hWglContext = s_pWglCreateContext(s_hDeviceContext);
	if (s_hWglContext == 0) {
		printf("Failed creating WGL context\n");
		return 0;
	}

	if (s_pWglMakeCurrent(s_hDeviceContext, s_hWglContext) == 0) {
		printf("Failed making context current\n");
		return 0;
	}

	ShowWindow(s_hWindow, SW_SHOW);
	UpdateWindow(s_hWindow);
#endif

#ifdef OS_LINUX
	s_pxDisplay = wl_display_connect(0);
	if (s_pxDisplay == 0) {
		printf("Failed creating wayland display\n");
		return 0;
	}

	s_pxRegistry = wl_display_get_registry(s_pxDisplay);
	if (s_pxRegistry == 0) {
		printf("Failed creating wayland registry\n");
		return 0;
	}

	wl_registry_add_listener(s_pxRegistry, &s_xRgistryListener, 0);

	wl_display_roundtrip(s_pxDisplay);

	if ((s_pxCompositor == 0) || (s_pxXdgWmBase == 0)) {
		if (s_pxCompositor == 0) {
			printf("Failed creating compositor\n");
		}
		if (s_pxXdgWmBase == 0) {
			printf("Failed creating XDG base\n");
		}
		return 0;
	}

	s_pxSurface = wl_compositor_create_surface(s_pxCompositor);
	if (s_pxSurface == 0) {
		printf("Failed creating wayland surface\n");
		return 0;
	}

	s_pxXdgSurface = xdg_wm_base_get_xdg_surface(s_pxXdgWmBase, s_pxSurface);
	if (s_pxXdgSurface == 0) {
		printf("Failed creating XDG surface\n");
		return 0;
	}

	s_pxXdgTopLevel = xdg_surface_get_toplevel(s_pxXdgSurface);

	xdg_toplevel_set_title(s_pxXdgTopLevel, pcWindowTitle);

	wl_surface_commit(s_pxSurface);

	s_pxRegion = wl_compositor_create_region(s_pxCompositor); // TODO

	wl_region_add(s_pxRegion, 0, 0, nWidth, nHeight);
	
	wl_surface_set_opaque_region(s_pxSurface, s_pxRegion);

	s_pxEglWindow = wl_egl_window_create(s_pxSurface, nWidth, nHeight);
	if (s_pxEglWindow == 0) {
		printf("Failed creating EGL window\n");
		return 0;
	}

	s_pxEglDisplay = eglGetDisplay(s_pxDisplay);
	if (s_pxEglDisplay == 0) {
		printf("Failed creating EGL display\n");
		return 0;
	}

	int32_t nMajor, nMinor;
	if (eglInitialize(s_pxEglDisplay, &nMajor, &nMinor) == 0) {
		printf("Failed initializing EGL\n");
		return 0;
	}

	printf("Found GL version %d.%d\n", nMajor, nMinor);

	EGLConfig xConfig;

	int32_t anConfigAttribs[] = {
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
		EGL_NONE
	};

	int32_t nNumConfigs;

	if (eglChooseConfig(s_pxEglDisplay, anConfigAttribs, &xConfig, 1, &nNumConfigs) == 0) {
		printf("Failed choosing EGL config\n");
		return 0;
	}

	s_pxEglSurface = eglCreateWindowSurface(s_pxEglDisplay, xConfig, s_pxEglWindow, 0);
	if (s_pxEglSurface == EGL_NO_SURFACE) {
		printf("Failed creating EGL surface\n");
		return 0;
	}

	int32_t anContextAttribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	s_pxEglContext = eglCreateContext(s_pxEglDisplay, xConfig, EGL_NO_CONTEXT, anContextAttribs);
	if (s_pxEglContext == EGL_NO_CONTEXT) {
		printf("Failed creating EGL context\n");
		return 0;
	}

	if (eglMakeCurrent(s_pxEglDisplay, s_pxEglSurface, s_pxEglSurface, s_pxEglContext) == 0) {
		printf("Failed making context current\n");
		return -1;
	}
#endif

	return &s_xWindow;
}

bool Window_ShouldNotClose(struct xWindow_t* pxWindow) {
	return pxWindow->bShouldClose == 0;
}

void Window_PollEvents(struct xWindow_t* pxWindow) {
#ifdef OS_WINDOWS
	MSG xMsg;
	if (PeekMessage(&xMsg, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&xMsg);
		DispatchMessage(&xMsg);
	}
#endif

#ifdef OS_LINUX
	wl_display_dispatch(s_pxDisplay);
	//wl_display_dispatch_pending(s_pxDisplay);
#endif
}

void Window_SwapBuffers(struct xWindow_t* pxWindow) {
#ifdef OS_WINDOWS
	SwapBuffers(s_hDeviceContext);
#endif

#ifdef OS_LINUX
	eglSwapBuffers(s_pxEglDisplay, s_pxEglSurface);
#endif
}

void Window_Free(struct xWindow_t* pxWindow) {
#ifdef OS_WINDOWS
	s_pWglMakeCurrent(0, 0);
	s_pWglDeleteContext(s_hWglContext);

	ReleaseDC(s_hWindow, s_hDeviceContext);

	DestroyWindow(s_hWindow);

	FreeLibrary(s_hOpenGl32);
#endif

#ifdef OS_LINUX
	eglMakeCurrent(s_pxEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroySurface(s_pxEglDisplay, s_pxEglSurface);

	wl_egl_window_destroy(s_pxEglWindow);

	xdg_toplevel_destroy(s_pxXdgTopLevel);
	xdg_surface_destroy(s_pxXdgSurface);

	eglDestroyContext(s_pxEglDisplay, s_pxEglContext);

	wl_surface_destroy(s_pxSurface);
	wl_compositor_destroy(s_pxCompositor);
	wl_registry_destroy(s_pxRegistry);
	wl_display_disconnect(s_pxDisplay);
#endif
}
