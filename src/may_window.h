#pragma once
#include <stdint.h>
#include "may_win.h"

typedef struct MAY_Window MAY_Window;

#ifdef  __cplusplus
extern "C" {
#endif //  __cplusplus
	uint8_t MAY_Init();
	MAY_Window * MAY_CreateWindow(uint32_t width, uint32_t height, const char * title);
	void* MAY_GetWindowHandle(const MAY_Window * const window);
	uint8_t MAY_ShowWindow(const MAY_Window * const window);
	uint32_t MAY_GetWindowWidth(const MAY_Window * const window);
	uint32_t MAY_GetWindowHeight(const MAY_Window * const window);
	uint8_t MAY_ProcessMessage();
	void MAY_FreeWindow(MAY_Window * window);

	uint8_t MAY_MouseLeftIsPressed(const MAY_Window * const window);
	uint8_t MAY_MouseLeftIsReleased(const MAY_Window * const window);
	int32_t MAY_GetMouseX(const MAY_Window * const window);
	int32_t MAY_GetMouseY(const MAY_Window * const window);
	uint8_t MAY_KeyIsPressed(const MAY_Window * const window, uint32_t keyCode);
	uint8_t MAY_KeyIsReleased(const MAY_Window * const window, uint32_t keyCode);
#ifdef __cplusplus
}
#endif // __cplusplus
