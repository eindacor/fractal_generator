#include "screencap.h"
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

bool saveImage(float image_scale, const fractal_generator &fg, const shared_ptr<ogl_context> &context)
{
	cout << "rendering image..." << endl;
	vec4 background_color = context->getBackgroundColor();

	string resolution_input;
	cout << "resolution: ";
	std::getline(std::cin, resolution_input);
	cout << endl;
	int image_height = glm::clamp((resolution_input == "" || resolution_input == "\n") ? int(context->getWindowHeight() * image_scale) : std::stoi(resolution_input), 100, 5000);

	int image_width = int(float(image_height) * float(context->getAspectRatio()));

	GLsizei width(image_width);
	GLsizei height(image_height);

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

	string filename;
	FILE *file_check;

	int nShot = 0;
	while (nShot < 256)
	{
		filename = fg.getSeed() + "_g" + paddedValue(fg.getGeneration(), 3) + "_" + paddedValue(nShot, 3) + ".jpg";
		file_check = fopen(filename.c_str(), "rb");
		if (file_check == NULL) break;
		else fclose(file_check);

		++nShot;

		if (nShot == 256)
		{
			cout << "Screenshot limit of 100 reached. Remove some shots if you want to take more." << endl;
			return false;
		}
	}

	GLubyte *texture_pixels = new GLubyte[width * height * 4];
	if (texture_pixels == NULL)
		return false;

	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, texture_pixels);

	//Bind 0, which means render to back buffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	Bitmap output_bitmap(width, height);

	//convert to BGR format    
	for (int i = 0; i < width * height; i++)
	{
		int texture_index = i * 4;

		ColorTranslator ct();
		Color pixel_color;
		pixel_color = pixel_color.FromArgb(Byte(texture_pixels[texture_index + 3]), Byte(texture_pixels[texture_index]), Byte(texture_pixels[texture_index + 1]), Byte(texture_pixels[texture_index + 2]));
		output_bitmap.SetPixel(i % width, i / width, pixel_color);
	}

	delete[] texture_pixels;

	//Delete resources
	glDeleteTextures(1, &color_tex);
	glDeleteRenderbuffersEXT(1, &depth_rb);
	//Bind 0, which means render to back buffer, as a result, fb is unbound
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glDeleteFramebuffersEXT(1, &fb);

	context->setBackgroundColor(background_color);

	glViewport(0, 0, context->getWindowWidth(), context->getWindowHeight());

	output_bitmap.Save(gcnew String(&filename[0]), ImageFormat::Jpeg);

	cout << "file saved: " << filename << endl;

	return true;
}