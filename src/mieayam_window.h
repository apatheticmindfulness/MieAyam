#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "mieayam_win.h"

typedef struct
{
	uint32_t id;
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
void		MieAyam_CreateMainWindow(const mieayam_window_attributes * const window_attributes, int32_t count);
uint8_t		MieAyam_RunProccess();
uint8_t		MieAyam_MouseLeftIsPressed();
uint8_t		MieAyam_MouseLeftIsReleased();
