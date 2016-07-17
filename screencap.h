#pragma once

#include "header.h"

#using <system.drawing.dll>
using namespace System;
using namespace System::Drawing;
using namespace System::Drawing::Imaging;

bool saveImage(
	float image_scale, 
	const fractal_generator &fg, 
	const shared_ptr<ogl_context> &context);

string paddedValue(unsigned int value, unsigned short total_digits);
GLint glExtCheckFramebufferStatus(char *errorMessage);