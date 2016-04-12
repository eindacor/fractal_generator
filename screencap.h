#pragma once

#ifndef SCREENCAP_H
#define SCREENCAP_H

#include "header.h"

bool saveImage(
	float image_scale, 
	const fractal_generator &fg, 
	const shared_ptr<ogl_context> &context);

void SaveBitmapToFile(
	BYTE* pBitmapBits, LONG lWidth,
	LONG lHeight,
	WORD wBitsPerPixel,
	LPCTSTR lpszFileName);

string paddedValue(unsigned int value, unsigned short total_digits);

#endif