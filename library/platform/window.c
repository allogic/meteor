#include <stdio.h>
#include <string.h>

#include <macros.h>

#include <platform/window.h>

#ifdef OS_WINDOWS
#	include <windows.h>
#endif

#ifdef OS_LINUX
#	include <wayland-client.h>
#	include <wayland-client-protocol.h>

#	include "platform/xdgshell.h"
#endif

struct xWindow_t {
#ifdef OS_WINDOWS
	HMODULE xInstance;
	HWND xWindow;
	HDC xDeviceContext;
	MSG xMsg;
#endif
#ifdef OS_LINUX
	struct wl_display* pxDisplay;
	struct wl_registry* pxRegistry;
	struct wl_compositor* pxCompositor;
	struct wl_region* pxRegion;
	struct wl_surface* pxSurface;
	struct xdg_wm_base* pxXdgWmBase;
	struct xdg_surface* pxXdgSurface;
	struct xdg_toplevel* pxXdgTopLevel;
#endif
	bool bShouldClose;
	uint32_t nWindowWidth;
	uint32_t nWindowHeight;
	uint32_t nMouseX;
	uint32_t nMouseY;
};

static struct xWindow_t s_xWindow;

#ifdef OS_WINDOWS
static LRESULT WndProc(HWND xWindow, UINT nMessage, WPARAM wParam, LPARAM lParam) {
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
			return DefWindowProc(xWindow, nMessage, wParam, lParam);
		}
	}
	return 0;
}
#endif

#ifdef OS_LINUX
static void RegistryGlobal(void* pData, struct wl_registry* pxRegistry, uint32_t nId, const char* pcInterface, uint32_t nVersion) {
	UNUSED(pData);
	UNUSED(nVersion);

	if (strcmp(pcInterface, "wl_compositor") == 0) {
		s_xWindow.pxCompositor = wl_registry_bind(pxRegistry, nId, &wl_compositor_interface, 1);
	} else if (strcmp(pcInterface, "xdg_wm_base") == 0) {
		s_xWindow.pxXdgWmBase = wl_registry_bind(pxRegistry, nId, &xdg_wm_base_interface, 1);
	}
}

static void RegistryGlobalRemove(void* pData, struct wl_registry* pxRegistry, uint32_t nId) {
	UNUSED(pData);
	UNUSED(pxRegistry);
	UNUSED(nId);
}

static const struct wl_registry_listener s_xRegistryListener = {
	.global = RegistryGlobal,
	.global_remove = RegistryGlobalRemove,
};
#endif

struct xWindow_t* Window_Alloc(const char* pcWindowTitle, uint32_t nWidth, uint32_t nHeight) {
#ifdef OS_WINDOWS
	s_xWindow.xInstance = GetModuleHandle(0);

	WNDCLASSEX xWindowClassEx;
	memset(&xWindowClassEx, 0, sizeof(xWindowClassEx));
	xWindowClassEx.cbSize = sizeof(xWindowClassEx);
	xWindowClassEx.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	xWindowClassEx.lpfnWndProc = WndProc;
	xWindowClassEx.cbClsExtra = 0;
	xWindowClassEx.cbWndExtra = 0;
	xWindowClassEx.hInstance = s_xWindow.xInstance;
	xWindowClassEx.hIcon = LoadIcon(0, IDI_APPLICATION);
	xWindowClassEx.hCursor = LoadCursor(0, IDC_ARROW);
	xWindowClassEx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	xWindowClassEx.lpszMenuName = 0;
	xWindowClassEx.lpszClassName = WIN32_CLASS_NAME;
	xWindowClassEx.hIconSm = LoadIcon(0, IDI_APPLICATION);

	RegisterClassEx(&xWindowClassEx);

	s_xWindow.xWindow = CreateWindowEx(0, WIN32_CLASS_NAME, pcWindowTitle, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, nWidth, nHeight, 0, 0, s_xWindow.xInstance, 0);
	if (s_xWindow.xWindow == 0) {
		printf("Failed creating window\n");
		return 0;
	}

	s_xWindow.xDeviceContext = GetDC(s_xWindow.xWindow);
	if (s_xWindow.xDeviceContext == 0) {
		printf("Failed creating device context\n");
		return 0;
	}

	PIXELFORMATDESCRIPTOR xPixelFormatDesc;
	memset(&xPixelFormatDesc, 0, sizeof(xPixelFormatDesc));
	xPixelFormatDesc.nSize = sizeof(xPixelFormatDesc);
	xPixelFormatDesc.nVersion = 1;
	xPixelFormatDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	xPixelFormatDesc.iPixelType = PFD_TYPE_RGBA;
	xPixelFormatDesc.cColorBits = 32;
	xPixelFormatDesc.cDepthBits = 24;
	xPixelFormatDesc.iLayerType = PFD_MAIN_PLANE;

