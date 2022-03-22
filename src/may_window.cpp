#include "may_graphics.h"

enum MAY_MOUSE_STATE
{
	MAY_MOUSE_CLICKED,
	MAY_MOUSE_RELEASED,
	MAY_MOUSE_NONE
};

struct MAY_Mouse
{
	MAY_MOUSE_STATE state = MAY_MOUSE_STATE::MAY_MOUSE_NONE;
	int32_t x;
	int32_t y;
};

enum MAY_KEYBOARD_STATE
{
	MAY_KEYBOARD_PRESSED,
	MAY_KEYBOARD_RELEASED,
	MAY_KEYBOARD_NONE
};

struct MAY_Keyboard
{
	MAY_KEYBOARD_STATE state = MAY_KEYBOARD_STATE::MAY_KEYBOARD_PRESSED;
	uint8_t autoRepeat = false;
	uint8_t keyCode[256];
};

struct MAY_Window
{
	uint32_t width;
	uint32_t height;
	const char * title;

	// Win32
	HWND window_handle;

	// Events
	MAY_Keyboard	keyboard;
	MAY_Mouse mouse;
};

// Variables
static WNDCLASSEX _MAY_WindowClass;

// Functions
static LRESULT CALLBACK _MAY_Win32HandleMessageState(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK _MAY_Win32HandleProcess(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static inline MAY_Window * _MAY_GetWindowState(HWND hwnd);

// Events
static void _MAY_SendMouseLeftClickedState(MAY_Window * window, int32_t x, int32_t y);
static void _MAY_SendMouseLeftReleased(MAY_Window * window, int32_t x, int32_t y);

static void _MAY_SendKeyboardPressedState(MAY_Window * window, uint32_t keyCode);
static void _MAY_SendKeyboardReleasedState(MAY_Window * window, uint32_t keyCode);


uint8_t MAY_Init()
{
	_MAY_WindowClass.cbSize = sizeof(WNDCLASSEX);
	_MAY_WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	_MAY_WindowClass.lpfnWndProc = _MAY_Win32HandleMessageState;
	_MAY_WindowClass.hInstance = GetModuleHandle(0);
	_MAY_WindowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	_MAY_WindowClass.lpszMenuName = nullptr;
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
	if (window)
	{
		window->width = width;
		window->height = height;
		window->title = title;

		// Set all the keyboard's keys to 0
		memset(window->keyboard.keyCode, 0, sizeof(window->keyboard.keyCode));

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
			nullptr,
			nullptr,
			GetModuleHandle(0),
			window);

		// Set window handle
		window->window_handle = windowHandle;

		if (!window->window_handle)
		{
			return 0;
		}

		return window;
	}

	return 0;
}

void * MAY_GetWindowHandle(const MAY_Window * const window)
{
	return window->window_handle;
}

uint8_t MAY_ShowWindow(const MAY_Window * const window)
{
	uint8_t showWindowCheck = ShowWindow(window->window_handle, SW_SHOW);
	if (!showWindowCheck)
	{
		return false;
	}

	return true;
}

uint32_t MAY_GetWindowWidth(const MAY_Window * const window)
{
	return window->width;
}

uint32_t MAY_GetWindowHeight(const MAY_Window * const window)
{
	return window->height;
}

static LRESULT CALLBACK _MAY_Win32HandleMessageState(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_CREATE)
	{
		CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		MAY_Window * pWindowState = reinterpret_cast<MAY_Window*>(pCreate->lpCreateParams);

		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindowState)); // Store the pWindowState pointer in the instance data for the window
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&_MAY_Win32HandleProcess));
		_MAY_Win32HandleProcess(hwnd, uMsg, wParam, lParam);
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

static LRESULT CALLBACK _MAY_Win32HandleProcess(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
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

		// Keyboard input
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			uint32_t keyCode = (uint32_t)wParam;
			uint32_t keyIsDown = (uint32_t)(lParam & (1 << 30));
			if (keyIsDown == 0u) // No auto repeat
			{
				_MAY_SendKeyboardPressedState(pWindowState, keyCode);
			}
		}
		break;

		case WM_SYSKEYUP:
		case WM_KEYUP:
		{
			uint32_t keyCode = (uint32_t)wParam;
			_MAY_SendKeyboardReleasedState(pWindowState, keyCode);
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

		case WM_MOUSEMOVE:
		{
			if (wParam & MK_LBUTTON)
			{
				const POINTS point = MAKEPOINTS(lParam);
				_MAY_SendMouseLeftClickedState(pWindowState, point.x, point.y);
			}
		}
		break;

	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

static inline MAY_Window * _MAY_GetWindowState(HWND hwnd)
{
	LONG_PTR pWindowData = GetWindowLongPtr(hwnd, GWLP_USERDATA);
	MAY_Window * pWindow = reinterpret_cast<MAY_Window*>(pWindowData);
	return pWindow;
}

static void _MAY_SendMouseLeftClickedState(MAY_Window * window, int32_t x, int32_t y)
{
	window->mouse.state = MAY_MOUSE_STATE::MAY_MOUSE_CLICKED;
	window->mouse.x = x;
	window->mouse.y = y;

}

static void _MAY_SendMouseLeftReleased(MAY_Window * window, int32_t x, int32_t y)
{
	window->mouse.state = MAY_MOUSE_STATE::MAY_MOUSE_RELEASED;
	window->mouse.x = x;
	window->mouse.y = y;
}

static void _MAY_SendKeyboardPressedState(MAY_Window * window, uint32_t keyCode)
{
	window->keyboard.state = MAY_KEYBOARD_STATE::MAY_KEYBOARD_PRESSED;
	window->keyboard.keyCode[keyCode] = true;
}

static void _MAY_SendKeyboardReleasedState(MAY_Window * window, uint32_t keyCode)
{
	window->keyboard.state = MAY_KEYBOARD_STATE::MAY_KEYBOARD_RELEASED;
	window->keyboard.keyCode[keyCode] = false;
}

uint8_t MAY_ProcessMessage()
{
	MSG msg = { };
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
		{
			return 0;
		}

		if (GetAsyncKeyState(VK_ESCAPE))
		{
			return 0;
		}
	}

	return true;
}

uint8_t MAY_MouseLeftIsPressed(const MAY_Window * const window)
{
	return window->mouse.state == MAY_MOUSE_STATE::MAY_MOUSE_CLICKED;
}

uint8_t MAY_MouseLeftIsReleased(const MAY_Window * const window)
{
	if (window->mouse.state == MAY_MOUSE_STATE::MAY_MOUSE_RELEASED)
	{
		return true;
	}

	return false;
}

int32_t MAY_GetMouseX(const MAY_Window * const window)
{
	return window->mouse.x;
}

int32_t MAY_GetMouseY(const MAY_Window * const window)
{
	return window->mouse.y;
}

void MAY_FreeWindow(MAY_Window * window)
{
	if (window)
	{
		free(window);
		window = nullptr;
	}
}

uint8_t MAY_KeyIsPressed(const MAY_Window * const window, uint32_t keyCode)
{
	return window->keyboard.keyCode[keyCode];
}

uint8_t MAY_KeyIsReleased(const MAY_Window * const window, uint32_t keyCode)
{
	if (window->keyboard.state == MAY_KEYBOARD_STATE::MAY_KEYBOARD_RELEASED)
	{
		return window->keyboard.keyCode[keyCode] == false; // return true
	}

	return false;
}