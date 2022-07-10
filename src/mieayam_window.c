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
	mieayam_window_internal			window;
	mieayam_keyboard_internal		keyboard;
	mieayam_mouse_internal			mouses;
} mieayam_window_handles;

// Variables
static uint32_t							_mieayam_window_count;				// Store how many windows are
static uint32_t							_mieayam_window_track_window_count; // Track current the count of the windows
static int32_t							_mieayam_current_active_index;		// Store current active window index from an array
static WNDCLASSEX						_mieayam_window_class;				// Store window main class
static mieayam_window_handles			_mieayam_window_handle[MAX_WINDOW_COUNT];				// Store all the window handles				

// Functions
static LRESULT CALLBACK					_mieayam_Win32HandleProcess(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static void								_mieayam_Win32WindowClearState();

void MieAyam_Init(void)
{
	_mieayam_window_class.cbSize = sizeof(WNDCLASSEX);
	_mieayam_window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	_mieayam_window_class.lpfnWndProc = _mieayam_Win32HandleProcess;
	_mieayam_window_class.hInstance = GetModuleHandle(0);
	_mieayam_window_class.hCursor = LoadCursor(0, IDC_ARROW);
	_mieayam_window_class.lpszMenuName = 0;
	_mieayam_window_class.lpszClassName = "MieAyam_MainWindow";
	_mieayam_window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); //(HBRUSH)(COLOR_BACKGROUND + 1);

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
		const int32_t windowWidth = window_attributes[i].width;
		const int32_t windowHeight = window_attributes[i].height;
		const char * title = window_attributes[i].title;

		DWORD windowFlags = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;

		// If window was set to resizable
		if (window_attributes[i].resizable)
		{
			windowFlags |= WS_THICKFRAME | WS_MAXIMIZEBOX;
		}

		RECT rc = { 0, 0, (LONG)windowWidth, (LONG)windowHeight };
		if (!AdjustWindowRect(&rc, windowFlags, false))
		{
			// TODO : Error log
			return false;
		}

		_mieayam_window_handle[i].window = CreateWindow(
			_mieayam_window_class.lpszClassName,
			window_attributes[i].title,
			windowFlags,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			rc.right - rc.left,
			rc.bottom - rc.top,
			0,
			0,
			GetModuleHandle(0),
			0);

		if (!_mieayam_window_handle[i].window)
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
	if (ShowWindow(_mieayam_window_handle[window_index].window, SW_SHOW))
	{
		return true;
	}
	return false;
}

HWND MieAyam_GetWindowHandle(int32_t window_index)
{
	return _mieayam_window_handle[window_index].window;
}

uint8_t MieAyam_KeyboardIsPressed(uint32_t key_code)
{
	return _mieayam_window_handle[_mieayam_current_active_index].keyboard.keyCode[key_code];
}

uint8_t MieAyam_KeyboardIsReleased(uint32_t key_code)
{
	if (_mieayam_window_handle[_mieayam_current_active_index].keyboard.state == MIEAYAM_KEYBOARD_RELEASED)
	{
		return _mieayam_window_handle[_mieayam_current_active_index].keyboard.keyCode[key_code] == false; // this is return true
	}

	return false;
}

uint8_t MieAyam_MouseLeftIsPressed(void)
{
	return _mieayam_window_handle[_mieayam_current_active_index].mouses.state == MIEAYAM_MOUSE_CLICKED;
}

uint8_t MieAyam_MouseLeftIsReleased(void)
{
	if (_mieayam_window_handle[_mieayam_current_active_index].mouses.state == MIEAYAM_MOUSE_RELEASED)
	{
		return true;
	}

	return false;
}

int32_t MieAyam_GetMouseX(void)
{
	return _mieayam_window_handle[_mieayam_current_active_index].mouses.x;
}

int32_t MieAyam_GetMouseY(void)
{
	return _mieayam_window_handle[_mieayam_current_active_index].mouses.y;
}

#define KEYBOARD_STATE(STATE, STATUS) \
_mieayam_window_handle[_mieayam_current_active_index].keyboard.state = STATE; \
_mieayam_window_handle[_mieayam_current_active_index].keyboard.keyCode[keyCode] = STATUS

#define MOUSE_STATE(STATE, MOUSE_POINT) \
_mieayam_window_handle[_mieayam_current_active_index].mouses.state = STATE; \
_mieayam_window_handle[_mieayam_current_active_index].mouses.x = (int32_t)MOUSE_POINT.x; \
_mieayam_window_handle[_mieayam_current_active_index].mouses.y = (int32_t)MOUSE_POINT.y;

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
				if (activeWindow == _mieayam_window_handle[i].window)
				{
					_mieayam_current_active_index = i;
				}
			}
		}
		break;

		case WM_CLOSE:
		{
			_mieayam_window_track_window_count--;
			DestroyWindow(_mieayam_window_handle[_mieayam_current_active_index].window);
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
			_mieayam_Win32WindowClearState();
		}
		break;

		// Keyboard input
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			const uint32_t keyCode = (uint32_t)wParam;
			const uint32_t keyIsDown = (uint32_t)(lParam & (1 << 30));
			if (keyIsDown == 0u) // No auto repeat
			{
				KEYBOARD_STATE(MIEAYAM_KEYBOARD_PRESSED, true);
			}
		}
		break;

		case WM_SYSKEYUP:
		case WM_KEYUP:
		{
			const uint32_t keyCode = (uint32_t)wParam;
			KEYBOARD_STATE(MIEAYAM_KEYBOARD_RELEASED, false);
		}
		break;

		// Mouse input
		case WM_LBUTTONDOWN:
		{
			const POINTS point = MAKEPOINTS(lParam);
			MOUSE_STATE(MIEAYAM_MOUSE_CLICKED, point);
		}
		break;

		case WM_LBUTTONUP:
		{
			const POINTS point = MAKEPOINTS(lParam);
			MOUSE_STATE(MIEAYAM_MOUSE_RELEASED, point);
		}
		break;

		case WM_MOUSEMOVE:
		{
			if (wParam & MK_LBUTTON)
			{
				const POINTS point = MAKEPOINTS(lParam);
				MOUSE_STATE(MIEAYAM_MOUSE_CLICKED, point);
			}
		}
		break;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void _mieayam_Win32WindowClearState()
{
	for (size_t i = 0; i < (size_t)_mieayam_window_count; i++)
	{
		_mieayam_window_handle[i].keyboard.state = MIEAYAM_KEYBOARD_NONE;
		memset(_mieayam_window_handle[i].keyboard.keyCode, 0, 256);
	}
}