	INT nPixelFormat = ChoosePixelFormat(s_xWindow.xDeviceContext, &xPixelFormatDesc);
	SetPixelFormat(s_xWindow.xDeviceContext, nPixelFormat, &xPixelFormatDesc);

	ShowWindow(s_xWindow.xWindow, SW_SHOW);
	UpdateWindow(s_xWindow.xWindow);
#endif

#ifdef OS_LINUX
	s_xWindow.pxDisplay = wl_display_connect(0);
	if (s_xWindow.pxDisplay == 0) {
		printf("Failed creating wayland display\n");
		return 0;
	}

	s_xWindow.pxRegistry = wl_display_get_registry(s_xWindow.pxDisplay);
	if (s_xWindow.pxRegistry == 0) {
		printf("Failed creating wayland registry\n");
		return 0;
	}

	wl_registry_add_listener(s_xWindow.pxRegistry, &s_xRegistryListener, 0);

	wl_display_roundtrip(s_xWindow.pxDisplay);

	if ((s_xWindow.pxCompositor == 0) || (s_xWindow.pxXdgWmBase == 0)) {
		if (s_xWindow.pxCompositor == 0) {
			printf("Failed creating compositor\n");
		}
		if (s_xWindow.pxXdgWmBase == 0) {
			printf("Failed creating XDG base\n");
		}
		return 0;
	}

	s_xWindow.pxSurface = wl_compositor_create_surface(s_xWindow.pxCompositor);
	if (s_xWindow.pxSurface == 0) {
		printf("Failed creating wayland surface\n");
		return 0;
	}

	s_xWindow.pxXdgSurface = xdg_wm_base_get_xdg_surface(s_xWindow.pxXdgWmBase, s_xWindow.pxSurface);
	if (s_xWindow.pxXdgSurface == 0) {
		printf("Failed creating XDG surface\n");
		return 0;
	}

	s_xWindow.pxXdgTopLevel = xdg_surface_get_toplevel(s_xWindow.pxXdgSurface);

	xdg_toplevel_set_title(s_xWindow.pxXdgTopLevel, pcWindowTitle);

	wl_surface_commit(s_xWindow.pxSurface);

	s_xWindow.pxRegion = wl_compositor_create_region(s_xWindow.pxCompositor);

	wl_region_add(s_xWindow.pxRegion, 0, 0, nWidth, nHeight);
	
	wl_surface_set_opaque_region(s_xWindow.pxSurface, s_xWindow.pxRegion);
#endif

	s_xWindow.nWindowWidth = nWidth;
	s_xWindow.nWindowHeight = nHeight;

	return &s_xWindow;
}

void Window_Free(struct xWindow_t* pxWindow) {
#ifdef OS_WINDOWS
	ReleaseDC(pxWindow->xWindow, pxWindow->xDeviceContext);

	DestroyWindow(pxWindow->xWindow);
#endif

#ifdef OS_LINUX
	xdg_toplevel_destroy(pxWindow->pxXdgTopLevel);
	xdg_surface_destroy(pxWindow->pxXdgSurface);

	wl_surface_destroy(pxWindow->pxSurface);
	wl_compositor_destroy(pxWindow->pxCompositor);
	wl_registry_destroy(pxWindow->pxRegistry);
	wl_display_disconnect(pxWindow->pxDisplay);
#endif
}

bool Window_ShouldNotClose(struct xWindow_t* pxWindow) {
	return pxWindow->bShouldClose == 0;
}

void Window_PollEvents(struct xWindow_t* pxWindow) {
#ifdef OS_WINDOWS
	if (PeekMessage(&pxWindow->xMsg, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&pxWindow->xMsg);
		DispatchMessage(&pxWindow->xMsg);
	}
#endif

#ifdef OS_LINUX
	wl_display_dispatch(pxWindow->pxDisplay);
	//wl_display_dispatch_pending(pxWindow->pxDisplay); // TODO
#endif
}

void Window_SwapBuffers(struct xWindow_t* pxWindow) {
	UNUSED(pxWindow);
}

#ifdef OS_WINDOWS
void* Window_GetWindowHandle(struct xWindow_t* pxWindow) {
	return pxWindow->xWindow;
}

void* Window_GetModuleHandle(struct xWindow_t* pxWindow) {
	return pxWindow->xInstance;
}
#endif

uint32_t Window_GetWidth(struct xWindow_t* pxWindow) {
	return pxWindow->nWindowWidth;
}

uint32_t Window_GetHeight(struct xWindow_t* pxWindow) {
	return pxWindow->nWindowHeight;
}
