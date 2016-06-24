#include "header.h"
#include "fractal_generator.h"

string paddedValue(unsigned int value, unsigned short total_digits)
{
	string padded_number;

	for (int i = 0; i < total_digits; i++)
	{
		padded_number += std::to_string(value % 10);
		value /= 10;
	}

	std::reverse(padded_number.begin(), padded_number.end());
	return padded_number;
}

// Save the bitmap to a bmp file  
void SaveBitmapToFile(BYTE* pBitmapBits,
	LONG lWidth,
	LONG lHeight,
	WORD wBitsPerPixel,
	LPCTSTR lpszFileName)
{
	// Some basic bitmap parameters  
	unsigned long headers_size = sizeof(BITMAPFILEHEADER) +
		sizeof(BITMAPINFOHEADER);

	unsigned long pixel_data_size = lHeight * ((lWidth * (wBitsPerPixel / 8)));

	BITMAPINFOHEADER bmpInfoHeader = { 0 };

	// Set the size  
	bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);

	// Bit count  
	bmpInfoHeader.biBitCount = wBitsPerPixel;

	// Use all colors  
	bmpInfoHeader.biClrImportant = 0;

	// Use as many colors according to bits per pixel  
	bmpInfoHeader.biClrUsed = 0;

	// Store as un Compressed  
	bmpInfoHeader.biCompression = BI_RGB;

	// Set the height in pixels  
	bmpInfoHeader.biHeight = lHeight;

	// Width of the Image in pixels  
	bmpInfoHeader.biWidth = lWidth;

	// Default number of planes  
	bmpInfoHeader.biPlanes = 1;

	// Calculate the image size in bytes  
	bmpInfoHeader.biSizeImage = pixel_data_size;

	BITMAPFILEHEADER bfh = { 0 };

	// This value should be values of BM letters i.e 0x4D42  
	// 0x4D = M 0×42 = B storing in reverse order to match with endian  
	bfh.bfType = 0x4D42;
	//bfh.bfType = 'B'+('M' << 8); 

	// <<8 used to shift ‘M’ to end  */  

	// Offset to the RGBQUAD  
	bfh.bfOffBits = headers_size;

	// Total size of image including size of headers  
	bfh.bfSize = headers_size + pixel_data_size;

	// Create the file in disk to write  
	HANDLE hFile = CreateFile(lpszFileName,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	// Return if error opening file  
	if (!hFile) return;

	DWORD dwWritten = 0;

	// Write the File header  
	WriteFile(hFile,
		&bfh,
		sizeof(bfh),
		&dwWritten,
		NULL);

	// Write the bitmap info header  
	WriteFile(hFile,
		&bmpInfoHeader,
		sizeof(bmpInfoHeader),
		&dwWritten,
		NULL);

	// Write the RGB Data  
	WriteFile(hFile,
		pBitmapBits,
		bmpInfoHeader.biSizeImage,
		&dwWritten,
		NULL);

	// Close the file handle  
	CloseHandle(hFile);
}

bool saveImage(float image_scale, const fractal_generator &fg, const shared_ptr<ogl_context> &context)
{
	vec4 background_color = context->getBackgroundColor();

	GLsizei width(context->getWindowWidth() * image_scale);
	GLsizei height(context->getWindowHeight() * image_scale);

	//RGBA8 2D texture, 24 bit depth texture
	GLuint color_tex, fb, depth_rb;
	glGenTextures(1, &color_tex);
	glBindTexture(GL_TEXTURE_2D, color_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//NULL means reserve texture memory, but texels are undefined
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
	//-------------------------
	glGenFramebuffersEXT(1, &fb);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
	//Attach 2D texture to this FBO
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, color_tex, 0);
	//-------------------------
	glGenRenderbuffersEXT(1, &depth_rb);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_rb);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, width, height);
	//-------------------------
	//Attach depth buffer to FBO
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_rb);
	//-------------------------
	//Does the GPU support current FBO configuration?
	GLenum status;
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch (status)
	{
	case GL_FRAMEBUFFER_COMPLETE_EXT: break;
	default: cout << "error" << endl; break;
	}
	//-------------------------
	//and now you can render to GL_TEXTURE_2D
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
	//glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//-------------------------
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, (GLdouble)width, 0.0, (GLdouble)height, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//-------------------------
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	glUniform1f(context->getShaderGLint("point_size_scale"), image_scale);
	glLineWidth(image_scale * 2.0f * fg.getLineWidth());
	fg.drawFractal();
	glLineWidth(fg.getLineWidth());
	glUniform1f(context->getShaderGLint("point_size_scale"), 1.0f);

	string output_filename;
	FILE *file_check;

	int nShot = 0;
	while (nShot < 64)
	{
		output_filename = fg.getSeed() + "_" + paddedValue(nShot, 3) + ".bmp";
		file_check = fopen(output_filename.c_str(), "rb");
		if (file_check == NULL) break;
		else fclose(file_check);

		++nShot;

		if (nShot > 100)
		{
			cout << "Screenshot limit of 100 reached. Remove some shots if you want to take more." << endl;
			return false;
		}
	}

	GLubyte *texture_pixels = new GLubyte[width * height * 4];
	if (texture_pixels == NULL)
		return false;

	GLubyte *output_pixels = new GLubyte[width * height * 3];
	if (output_pixels == NULL)
		return false;

	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, texture_pixels);

	//Bind 0, which means render to back buffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	//convert to BGR format    
	for (int i = 0; i < width * height; i++)
	{
		int texture_index = i * 4;
		int output_index = i * 3;

		vec4 texture_color;

		texture_color.r = (float)texture_pixels[texture_index] / 256.0f;
		texture_color.g = (float)texture_pixels[texture_index + 1] / 256.0f;
		texture_color.b = (float)texture_pixels[texture_index + 2] / 256.0f;
		texture_color.a = (float)texture_pixels[texture_index + 3] / 256.0f;

		float texture_alpha = texture_color.a;
		float modified_alpha = texture_alpha + ((1.0f - texture_alpha) * 0.9f);

		vec4 final_color = (texture_color * modified_alpha) + (background_color * (1.0f - modified_alpha));

		output_pixels[output_index] = GLubyte(final_color.b * 256.0f);
		output_pixels[output_index + 1] = GLubyte(final_color.g * 256.0f);
		output_pixels[output_index + 2] = GLubyte(final_color.r * 256.0f);
	}

	std::wstring bmp_filename;
	bmp_filename.assign(output_filename.begin(), output_filename.end());
	LPCWSTR filename_pointer = bmp_filename.c_str();

	SaveBitmapToFile(
		(BYTE*)output_pixels,
		width,
		height,
		24,
		filename_pointer);

	delete[] output_pixels;
	delete[] texture_pixels;

	//Delete resources
	glDeleteTextures(1, &color_tex);
	glDeleteRenderbuffersEXT(1, &depth_rb);
	//Bind 0, which means render to back buffer, as a result, fb is unbound
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glDeleteFramebuffersEXT(1, &fb);

	context->setBackgroundColor(background_color);

	glViewport(0, 0, context->getWindowWidth(), context->getWindowHeight());

	cout << "file saved: " << output_filename << endl;
	return true;
}