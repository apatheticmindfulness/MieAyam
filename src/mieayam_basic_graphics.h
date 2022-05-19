#pragma once

// mieayam_basic_graphics used GDI for it's backend
// this utility only has one function for drawing which is MieAyam_SetPixel()

#include <stdint.h>
#include <stdbool.h>
#include "mieayam_win.h"
#include "mieayam_window.h"
#include "mieayam_color.h"

typedef struct
{
	int32_t width;
	int32_t height;
	int32_t windowIndex;
	int32_t bitCount;
	uint8_t keepAspectRatio;
} mieayam_basic_graphics_attributes;


#ifdef __cplusplus
extern "C" {
#endif

uint8_t MieAyam_InitBasicGraphics(const mieayam_basic_graphics_attributes * const graphics, int32_t count);
void MieAyam_RenderStart(int32_t graphics_index);
void MieAyam_SetPixel(int32_t x, int32_t y, mieayam_color color);
void MieAyam_RenderEnd(void);

void MieAyam_CleanBackground(mieayam_color color);

#ifdef __cplusplus
}
#endif