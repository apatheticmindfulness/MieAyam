#pragma once

// mieayam_basic_graphics used GDI for it's backend
// this utility only has one function for drawing which is MieAyam_SetPixel()

#include <stdint.h>
#include <stdbool.h>
#include "mieayam_win.h"
#include "mieayam_color.h"

typedef struct
{
	int32_t width;
	int32_t height;
	int32_t windowIndex;
	int32_t bitCount;
} mieayam_basic_graphics_attributes;


uint8_t MieAyam_InitBasicGraphics(const mieayam_basic_graphics_attributes * const graphics, int32_t count);
void MieAyam_RenderStart(int32_t graphics_index);
void MieAyam_SetPixel(int32_t x, int32_t y, mieayam_color color);
void MieAyam_RenderEnd();