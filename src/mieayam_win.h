#pragma once
#ifdef _WIN64
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
	#include <windows.h>
	#define main() WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
#else
#error MIE AYAM only support Windows with 64 bit architecture
#endif