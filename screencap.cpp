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

	glEnable(GL_MULTISAMPLE);

	// initialize multisample texture
	GLuint multisample_tex;
	glGenTextures(1, &multisample_tex);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, multisample_tex);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//NULL means reserve texture memory, but texels are undefined
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, width, height, GL_FALSE);

	// initialize multisample fbo
	GLuint multisample_fbo;
	glGenFramebuffers(1, &multisample_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, multisample_fbo);
	// attach multisample texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, multisample_tex, 0);

	char error_messages[256];
	GLint value = glExtCheckFramebufferStatus(error_messages);
	if (value < 0)
	{
		cout << error_messages << endl;
		return false;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width, height);
	
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glOrtho(0.0, (GLdouble)width, 0.0, (GLdouble)height, -1.0, 1.0);

	// render
	glUniform1f(context->getShaderGLint("point_size_scale"), image_scale);
	fg.drawFractal();
	glUniform1f(context->getShaderGLint("point_size_scale"), 1.0f);

	// initialize downsample texture
	GLuint downsample_tex;
	glGenTextures(1, &downsample_tex);
	glBindTexture(GL_TEXTURE_2D, downsample_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//NULL means reserve texture memory, but texels are undefined
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

	// initialize downsample fbo
	GLuint downsample_fbo;
	glGenFramebuffers(1, &downsample_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, downsample_fbo);
	// attach downsample texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, downsample_tex, 0);

	value = glExtCheckFramebufferStatus(error_messages);
	if (value < 0)
	{
		cout << error_messages << endl;
		return false;
	}

	// select multisample framebuffer for reading
	glBindFramebuffer(GL_READ_FRAMEBUFFER, multisample_fbo);
	// select downsample framebuffer for drawing
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, downsample_fbo);
	
	// blit from read framebuffer to draw framebuffer
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	GLubyte *pixels = new GLubyte[width * height * 4];
	if (pixels == NULL)
		return false;

	// select downsample framebuffer for reading
	glBindFramebuffer(GL_READ_FRAMEBUFFER, downsample_fbo);

	// read pixels
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	string filename;
	FILE *file_check;

	int image_count = 0;
	while (image_count < 256)
	{
		filename = fg.getSeed() + "_g" + paddedValue(fg.getGeneration(), 3) + "_" + paddedValue(image_count, 3) + ".jpg";
		file_check = fopen(filename.c_str(), "rb");
		if (file_check == NULL) break;
		else fclose(file_check);

		++image_count;

		if (image_count == 256)
		{
			cout << "Screenshot limit of 256 reached." << endl;
			return false;
		}
	}

	// save to image class
	Bitmap output_bitmap(width, height);
	for (int i = 0; i < width * height; i++)
	{
		int texture_index = i * 4;
		ColorTranslator ct();
		Color pixel_color;
		pixel_color = pixel_color.FromArgb(Byte(pixels[texture_index + 3]), Byte(pixels[texture_index]), Byte(pixels[texture_index + 1]), Byte(pixels[texture_index + 2]));
		output_bitmap.SetPixel(i % width, height - (i / width) - 1, pixel_color);
	}

	delete[] pixels;

	// delete resources
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &multisample_fbo);

	context->setBackgroundColor(background_color);

	glViewport(0, 0, context->getWindowWidth(), context->getWindowHeight());

	output_bitmap.Save(gcnew String(&filename[0]), ImageFormat::Jpeg);

	cout << "file saved: " << filename << endl;

	return true;
}

GLint glExtCheckFramebufferStatus(char *error_message)
{
	GLenum status;
	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (status)
	{
	case GL_FRAMEBUFFER_COMPLETE:
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		//Choose different formats
		strcpy(error_message, "Framebuffer object format is unsupported by the video hardware. (GL_FRAMEBUFFER_UNSUPPORTED)(FBO - 820)");
		return -1;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		strcpy(error_message, "Incomplete attachment. (GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)(FBO - 820)");
		return -1;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		strcpy(error_message, "Incomplete missing attachment. (GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)(FBO - 820)");
		return -1;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		strcpy(error_message, "Incomplete draw buffer. (GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)(FBO - 820)");
		return -1;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		strcpy(error_message, "Incomplete read buffer. (GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)(FBO - 820)");
		return -1;
	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		strcpy(error_message, "Incomplete multisample buffer. (GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE)(FBO - 820)");
		return -1;
	default:
		//Programming error; will fail on all hardware
		strcpy(error_message, "Some video driver error or programming error occured. Framebuffer object status is invalid. (FBO - 823)");
		return -2;
	}
	return 1;
}