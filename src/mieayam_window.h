#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "mieayam_win.h"

typedef struct
{
	int32_t width;
	int32_t height;
	const char *title;
} mieayam_window_attributes;

void		MieAyam_Init();
uint8_t		MieAyam_CreateWindow(const mieayam_window_attributes * const window_attributes, int32_t count);
int32_t		MieAyam_GetCurrentActiveWindowIndex();
int32_t		MieAyam_GetWindowCount();
uint8_t		MieAyam_RunProccess();
uint8_t		MieAyam_ShowWindow(int32_t window_index);
HWND		MieAyam_GetWindowHandle(int32_t index);

uint8_t		MieAyam_KeyboardIsPressed(uint32_t key_code);
uint8_t		MieAyam_KeyboardIsReleased(uint32_t key_code);

uint8_t		MieAyam_MouseLeftIsPressed();
uint8_t		MieAyam_MouseLeftIsReleased();
int32_t		MieAyam_GetMouseX();
int32_t		MieAyam_GetMouseY();
