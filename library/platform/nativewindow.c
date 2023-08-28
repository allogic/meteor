#include <stdio.h>
#include <string.h>

#include <macros.h>

#include <platform/nativewindow.h>

#ifdef OS_WINDOWS
#	include <windows.h>
#endif

#ifdef OS_LINUX
#	include <wayland-client.h>
#	include <wayland-client-protocol.h>

#	include "platform/xdgshell.h"
#endif

struct xNativeWindow_t {
#ifdef OS_WINDOWS
	HMODULE xInstance;
	HWND xWindow;
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
	uint32_t nWidth;
	uint32_t nHeight;
	uint32_t nMouseX;
	uint32_t nMouseY;
};

static struct xNativeWindow_t s_xNativeWindow;

#ifdef OS_WINDOWS
static LRESULT WndProc(HWND xWindow, UINT nMessage, WPARAM wParam, LPARAM lParam) {
	switch (nMessage) {
		case WM_CLOSE: {
			s_xNativeWindow.bShouldClose = true;
		}
		break;
		case WM_MOUSEMOVE: {
			s_xNativeWindow.nMouseX = LOWORD(lParam);
			s_xNativeWindow.nMouseY = HIWORD(lParam);
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
		s_xNativeWindow.pxCompositor = wl_registry_bind(pxRegistry, nId, &wl_compositor_interface, 1);
	} else if (strcmp(pcInterface, "xdg_wm_base") == 0) {
		s_xNativeWindow.pxXdgWmBase = wl_registry_bind(pxRegistry, nId, &xdg_wm_base_interface, 1);
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

struct xNativeWindow_t* NativeWindow_Alloc(const char* pcTitle, uint32_t nWidth, uint32_t nHeight) {
#ifdef OS_WINDOWS
	s_xNativeWindow.xInstance = GetModuleHandle(0);

	WNDCLASSEX xWindowClassEx;
	memset(&xWindowClassEx, 0, sizeof(xWindowClassEx));
	xWindowClassEx.cbSize = sizeof(xWindowClassEx);
	xWindowClassEx.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	xWindowClassEx.lpfnWndProc = WndProc;
	xWindowClassEx.cbClsExtra = 0;
	xWindowClassEx.cbWndExtra = 0;
	xWindowClassEx.hInstance = s_xNativeWindow.xInstance;
	xWindowClassEx.hIcon = LoadIcon(0, IDI_APPLICATION);
	xWindowClassEx.hCursor = LoadCursor(0, IDC_ARROW);
	xWindowClassEx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	xWindowClassEx.lpszMenuName = 0;
	xWindowClassEx.lpszClassName = WIN32_CLASS_NAME;
	xWindowClassEx.hIconSm = LoadIcon(0, IDI_APPLICATION);

	RegisterClassEx(&xWindowClassEx);

	s_xNativeWindow.xWindow = CreateWindowEx(0, WIN32_CLASS_NAME, pcTitle, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, nWidth, nHeight, 0, 0, s_xNativeWindow.xInstance, 0);

	ShowWindow(s_xNativeWindow.xWindow, SW_SHOW);
	UpdateWindow(s_xNativeWindow.xWindow);
#endif

#ifdef OS_LINUX
	s_xNativeWindow.pxDisplay = wl_display_connect(0);
	s_xNativeWindow.pxRegistry = wl_display_get_registry(s_xNativeWindow.pxDisplay);

	wl_registry_add_listener(s_xNativeWindow.pxRegistry, &s_xRegistryListener, 0);

	wl_display_roundtrip(s_xNativeWindow.pxDisplay);

	s_xNativeWindow.pxSurface = wl_compositor_create_surface(s_xNativeWindow.pxCompositor);
	s_xNativeWindow.pxXdgSurface = xdg_wm_base_get_xdg_surface(s_xNativeWindow.pxXdgWmBase, s_xNativeWindow.pxSurface);
	s_xNativeWindow.pxXdgTopLevel = xdg_surface_get_toplevel(s_xNativeWindow.pxXdgSurface);

	xdg_toplevel_set_title(s_xNativeWindow.pxXdgTopLevel, pcTitle);

	wl_surface_commit(s_xNativeWindow.pxSurface);

	s_xNativeWindow.pxRegion = wl_compositor_create_region(s_xNativeWindow.pxCompositor);

	wl_region_add(s_xNativeWindow.pxRegion, 0, 0, nWidth, nHeight);
	
	wl_surface_set_opaque_region(s_xNativeWindow.pxSurface, s_xNativeWindow.pxRegion);
#endif

	s_xNativeWindow.nWidth = nWidth;
	s_xNativeWindow.nHeight = nHeight;

	return &s_xNativeWindow;
}

void NativeWindow_Free(struct xNativeWindow_t* pxNativeWindow) {
#ifdef OS_WINDOWS
	DestroyWindow(pxNativeWindow->xWindow);
#endif

#ifdef OS_LINUX
	xdg_toplevel_destroy(pxNativeWindow->pxXdgTopLevel);
	xdg_surface_destroy(pxNativeWindow->pxXdgSurface);

	wl_surface_destroy(pxNativeWindow->pxSurface);
	wl_compositor_destroy(pxNativeWindow->pxCompositor);
	wl_registry_destroy(pxNativeWindow->pxRegistry);
	wl_display_disconnect(pxNativeWindow->pxDisplay);
#endif
}

bool NativeWindow_ShouldNotClose(struct xNativeWindow_t* pxNativeWindow) {
	return pxNativeWindow->bShouldClose == 0;
}

void NativeWindow_PollEvents(struct xNativeWindow_t* pxNativeWindow) {
#ifdef OS_WINDOWS
	if (PeekMessage(&pxNativeWindow->xMsg, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&pxNativeWindow->xMsg);
		DispatchMessage(&pxNativeWindow->xMsg);
	}
#endif

#ifdef OS_LINUX
	wl_display_dispatch(pxNativeWindow->pxDisplay);
	//wl_display_dispatch_pending(pxNativeWindow->pxDisplay); // TODO
#endif
}

#ifdef OS_WINDOWS
void* NativeWindow_GetWindowHandle(struct xNativeWindow_t* pxNativeWindow) {
	return pxNativeWindow->xWindow;
}

void* NativeWindow_GetModuleHandle(struct xNativeWindow_t* pxNativeWindow) {
	return pxNativeWindow->xInstance;
}
#endif

#ifdef OS_LINUX
void* NativeWindow_GetDisplayHandle(struct xNativeWindow_t* pxNativeWindow) {
	return pxNativeWindow->pxDisplay;
}

void* NativeWindow_GetSurfaceHandle(struct xNativeWindow_t* pxNativeWindow) {
	return pxNativeWindow->pxSurface;
}
#endif

uint32_t NativeWindow_GetWidth(struct xNativeWindow_t* pxNativeWindow) {
	return pxNativeWindow->nWidth - 16; // TODO: Compute and subtract window border if any..
}

uint32_t NativeWindow_GetHeight(struct xNativeWindow_t* pxNativeWindow) {
	return pxNativeWindow->nHeight - 39; // TODO: Compute and subtract window border if any..
}
