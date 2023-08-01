#include "window.h"

#include <windows.h>

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

void Window_AllocWindows(const char* pcWindowTitle) {
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
}
void Window_PollEventsWindows(void) {
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
void Window_SwapBuffers(void) {
	SwapBuffers(s_Hdc);
}
void Window_FreeWindows(void) {
	//wglMakeCurrent(0, 0);
	//wglDeleteContext(s_Hrc);
	ReleaseDC(s_Hwnd, s_Hdc);
	DestroyWindow(s_Hwnd);
}
