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

#ifdef OS_WINDOWS
static HWND s_Hwnd;
static HDC s_Hdc;
//static HGLRC s_Hrc;

static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_CLOSE: {
			PostQuitMessage(0);
		}
		break;
		case WM_DESTROY: {
			PostQuitMessage(0);
		}
		break;
		default: {
			return DefWindowProc(hwnd, message, wParam, lParam);
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

int32_t Window_Alloc(const char* pcWindowTitle) {
	UNUSED(pcWindowTitle); // TODO

#ifdef OS_WINDOWS
	const char className[] = "OpenGLWin32Class";

	HINSTANCE hinstance = GetModuleHandle(0);
	WNDCLASSEX wcex;
	PIXELFORMATDESCRIPTOR pfd;

	ZeroMemory(&wcex, sizeof(WNDCLASSEX));
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hinstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(0, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = className;
	wcex.hIconSm = 0;

	RegisterClassEx(&wcex);

	s_Hwnd = CreateWindowEx(
		0, className, pcWindowTitle,
		WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0, 0, 800, 600,
		0, 0, hinstance, 0);

	s_Hdc = GetDC(s_Hwnd);

	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.iLayerType = PFD_MAIN_PLANE;

	SetPixelFormat(s_Hdc, ChoosePixelFormat(s_Hdc, &pfd), &pfd);

	//s_Hrc = wglCreateContext(s_Hdc);
	//wglMakeCurrent(s_Hdc, s_Hrc);

	ShowWindow(s_Hwnd, SW_SHOW);
	UpdateWindow(s_Hwnd);
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
#endif

	gladLoadGL();

	return 0;
}

void Window_PollEvents(void) {
#ifdef OS_WINDOWS
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
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
	SwapBuffers(s_Hdc);
#endif

#ifdef OS_LINUX
	eglSwapBuffers(s_pxEglDisplay, s_pxEglSurface);
#endif
}

void Window_Free(void) {
#ifdef OS_WINDOWS
	//wglMakeCurrent(0, 0); // TODO
	//wglDeleteContext(s_Hrc);

	ReleaseDC(s_Hwnd, s_Hdc);
	DestroyWindow(s_Hwnd);
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
