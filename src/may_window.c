#include <stdlib.h>
#include "may_window.h"

// Variables
static WNDCLASSEX _MAY_WindowClass;

// Functions
static LRESULT CALLBACK _MAY_Win32HandleMessageState(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK _MAY_Win32HandleProcess(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

uint8_t MAY_Init()
{
	_MAY_WindowClass.cbSize = sizeof(WNDCLASSEX);
	_MAY_WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	_MAY_WindowClass.lpfnWndProc = _MAY_Win32HandleMessageState;
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

uint8_t MAY_ShowWindow(MAY_Window window)
{
	if (ShowWindow(window.pHandle, SW_SHOW))
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

MAY_Window MAY_CreateWindow(uint32_t width, uint32_t height, const char * title)
{
	MAY_Window window =
	{
		.width = width,
		.height = height,
		.pTitle = title,
		.isValid = true
	};

	// Create window handle and adjusting it
	RECT rc = { 0, 0, (LONG)window.width, (LONG)window.height };
	AdjustWindowRect(&rc, WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, false);

	HWND windowHandle = CreateWindow(
		_MAY_WindowClass.lpszClassName,
		window.pTitle,
		WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		0,
		0,
		GetModuleHandle(0),
		&window);

	window.pHandle = windowHandle;

	if (!window.pHandle)
	{
		window.isValid = false;
	}

	return window;
}

LRESULT _MAY_Win32HandleMessageState(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_CREATE)
	{
		CREATESTRUCT *pCreate = (CREATESTRUCT*)(lParam);
		MAY_Window * pWindowState = (MAY_Window*)(pCreate->lpCreateParams);

		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(pWindowState));
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)(&_MAY_Win32HandleProcess));
		_MAY_Win32HandleProcess(hwnd, uMsg, wParam, lParam);
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT _MAY_Win32HandleProcess(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (uMsg)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		break;

		case WM_PAINT:
		{
			hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
		}

	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
