#include "mieayam_color.h"

mieayam_color MieAyam_MakeColorFromHex(uint32_t colorCode)
{
	uint8_t red = (colorCode & 0xFF0000) >> 16;
	uint8_t green = (colorCode & 0x00FF00) >> 8;
	uint8_t blue = (colorCode & 0x0000FF);

	mieayam_color color = { blue, green, red, 0xFF };
	return color;
}
