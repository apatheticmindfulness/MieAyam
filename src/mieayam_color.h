#pragma once
#include <stdint.h>

typedef struct
{
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t a;
} mieayam_color;

#ifdef __cplusplus
extern "C" {
#endif

mieayam_color	MieAyam_MakeColorFromHex(uint32_t colorCode);

#ifdef __cplusplus
}
#endif

#define MIEAYAM_COLOR_BLACK				{ 0x00, 0x00, 0x00, 0x00 }
#define MIEAYAM_COLOR_WHITE				{ 0xFF, 0xFF, 0xFF, 0xFF }
#define MIEAYAM_COLOR_BLUE				{ 0xFF, 0x00, 0x00, 0xFF }
#define MIEAYAM_COLOR_GREEN				{ 0x00, 0xFF, 0x00, 0xFF }
#define MIEAYAM_COLOR_RED				{ 0x00, 0x00, 0xFF, 0xFF }
#define MIEAYAM_COLOR_PURPLE			{ 0x80, 0x00, 0x80, 0xFF }
#define MIEAYAM_COLOR_CORNFLOWERBLUE	{ 0xED, 0x95, 0x64, 0xFF }

