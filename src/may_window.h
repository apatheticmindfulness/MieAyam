#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "may_win.h"

typedef struct MAY_Window
{
	int32_t		width;
	int32_t		height;
	int8_t		isValid;
	void *		pHandle;
	const char * pTitle;
} MAY_Window;

// Functions
uint8_t			MAY_Init();
uint8_t			MAY_ShowWindow(MAY_Window window);
uint8_t			MAY_ProcessMessage();
MAY_Window		MAY_CreateWindow(uint32_t width, uint32_t height, const char * title);

//HWND MA_GetWindowHandle(const MA_Window * const window);
//constexpr uint32_t MA_GetWindowWidth(const MA_Window * const window);
//constexpr uint32_t MA_GetWindowHeight(const MA_Window * const window);
//
//int32_t MA_GetMouseX(const MA_Window * const window);
//int32_t MA_GetMouseY(const MA_Window * const window);
//uint8_t MA_KeyIsPressed(const MA_Window * const window, uint32_t keyCode);
//uint8_t MA_KeyIsReleased(const MA_Window * const window, uint32_t keyCode);