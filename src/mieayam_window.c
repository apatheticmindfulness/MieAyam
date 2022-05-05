#include "mieayam_window.h"
#include "mieayam_config.h"

// Mouse stuff
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


// Keyboard stuff
typedef enum
{
	MIEAYAM_KEYBOARD_NONE,
	MIEAYAM_KEYBOARD_PRESSED,
	MIEAYAM_KEYBOARD_RELEASED
} mieayam_keyboard_state;

typedef struct
{
	mieayam_keyboard_state state;
	uint8_t autoRepeat;
	uint8_t keyCode[256];
} mieayam_keyboard_internal;


// Main window stuff
typedef HWND mieayam_window_internal;
typedef struct
{
	mieayam_window_internal			window[MAX_WINDOW_COUNT];
	mieayam_keyboard_internal		keyboard[MAX_WINDOW_COUNT];
	mieayam_mouse_internal			mouses[MAX_WINDOW_COUNT];
} mieayam_window_handles;

// Variables
static uint32_t							_mieayam_window_count;				// Store how many windows are
static uint32_t							_mieayam_window_track_window_count; // Track current the count of the windows
static int32_t							_mieayam_current_active_index;		// Store current active window index from an array
static WNDCLASSEX						_mieayam_window_class;				// Store window main class
static mieayam_window_handles			_mieayam_window_handle;				// Store all the window handles				

// Functions
static LRESULT CALLBACK					_mieayam_Win32HandleProcess(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void MieAyam_Init(void)
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

uint8_t MieAyam_CreateWindow(const mieayam_window_attributes * const window_attributes, int32_t count)
{
	_mieayam_window_count = count;
	_mieayam_window_track_window_count = count;

	for (int32_t i = 0; i < count; i++)
	{
		int32_t windowWidth = window_attributes[i].width;
		int32_t windowHeight = window_attributes[i].height;
		const char * title = window_attributes[i].title;

		RECT rc = { 0, 0, (LONG)windowWidth, (LONG)windowHeight };
		if (!AdjustWindowRect(&rc, WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, false))
		{
			// TODO : Error log
			return false;
		}

		_mieayam_window_handle.window[i] = CreateWindow(
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

		if (!_mieayam_window_handle.window[i])
		{
			// TODO : Error log
			return false;
		}
	}

	return true;
}

int32_t MieAyam_GetCurrentActiveWindowIndex(void)
{
	return _mieayam_current_active_index;
}

int32_t MieAyam_GetWindowCount(void)
{
	return _mieayam_window_count;
}

uint8_t MieAyam_RunProccess(void)
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
	}

	return true;
}

uint8_t MieAyam_ShowWindow(int32_t window_index)
{
	if (ShowWindow(_mieayam_window_handle.window[window_index], SW_SHOW))
	{
		return true;
	}
	return false;
}

HWND MieAyam_GetWindowHandle(int32_t index)
{
	return _mieayam_window_handle.window[index];
}

uint8_t MieAyam_KeyboardIsPressed(uint32_t key_code)
{
	return _mieayam_window_handle.keyboard[_mieayam_current_active_index].keyCode[key_code];
}

uint8_t MieAyam_KeyboardIsReleased(uint32_t key_code)
{
	if (_mieayam_window_handle.keyboard[_mieayam_current_active_index].state == MIEAYAM_KEYBOARD_RELEASED)
	{
		return _mieayam_window_handle.keyboard[_mieayam_current_active_index].keyCode[key_code] == false; // this is return true
	}

	return false;
}

uint8_t MieAyam_MouseLeftIsPressed(void)
{
	return _mieayam_window_handle.mouses[_mieayam_current_active_index].state == MIEAYAM_MOUSE_CLICKED;
}

uint8_t MieAyam_MouseLeftIsReleased(void)
{
	if (_mieayam_window_handle.mouses[_mieayam_current_active_index].state == MIEAYAM_MOUSE_RELEASED)
	{
		return true;
	}

	return false;
}

int32_t MieAyam_GetMouseX(void)
{
	return _mieayam_window_handle.mouses[_mieayam_current_active_index].x;
}

int32_t MieAyam_GetMouseY(void)
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
				if (activeWindow == _mieayam_window_handle.window[i])
				{
					_mieayam_current_active_index = i;
					//break;
				}
			}
		}
		break;

		case WM_CLOSE:
		{
			_mieayam_window_track_window_count--;
			DestroyWindow(_mieayam_window_handle.window[_mieayam_current_active_index]);
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

		case WM_KILLFOCUS:
		{
			// Kill focus
		}
		break;

		// Keyboard input
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			uint32_t keyCode = (uint32_t)wParam;
			uint32_t keyIsDown = (uint32_t)(lParam & (1 << 30));
			if (keyIsDown == 0u) // No auto repeat
			{
				_mieayam_window_handle.keyboard[_mieayam_current_active_index].state = MIEAYAM_KEYBOARD_PRESSED;
				_mieayam_window_handle.keyboard[_mieayam_current_active_index].keyCode[keyCode] = true;
			}
		}
		break;

		case WM_SYSKEYUP:
		case WM_KEYUP:
		{
			uint32_t keyCode = (uint32_t)wParam;
			_mieayam_window_handle.keyboard[_mieayam_current_active_index].state = MIEAYAM_KEYBOARD_RELEASED;
			_mieayam_window_handle.keyboard[_mieayam_current_active_index].keyCode[keyCode] = false;
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

		case WM_MOUSEMOVE:
		{
			if (wParam & MK_LBUTTON)
			{
				const POINTS point = MAKEPOINTS(lParam);
				_mieayam_window_handle.mouses[_mieayam_current_active_index].state = MIEAYAM_MOUSE_CLICKED;
				_mieayam_window_handle.mouses[_mieayam_current_active_index].x = (int32_t)point.x;
				_mieayam_window_handle.mouses[_mieayam_current_active_index].y = (int32_t)point.y;
			}
		}
		break;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
