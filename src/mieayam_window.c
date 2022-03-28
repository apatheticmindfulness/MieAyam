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
} mieayam_mouse_internal;

typedef struct
{
	int32_t index;
	HWND handle;
} mieayam_window_internal;

typedef struct
{
	mieayam_window_internal			window[MAX_WINDOW_COUNT];
	mieayam_mouse_internal			mouses[MAX_WINDOW_COUNT];
} mieayam_window_handles;


static uint32_t							_mieayam_window_count;				// Store how many windows are
static uint32_t							_mieayam_window_track_window_count; // Track current the count of the windows
static int32_t							_mieayam_current_active_id;			// Store current active window id
static int32_t							_mieayam_current_active_index;		// Store current active window index from an array
static WNDCLASSEX						_mieayam_window_class;				// Store window main class
static mieayam_window_handles			_mieayam_window_handle;				// Store all the window handles				


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

void MieAyam_CreateWindow(const mieayam_window_attributes * const window_attributes, int32_t count, MIEAYAM_WINDOW_SHOW show)
{
	_mieayam_window_count = count;
	_mieayam_window_track_window_count = count;

	for (int32_t i = 0; i < count; i++)
	{
		RECT rc = { 0, 0, (LONG)window_attributes[i].width, (LONG)window_attributes[i].height };

		if (!AdjustWindowRect(&rc, WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, false))
		{
			// TODO : Error log
			return;
		}

		_mieayam_window_handle.window[i].index = i;
		_mieayam_window_handle.window[i].handle = CreateWindow(
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

		if (!_mieayam_window_handle.window[i].handle)
		{
			// TODO : Error log
			return;
		}

		// Show all the windows
		if (show == MIEAYAM_WINDOW_SHOW_ALL)
		{
			if (!ShowWindow(_mieayam_window_handle.window[i].handle, SW_SHOW))
			{
				// TODO : Error log
			}
		}

	}

	// Show only one window (always index 0)
	if (show == MIEAYAM_WINDOW_SHOW_ONE)
	{
		if (!ShowWindow(_mieayam_window_handle.window[0].handle, SW_SHOW))
		{
			// TODO : Error log
		}
	}
}

void MieAyam_ShowWindow(int32_t window_index)
{
	if (!ShowWindow(_mieayam_window_handle.window[window_index].handle, SW_SHOW))
	{
		// TODO : Error log
	}
}

int32_t MieAyam_GetCurrentActiveWindowId()
{
	return _mieayam_current_active_id;
}

int32_t MieAyam_GetCurrentActiveWindowIndex()
{
	return _mieayam_current_active_index;
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
	return _mieayam_window_handle.mouses[_mieayam_current_active_index].state == MIEAYAM_MOUSE_CLICKED;
}

uint8_t MieAyam_MouseLeftIsReleased()
{
	if (_mieayam_window_handle.mouses[_mieayam_current_active_index].state == MIEAYAM_MOUSE_RELEASED)
	{
		return true;
	}

	return false;
}

int32_t MieAyam_GetMouseX()
{
	return _mieayam_window_handle.mouses[_mieayam_current_active_index].x;
}

int32_t Mieayam_GetMouseY()
{
	return _mieayam_window_handle.mouses[_mieayam_current_active_index].y;
}

LRESULT CALLBACK _mieayam_Win32HandleProcess(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (uMsg)
	{
		case WM_ACTIVATE:
		{
			HWND activeWindow = GetForegroundWindow();
			for (int32_t i = 0; i < (int32_t)_mieayam_window_count; i++)
			{
				if (activeWindow == _mieayam_window_handle.window[i].handle)
				{
					_mieayam_current_active_index = i;
				}
			}
		}
		break;

		case WM_CLOSE:
		{
			_mieayam_window_track_window_count--;
			DestroyWindow(_mieayam_window_handle.window[_mieayam_current_active_index].handle);
		}
		break;

		case WM_DESTROY:
		{
			if (_mieayam_window_track_window_count == 0)
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
			_mieayam_window_handle.mouses[_mieayam_current_active_index].state = MIEAYAM_MOUSE_CLICKED;
			_mieayam_window_handle.mouses[_mieayam_current_active_index].x = (int32_t)point.x;
			_mieayam_window_handle.mouses[_mieayam_current_active_index].y = (int32_t)point.y;

		}
		break;

		case WM_LBUTTONUP:
		{
			const POINTS point = MAKEPOINTS(lParam);
			_mieayam_window_handle.mouses[_mieayam_current_active_index].state = MIEAYAM_MOUSE_RELEASED;
			_mieayam_window_handle.mouses[_mieayam_current_active_index].x = (int32_t)point.x;
			_mieayam_window_handle.mouses[_mieayam_current_active_index].y = (int32_t)point.y;
		}
		break;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
