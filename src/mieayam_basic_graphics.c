#include "mieayam_basic_graphics.h"
#include "mieayam_config.h"
#include "mieayam_window.h"
#include <math.h>
#include <assert.h>
#include <malloc.h>

typedef struct
{
	HWND			window;
	BITMAPINFO		bitmapInfo;
	mieayam_color	* memory;
} mieayam_basic_graphics_internal;

// Variables
static int32_t							_mieayam_graphics_count;				// Store how many graphics are
static int32_t							_mieayam_graphics_current_index;		// Keep track the current active window for the graphics
static mieayam_basic_graphics_internal	_mieayam_graphics[MAX_WINDOW_COUNT];	// Store all the graphics handles	

// Functions
static int32_t							_MieAyam_GetCanvasHeightInternal();
static int32_t							_MieAyam_GetTotalCanvasSizeInternal();
static int32_t							_MieAyam_GetCanvasWidthInternal();

uint8_t MieAyam_InitBasicGraphics(const mieayam_basic_graphics_attributes * const graphics, int32_t count)
{
	_mieayam_graphics_count = count;

	for (int32_t i = 0; i < _mieayam_graphics_count; i++)
	{
		int32_t canvasWidth = graphics[i].width;
		int32_t canvasHeight = graphics[i].height;
		int32_t totalCanvasSize = canvasWidth * canvasHeight * sizeof(mieayam_color);

		// Configure BitmapInfo
		_mieayam_graphics[i].bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		_mieayam_graphics[i].bitmapInfo.bmiHeader.biWidth = graphics[i].width;
		_mieayam_graphics[i].bitmapInfo.bmiHeader.biHeight = -graphics[i].height;
		_mieayam_graphics[i].bitmapInfo.bmiHeader.biPlanes = 1;
		_mieayam_graphics[i].bitmapInfo.bmiHeader.biBitCount = graphics[i].bitCount;
		_mieayam_graphics[i].bitmapInfo.bmiHeader.biCompression = BI_RGB;

		// Get window index
		_mieayam_graphics[i].window = MieAyam_GetWindowHandle(graphics[i].windowIndex);

		// Allocate some memory for renderer
		_mieayam_graphics[i].memory = (mieayam_color*)VirtualAlloc(0, totalCanvasSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (!_mieayam_graphics[i].memory)
		{
			// Log : Failed to allocate memory
			return false;
		}
	}

	return true;
}

void MieAyam_RenderStart(int32_t graphics_index)
{
	// Get the current active graphics state
	_mieayam_graphics_current_index = graphics_index;

	// From that active graphics state apply the memset function (for clearing the screen)
	int32_t totalSize = abs(_mieayam_graphics[_mieayam_graphics_current_index].bitmapInfo.bmiHeader.biWidth * _mieayam_graphics[_mieayam_graphics_current_index].bitmapInfo.bmiHeader.biHeight * sizeof(mieayam_color));
	memset(_mieayam_graphics[_mieayam_graphics_current_index].memory, 0, totalSize);
}

void MieAyam_SetPixel(int32_t x, int32_t y, mieayam_color color)
{
	int32_t xPos = x;
	int32_t yPos = y;
	mieayam_color pixelColor = color;

	int32_t canvasWidth = _MieAyam_GetCanvasWidthInternal();
	int32_t canvasHeight = _MieAyam_GetCanvasHeightInternal();

	assert(xPos >= 0);
	assert(xPos < canvasWidth);
	assert(yPos >= 0);
	assert(yPos < canvasHeight);
	_mieayam_graphics[_mieayam_graphics_current_index].memory[canvasWidth * yPos + xPos] = pixelColor;
}

void MieAyam_RenderEnd(void)
{
	// Get the width and the height from the active graphics state
	int32_t canvasWidth = _MieAyam_GetCanvasWidthInternal();
	int32_t canvasHeight = _MieAyam_GetCanvasHeightInternal();
	float canvasAspectRatio = (float)canvasWidth / (float)canvasHeight;

	// Get the width and the height of the active window
	RECT windowRect = { 0 };
	GetClientRect(_mieayam_graphics[_mieayam_graphics_current_index].window, &windowRect);
	int32_t windowWidth = windowRect.right - windowRect.left;
	int32_t windowHeight = windowRect.bottom - windowRect.top;
	float windowAspectRatio = (float)windowWidth / (float)windowHeight;

	float centeredCanvasX = 0.0f;
	float centeredCanvasY = 0.0f;
	float targetCanvasWidth = (float)windowHeight * canvasAspectRatio;
	float targetCanvasHeight = (float)windowWidth / canvasAspectRatio;

	if (targetCanvasWidth > (float)windowWidth)
	{
		targetCanvasWidth = (float)windowWidth;
		centeredCanvasY = ((float)windowHeight / 2.0f) - ((float)targetCanvasHeight / 2.0f);
	}
	else
	{
		targetCanvasHeight = (float)windowHeight;
		centeredCanvasX = ((float)windowWidth / 2.0f) - ((float)targetCanvasWidth / 2.0f);
	}


	/*
	if (targetCanvasWidth > (float)windowWidth) // This I don't understand, how can be that possible Ky?
	{
		targetCanvasWidth = (float)windowWidth;
		centeredCanvasY = ((float)windowHeight / 2.0f) - ((float)targetCanvasHeight / 2.0f); // centered y-axis
	}
	else
	{
		targetCanvasHeight = (float)windowHeight;
		centeredCanvasX = ((float)windowWidth / 2.0f) - ((float)targetCanvasWidth / 2.0f); // centered x-axis
	}
	*/

	#pragma region This looks works | maintaining the aspect ratio
	// The problem is I don't quite understand the code, I understand the math
	// but the logic, I don't really understand. How is the width or the height match with the aspect ratio?
#if 0
	if (canvasAspectRatio > 0 && windowWidth > 0 && windowHeight > 0)
	{
		targetCanvasWidth = (float)windowHeight * canvasAspectRatio; // This is already optimal size 16 units
		targetCanvasHeight = (float)windowWidth / canvasAspectRatio; // This is already optimal size too 9 units

		if (targetCanvasWidth > (float)windowWidth) // This I don't understand, how can be that possible Ky?
		{
			targetCanvasWidth = (float)windowWidth;
			centeredCanvasY = ((float)windowHeight / 2.0f) - ((float)targetCanvasHeight / 2.0f); // centered y-axis
		}
		else
		{
			targetCanvasHeight = (float)windowHeight;
			centeredCanvasX = ((float)windowWidth / 2.0f) - ((float)targetCanvasWidth / 2.0f); // centered x-axis
		}
	}
#endif
	#pragma endregion

	HDC dc = GetDC(_mieayam_graphics[_mieayam_graphics_current_index].window);
	StretchDIBits(dc, 
		(int32_t)centeredCanvasX, (int32_t)centeredCanvasY, (int32_t)targetCanvasWidth, (int32_t)targetCanvasHeight,
		0, 0, canvasWidth, canvasHeight, 
		_mieayam_graphics[_mieayam_graphics_current_index].memory, 
		&_mieayam_graphics[_mieayam_graphics_current_index].bitmapInfo, 
		DIB_RGB_COLORS, SRCCOPY);
	ReleaseDC(_mieayam_graphics[_mieayam_graphics_current_index].window, dc);
}

void MieAyam_CleanBackground(mieayam_color color)
{
	mieayam_color clearBackgroundColor = color;
	int32_t totalCanvasSize = _MieAyam_GetTotalCanvasSizeInternal();

	for (int32_t x = 0; x < totalCanvasSize; x++)
	{
		memcpy(_mieayam_graphics[_mieayam_graphics_current_index].memory + x, &clearBackgroundColor, sizeof(mieayam_color));
	}
}

static int32_t _MieAyam_GetCanvasWidthInternal()
{
	return _mieayam_graphics[_mieayam_graphics_current_index].bitmapInfo.bmiHeader.biWidth;
}

static int32_t _MieAyam_GetCanvasHeightInternal()
{
	return abs(_mieayam_graphics[_mieayam_graphics_current_index].bitmapInfo.bmiHeader.biHeight);;
}

static int32_t _MieAyam_GetTotalCanvasSizeInternal()
{
	return _MieAyam_GetCanvasWidthInternal() * _MieAyam_GetCanvasHeightInternal();
}
