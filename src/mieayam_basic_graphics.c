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
	uint8_t			keepAspectRatio;
} mieayam_basic_graphics_internal;

// Variables
static int32_t							_mieayam_graphics_count;				// Store how many graphics are
static int32_t							_mieayam_graphics_current_index;		// Keep track the current active window for the graphics
static mieayam_basic_graphics_internal	_mieayam_graphics[MAX_WINDOW_COUNT];	// Store all the graphics handles

static mieayam_rect4f					_mieayam_graphics_rect;					// Store the current graphics/canvas size and position

uint8_t MieAyam_InitBasicGraphics(const mieayam_basic_graphics_attributes * const graphics, int32_t count)
{
	_mieayam_graphics_count = count;

	for (int32_t i = 0; i < _mieayam_graphics_count; i++)
	{
		const int32_t canvasWidth = graphics[i].width;
		const int32_t canvasHeight = graphics[i].height;
		const int32_t totalCanvasSize = canvasWidth * canvasHeight * sizeof(mieayam_color);

		// Configure BitmapInfo
		_mieayam_graphics[i].bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		_mieayam_graphics[i].bitmapInfo.bmiHeader.biWidth = graphics[i].width;
		_mieayam_graphics[i].bitmapInfo.bmiHeader.biHeight = -graphics[i].height;
		_mieayam_graphics[i].bitmapInfo.bmiHeader.biPlanes = 1;
		_mieayam_graphics[i].bitmapInfo.bmiHeader.biBitCount = graphics[i].bitCount;
		_mieayam_graphics[i].bitmapInfo.bmiHeader.biCompression = BI_RGB;

		// Tell the canvas to keep it's aspect ratio
		const uint8_t keepAspectRatio = graphics[i].keepAspectRatio;
		if (keepAspectRatio)
		{
			_mieayam_graphics[i].keepAspectRatio = keepAspectRatio;
		}

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

#define GET_CANVAS_WIDTH_INTERNAL _mieayam_graphics[_mieayam_graphics_current_index].bitmapInfo.bmiHeader.biWidth
#define GET_CANVAS_HEIGHT_INTERNAL abs(_mieayam_graphics[_mieayam_graphics_current_index].bitmapInfo.bmiHeader.biHeight)
#define GET_CANVAS_RESOLUTION_INTERNAL GET_CANVAS_WIDTH_INTERNAL * GET_CANVAS_HEIGHT_INTERNAL

void MieAyam_RenderStart(int32_t graphics_index)
{
	// Get the current active graphics state
	_mieayam_graphics_current_index = graphics_index;

	// From that active graphics state apply the memset function (for clearing the screen)
	const int32_t totalSize = abs(_mieayam_graphics[_mieayam_graphics_current_index].bitmapInfo.bmiHeader.biWidth * _mieayam_graphics[_mieayam_graphics_current_index].bitmapInfo.bmiHeader.biHeight * sizeof(mieayam_color));
	memset(_mieayam_graphics[_mieayam_graphics_current_index].memory, 0, totalSize);
}

void MieAyam_SetPixel(int32_t x, int32_t y, mieayam_color color)
{
	const int32_t xPos = x;
	const int32_t yPos = y;
	const mieayam_color pixelColor = color;

	const int32_t canvasWidth = GET_CANVAS_WIDTH_INTERNAL;
	const int32_t canvasHeight = GET_CANVAS_HEIGHT_INTERNAL;

	assert(xPos >= 0);
	assert(xPos < canvasWidth);
	assert(yPos >= 0);
	assert(yPos < canvasHeight);
	_mieayam_graphics[_mieayam_graphics_current_index].memory[canvasWidth * yPos + xPos] = pixelColor;
}

#define centeredCanvasX _mieayam_graphics_rect.left
#define centeredCanvasY _mieayam_graphics_rect.top
#define targetCanvasWidth _mieayam_graphics_rect.right
#define targetCanvasHeight _mieayam_graphics_rect.bottom

void MieAyam_RenderEnd(void)
{
	// Get the width and the height from the active graphics state
	const int32_t canvasWidth = GET_CANVAS_WIDTH_INTERNAL;
	const int32_t canvasHeight = GET_CANVAS_HEIGHT_INTERNAL;
	const float canvasAspectRatio = (float)canvasWidth / (float)canvasHeight;

	// Get the width and the height of the active window
	RECT windowRect = { 0 };
	GetClientRect(_mieayam_graphics[_mieayam_graphics_current_index].window, &windowRect);
	const int32_t windowWidth = windowRect.right - windowRect.left;
	const int32_t windowHeight = windowRect.bottom - windowRect.top;
	const float windowAspectRatio = (float)windowWidth / (float)windowHeight;

	centeredCanvasX = 0.0f;
	centeredCanvasY = 0.0f;
	targetCanvasWidth = 0.0f;
	targetCanvasHeight = 0.0f;

	if (windowWidth > 0 && windowHeight > 0 && canvasWidth > 0 && canvasHeight > 0)
	{
		// Keep the aspect ratio
		if (_mieayam_graphics[_mieayam_graphics_current_index].keepAspectRatio)
		{
			targetCanvasWidth = (float)windowHeight * canvasAspectRatio;
			targetCanvasHeight = (float)windowWidth / canvasAspectRatio;
			if (targetCanvasWidth > (float)windowWidth)
			{
				targetCanvasWidth = (float)windowWidth;
				centeredCanvasY = ((float)windowHeight / 2.0f) - (targetCanvasHeight / 2.0f);
			}
			else if (targetCanvasHeight > (float)windowHeight)
			{
				targetCanvasHeight = (float)windowHeight;
				centeredCanvasX = ((float)windowWidth / 2.0f) - (targetCanvasWidth / 2.0f);
			}
		}
		else
		{
			targetCanvasWidth = (float)windowWidth;
			targetCanvasHeight = (float)windowHeight;
		}
	}

	// Render
	HDC dc = GetDC(_mieayam_graphics[_mieayam_graphics_current_index].window);
	StretchDIBits(dc, 
		(int32_t)centeredCanvasX, (int32_t)centeredCanvasY, (int32_t)targetCanvasWidth, (int32_t)targetCanvasHeight,
		0, 0, canvasWidth, canvasHeight, 
		_mieayam_graphics[_mieayam_graphics_current_index].memory, 
		&_mieayam_graphics[_mieayam_graphics_current_index].bitmapInfo, 
		DIB_RGB_COLORS, SRCCOPY);
	ReleaseDC(_mieayam_graphics[_mieayam_graphics_current_index].window, dc);
}

void MieAyam_CleanBackground(const mieayam_color color)
{
	const mieayam_color clearBackgroundColor = color;
	const int32_t totalCanvasSize = GET_CANVAS_RESOLUTION_INTERNAL;

	for (int32_t x = 0; x < totalCanvasSize; x++)
	{
		memcpy(_mieayam_graphics[_mieayam_graphics_current_index].memory + x, &clearBackgroundColor, sizeof(mieayam_color));
	}
}

mieayam_rect4f MieAyam_GetActiveGraphicsSize(void)
{
	return _mieayam_graphics_rect;
}
