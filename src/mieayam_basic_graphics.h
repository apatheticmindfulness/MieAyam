#pragma once
#include "mieayam_win.h"
#include "mieayam_window.h"
#include <stdint.h>
#include <stdbool.h>

#define COBJMACROS
#include <d3d11.h>
#include <dxgi.h>

uint8_t MieAyam_InitGraphics(const mieayam_window_attributes window_attributes, int32_t index);
void	MieAyam_RenderEnd();
void	MieAyam_CleanGraphics();