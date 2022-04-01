#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "mieayam_win.h"

typedef enum
{
	MIEAYAM_WINDOW_SHOW_ONE,
	MIEAYAM_WINDOW_SHOW_ALL
} MIEAYAM_WINDOW_SHOW;

typedef struct
{
	int32_t width;
	int32_t height;
	const char *title;
} mieayam_window_attributes;

typedef struct
{
	int32_t x;
	int32_t y;
	int32_t z;
} mieayam_point;

void		MieAyam_Init();
void		MieAyam_CreateWindow(const mieayam_window_attributes * const window_attributes, int32_t count, MIEAYAM_WINDOW_SHOW show);
void		MieAyam_ShowWindow(int32_t window_index);
int32_t		MieAyam_GetCurrentActiveWindowIndex();
uint8_t		MieAyam_RunProccess();

uint8_t		MieAyam_MouseLeftIsPressed();
uint8_t		MieAyam_MouseLeftIsReleased();
int32_t		MieAyam_GetMouseX();
int32_t		Mieayam_GetMouseY();
