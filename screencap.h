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
	int multisample_count,
	shared_ptr<ogl_camera_flying> &camera);

bool batchRender(
	float image_scale, 
	fractal_generator &fg, 
	const shared_ptr<ogl_context> &context, 
	image_extension ie, 
	int multisample_count, 
	int x_count, 
	int y_count,
	int quadrant_size,
	bool mix_background,
	shared_ptr<ogl_camera_flying> &camera);

string paddedValue(unsigned int value, unsigned short total_digits);
GLint glExtCheckFramebufferStatus(char *errorMessage);