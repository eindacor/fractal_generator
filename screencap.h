#pragma once

#include "header.h"
#include "fractal_generator.h"

#using <system.drawing.dll>
using namespace System;
using namespace System::Drawing;
using namespace System::Drawing::Imaging;

bool saveImage(
	float image_scale,
	const fractal_generator &fg,
	const shared_ptr<ogl_context> &context, 
	image_extension ie, 
	int multisample_count);

string paddedValue(unsigned int value, unsigned short total_digits);
GLint glExtCheckFramebufferStatus(char *errorMessage);