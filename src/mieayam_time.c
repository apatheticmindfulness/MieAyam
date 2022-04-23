#include <stdio.h>
#include <stdlib.h>
#include "mieayam_win.h"
#include "mieayam_time.h"

static float			_mieayam_time;
static LARGE_INTEGER	_mieayam_elapsed_time;
static LARGE_INTEGER	_mieayam_frequency;
static LARGE_INTEGER	_mieayam_start_time;
static LARGE_INTEGER	_mieayam_end_time;

void MieAyam_TimeInit(void)
{
	QueryPerformanceFrequency(&_mieayam_frequency);
	QueryPerformanceCounter(&_mieayam_start_time);
}

void MieAyam_TimeCount(void)
{
	QueryPerformanceCounter(&_mieayam_end_time);
	_mieayam_elapsed_time.QuadPart = _mieayam_end_time.QuadPart - _mieayam_start_time.QuadPart;

	_mieayam_elapsed_time.QuadPart *= 1000000;
	_mieayam_elapsed_time.QuadPart /= _mieayam_frequency.QuadPart;

	_mieayam_time = (float)_mieayam_elapsed_time.QuadPart;

	_mieayam_start_time = _mieayam_end_time;

}

float MieAyam_GetTimeInMicroseconds(void)
{
	return _mieayam_time;
}

float MieAyam_GetTimeInMiliseconds(void)
{
	return _mieayam_time / 1000.0f;
}

float MieAyam_GetTimeInSeconds(void)
{
	return MieAyam_GetTimeInMiliseconds() / 1000.0f;
}
