#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "may_win.h"

typedef enum MAY_MOUSE_STATE
{
	MAY_MOUSE_NONE,
	MAY_MOUSE_CLICKED,
	MAY_MOUSE_RELEASED
} MAY_MOUSE_STATE;
typedef struct MAY_Mouse MAY_Mouse;
typedef struct MAY_Window MAY_Window;

uint8_t			MAY_Init();
uint8_t			MAY_ShowWindow(MAY_Window * window);
uint8_t			MAY_ProcessMessage();
uint8_t			MAY_MouseLeftIsPressed(MAY_Mouse * mouse);
uint8_t			MAY_MouseLeftIsReleased(MAY_Mouse * mouse);
MAY_Window		* MAY_CreateWindow(uint32_t width, uint32_t height, const char * title);
void			MAY_FreeWindow(MAY_Window * window);
void			* MAY_GetWindowHandle(MAY_Window * window);
void			* MAY_GetWindowMouseHandle(MAY_Window * window);

int32_t			MAY_GetMouseX(const MAY_Window * const window);
int32_t			MAY_GetMouseY(const MAY_Window * const window);

//HWND MA_GetWindowHandle(const MA_Window * const window);
//constexpr uint32_t MA_GetWindowWidth(const MA_Window * const window);
//constexpr uint32_t MA_GetWindowHeight(const MA_Window * const window);
//
//uint8_t MA_KeyIsPressed(const MA_Window * const window, uint32_t keyCode);
//uint8_t MA_KeyIsReleased(const MA_Window * const window, uint32_t keyCode);