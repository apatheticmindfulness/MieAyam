#include <stdlib.h>
#include "may_window.h"


struct MAY_Window
{
	uint32_t width;
	uint32_t height;
	const char * title;

	// Win32
	HWND window_handle;

	uint8_t isValid;
};

static WNDCLASSEX _MAY_WindowClass;

uint8_t MAY_Init()
{
	_MAY_WindowClass.cbSize = sizeof(WNDCLASSEX);
	_MAY_WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	_MAY_WindowClass.lpfnWndProc = DefWindowProc;
	_MAY_WindowClass.hInstance = GetModuleHandle(0);
	_MAY_WindowClass.hCursor = LoadCursor(0, IDC_ARROW);
	_MAY_WindowClass.lpszMenuName = 0;
	_MAY_WindowClass.lpszClassName = "MIE_AYAM_WINDOW";

	if (!RegisterClassEx(&_MAY_WindowClass))
	{
		return false;
	}

	return true;
}

MAY_Window * MAY_CreateWindow(uint32_t width, uint32_t height, const char * title)
{
	// Create and allocate window
	MAY_Window * window = (MAY_Window*)malloc(sizeof(MAY_Window));
	window->width = width;
	window->height = height;
	window->title = title;

	// Create window handle and adjusting it
	RECT rc = { 0, 0, (LONG)width, (LONG)height };
	AdjustWindowRect(&rc, WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, false);

	HWND windowHandle = CreateWindow(
		_MAY_WindowClass.lpszClassName,
		title,
		WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		0,
		0,
		GetModuleHandle(0),
		0);

	// Set window handle
	window->window_handle = windowHandle;

	if (!window->window_handle)
	{
		window->isValid = false;
	}

	return window;
}

uint8_t MAY_ShowWindow(const MAY_Window * const window)
{
	if (ShowWindow(window->window_handle, SW_SHOW))
	{
		return true;
	}
	return false;
}

uint8_t MAY_ProcessMessage()
{
	MSG msg = { 0 };
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
		{
			return false;
		}

		if (GetAsyncKeyState(VK_ESCAPE))
		{
			return false;
		}
	}

	return true;
}

void MAY_FreeWindow(MAY_Window * window)
{
	if (window)
	{
		free(window);
		window = 0;
	}
}
