#pragma once
#include <d3d11.h>
#include <d2d1_2.h>
#include <dxgi1_2.h>
#include <stdint.h>
#include "may_window.h"
#include "may_color.h"

typedef struct MAY_Graphics MAY_Graphics;

#if __cplusplus
extern "C" {
#endif

	MAY_Graphics		* MAY_CreateGraphics(MAY_Window * window);
	void				MAY_RenderEnd(MAY_Graphics * graphics);
	void				MAY_RenderStart(MAY_Graphics * graphics);
	void				MAY_FreeGraphics(MAY_Graphics * graphics);

#if __cplusplus
}
#endif