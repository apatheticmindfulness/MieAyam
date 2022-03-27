#include <stdlib.h>
#include <stdio.h>
#include "mieayam_window.h"
#include "mieayam_config.h"

typedef enum
{
	MIEAYAM_MOUSE_NONE,
	MIEAYAM_MOUSE_CLICKED,
	MIEAYAM_MOUSE_RELEASED
} mieayam_mouse_state;

typedef struct
{
	int32_t				x;
	int32_t				y;
	mieayam_mouse_state state;
} mieayam_mouse;

typedef struct
{
	HWND				handles[WINDOW_COUNT];
	mieayam_mouse		mouses[MOUSE_COUNT];
} mieayam_window_handles;


static uint32_t							_mieayam_window_count;
static uint32_t							_mieayam_current_active;
static WNDCLASSEX						_mieayam_window_class;
static mieayam_window_handles			_mieayam_window_handle;
static mieayam_mouse					_mieayam_mouse;


static LRESULT CALLBACK					_mieayam_Win32HandleProcess(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void MieAyam_Init()
{
	_mieayam_window_class.cbSize = sizeof(WNDCLASSEX);
	_mieayam_window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	_mieayam_window_class.lpfnWndProc = _mieayam_Win32HandleProcess;
	_mieayam_window_class.hInstance = GetModuleHandle(0);
	_mieayam_window_class.hCursor = LoadCursor(0, IDC_ARROW);
	_mieayam_window_class.lpszMenuName = 0;
	_mieayam_window_class.lpszClassName = "MieAyam_MainWindow";

	if (!RegisterClassEx(&_mieayam_window_class))
	{
		// TODO : Error log
		return;
	}
}

void MieAyam_CreateMainWindow(const mieayam_window_attributes * const window_attributes, int32_t count)
{
	_mieayam_window_count = count;

	for (int32_t i = 0; i < count; i++)
	{
		RECT rc = { 0, 0, (LONG)window_attributes[i].width, (LONG)window_attributes[i].height };

		if (!AdjustWindowRect(&rc, WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, false))
		{
			// TODO : Error log
			return;
		}

		_mieayam_window_handle.handles[i] = CreateWindow(
			_mieayam_window_class.lpszClassName,
			window_attributes[i].title,
			WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			rc.right - rc.left,
			rc.bottom - rc.top,
			0,
			0,
			GetModuleHandle(0),
			0);

		if (!_mieayam_window_handle.handles[i])
		{
			// TODO : Error log
			return;
		}

		ShowWindow(_mieayam_window_handle.handles[i], SW_SHOW);
	}
}

uint8_t MieAyam_RunProccess()
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

uint8_t MieAyam_MouseLeftIsPressed()
{
	return _mieayam_mouse.state == MIEAYAM_MOUSE_CLICKED;
}

uint8_t MieAyam_MouseLeftIsReleased()
{
	if (_mieayam_mouse.state == MIEAYAM_MOUSE_CLICKED)
	{
		return true;
	}

	return false;
}

LRESULT CALLBACK _mieayam_Win32HandleProcess(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (uMsg)
	{
		case WM_ACTIVATE:
		{
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				// Get current active window
				for (int32_t i = 0; i < (int32_t)_mieayam_window_count; i++)
				{
					HWND activeWindow = (HWND)lParam;
					if (activeWindow == _mieayam_window_handle.handles[i])
					{
						_mieayam_current_active = i;
					}
				}
			}
		}
		break;

		case WM_CLOSE:
		{
			_mieayam_window_count--;
			DestroyWindow(_mieayam_window_handle.handles[_mieayam_current_active]);
		}
		break;

		case WM_DESTROY:
		{
			if (_mieayam_window_count == 0)
			{
				PostQuitMessage(0);
			}
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
			_mieayam_window_handle.mouses[_mieayam_current_active].state = MIEAYAM_MOUSE_CLICKED;
			_mieayam_window_handle.mouses[_mieayam_current_active].x = (int32_t)point.x;
			_mieayam_window_handle.mouses[_mieayam_current_active].y = (int32_t)point.y;

		}
		break;

		case WM_LBUTTONUP:
		{
			const POINTS point = MAKEPOINTS(lParam);
			_mieayam_window_handle.mouses[_mieayam_current_active].state = MIEAYAM_MOUSE_RELEASED;
			_mieayam_window_handle.mouses[_mieayam_current_active].x = (int32_t)point.x;
			_mieayam_window_handle.mouses[_mieayam_current_active].y = (int32_t)point.y;
		}
		break;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
