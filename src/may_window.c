#include <stdlib.h>
#include "may_window.h"

struct MAY_Mouse
{
	int8_t		isValid;
	int32_t		x;
	int32_t		y;
	MAY_MOUSE_STATE state;
};

struct MAY_Window
{
	int32_t		width;
	int32_t		height;
	int8_t		isValid;
	void		*pHandle;
	const char	*pTitle;

	MAY_Mouse mouse;
};

// Variables
static WNDCLASSEX _MAY_WindowClass;

// Functions
static LRESULT CALLBACK _MAY_Win32HandleMessageState(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK _MAY_Win32HandleProcess(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static inline MAY_Window * _MAY_GetWindowState(HWND hwnd);
static void _MAY_SendMouseLeftClickedState(MAY_Window * window, int32_t x, int32_t y);
static void _MAY_SendMouseLeftReleased(MAY_Window * window, int32_t x, int32_t y);

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

uint8_t MAY_ShowWindow(MAY_Window * window)
{
	if (ShowWindow(window->pHandle, SW_SHOW))
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

uint8_t MAY_MouseLeftIsPressed(MAY_Mouse * mouse)
{
	return mouse->state == MAY_MOUSE_CLICKED;
}

uint8_t MAY_MouseLeftIsReleased(MAY_Mouse * mouse)
{
	if (mouse->state == MAY_MOUSE_RELEASED)
	{
		return true;
	}
	return false;
}

MAY_Window * MAY_CreateWindow(uint32_t width, uint32_t height, const char * title)
{
	MAY_Window * window = (MAY_Window*)malloc(sizeof(MAY_Window));
	if (window)
	{
		// Zero all for window
		memset(window, 0, sizeof(MAY_Window));
		window->width = width;
		window->height = height;
		window->pTitle = title;

		// Create window handle and adjusting it
		RECT rc = { 0, 0, (LONG)window->width, (LONG)window->height };
		AdjustWindowRect(&rc, WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, false);

		HWND windowHandle = CreateWindow(
			_MAY_WindowClass.lpszClassName,
			window->pTitle,
			WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			rc.right - rc.left,
			rc.bottom - rc.top,
			0,
			0,
			GetModuleHandle(0),
			window);

		window->pHandle = windowHandle;

		if (!window->pHandle)
		{
			window->isValid = false;
		}
		return window;
	}

	return 0;
}

void MAY_FreeWindow(MAY_Window * window)
{
	if (window)
	{
		free(window);
		window = 0;
	}
}

void * MAY_GetWindowHandle(MAY_Window * window)
{
	if (window)
	{
		return window->pHandle;
	}
	return 0;
}

void * MAY_GetWindowMouseHandle(MAY_Window * window)
{
	if (window)
	{
		return &window->mouse;
	}
	return 0;
}

int32_t MAY_GetMouseX(const MAY_Window * const window)
{
	if (window)
	{
		return window->mouse.x;
	}
	return 0;
}

int32_t MAY_GetMouseY(const MAY_Window * const window)
{
	if (window)
	{
		return window->mouse.x;
	}
	return 0;
}

LRESULT _MAY_Win32HandleMessageState(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_NCCREATE)
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
	LONG_PTR pWindowData = GetWindowLongPtr(hwnd, GWLP_USERDATA);
	MAY_Window * pWindowState = _MAY_GetWindowState(hwnd);
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
		break;

		// Mouse input
		case WM_LBUTTONDOWN:
		{
			const POINTS point = MAKEPOINTS(lParam);
			_MAY_SendMouseLeftClickedState(pWindowState, point.x, point.y);
		}
		break;

		case WM_LBUTTONUP:
		{
			const POINTS point = MAKEPOINTS(lParam);
			_MAY_SendMouseLeftReleased(pWindowState, point.x, point.y);
		}
		break;

	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

inline MAY_Window * _MAY_GetWindowState(HWND hwnd)
{
	LONG_PTR pWindowData = GetWindowLongPtr(hwnd, GWLP_USERDATA);
	MAY_Window * pWindow = (MAY_Window*)pWindowData;
	return pWindow;
}

void _MAY_SendMouseLeftClickedState(MAY_Window * window, int32_t x, int32_t y)
{
	window->mouse.state = MAY_MOUSE_CLICKED;
	window->mouse.x = x;
	window->mouse.y = y;
}

void _MAY_SendMouseLeftReleased(MAY_Window * window, int32_t x, int32_t y)
{
	window->mouse.state = MAY_MOUSE_RELEASED;
	window->mouse.x = x;
	window->mouse.y = y;
}


