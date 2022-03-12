#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "may_win.h"

typedef struct MAY_Window MAY_Window;

uint8_t MAY_Init();
MAY_Window * MAY_CreateWindow(uint32_t width, uint32_t height, const char * title);
uint8_t MAY_ShowWindow(const MAY_Window * const window);
uint8_t MAY_ProcessMessage();
void MAY_FreeWindow(MAY_Window * window);

//HWND MA_GetWindowHandle(const MA_Window * const window);
//constexpr uint32_t MA_GetWindowWidth(const MA_Window * const window);
//constexpr uint32_t MA_GetWindowHeight(const MA_Window * const window);
//
//uint8_t MA_MouseLeftIsPressed(const MA_Window * const window);
//uint8_t MA_MouseLeftIsReleased(const MA_Window * const window);
//int32_t MA_GetMouseX(const MA_Window * const window);
//int32_t MA_GetMouseY(const MA_Window * const window);
//uint8_t MA_KeyIsPressed(const MA_Window * const window, uint32_t keyCode);
//uint8_t MA_KeyIsReleased(const MA_Window * const window, uint32_t keyCode);