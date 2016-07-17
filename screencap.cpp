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

	// create a framebuffer
	GLuint fbo_id;
	glGenFramebuffersEXT(1, &fbo_id);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_id);

	// create a multisample color buffer
	GLuint color_buffer;
	glGenRenderbuffersEXT(1, &color_buffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, color_buffer);
	signed int samples = 4;
	glRenderbufferStorageMultisample(GL_RENDERBUFFER_EXT, samples, GL_RGBA8, width, height);

	// create a multisample depth buffer
	GLuint depth_buffer;
	glGenRenderbuffersEXT(1, &depth_buffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_buffer);
	glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, samples, GL_DEPTH_COMPONENT24, width, height);

	// attach renderbuffers to FBO
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, color_buffer);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_buffer);

	char error_messages[256];
	GLint value = glExtCheckFramebufferStatus(error_messages);
	if (value < 0)
	{
		cout << error_messages << endl;
		return false;
	}

	// render
	glUniform1f(context->getShaderGLint("point_size_scale"), image_scale);
	fg.drawFractal();
	glUniform1f(context->getShaderGLint("point_size_scale"), 1.0f);

	// copy to multisampled fbo
	GLuint multisample_fbo_id;
	glGenFramebuffersEXT(1, &multisample_fbo_id);
	glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, multisample_fbo_id);
	glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, fbo_id);
	glBlitFramebufferEXT(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_id);

	GLubyte *pixels = new GLubyte[width * height * 4];
	if (pixels == NULL)
		return false;

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
	glDeleteRenderbuffersEXT(1, &color_buffer);
	glDeleteRenderbuffersEXT(1, &depth_buffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glDeleteFramebuffersEXT(1, &fbo_id);
	glDeleteFramebuffersEXT(1, &multisample_fbo_id);

	context->setBackgroundColor(background_color);

	glViewport(0, 0, context->getWindowWidth(), context->getWindowHeight());

	output_bitmap.Save(gcnew String(&filename[0]), ImageFormat::Jpeg);

	cout << "file saved: " << filename << endl;

	return true;
}

GLint glExtCheckFramebufferStatus(char *error_message)
{
	GLenum status;
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch (status)
	{
	case GL_FRAMEBUFFER_COMPLETE_EXT:
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
		//Choose different formats
		strcpy(error_message, "Framebuffer object format is unsupported by the video hardware. (GL_FRAMEBUFFER_UNSUPPORTED_EXT)(FBO - 820)");
		return -1;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
		strcpy(error_message, "Incomplete attachment. (GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT)(FBO - 820)");
		return -1;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
		strcpy(error_message, "Incomplete missing attachment. (GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT)(FBO - 820)");
		return -1;
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
		strcpy(error_message, "Incomplete dimensions. (GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT)(FBO - 820)");
		return -1;
	case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
		strcpy(error_message, "Incomplete formats. (GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT)(FBO - 820)");
		return -1;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
		strcpy(error_message, "Incomplete draw buffer. (GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT)(FBO - 820)");
		return -1;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
		strcpy(error_message, "Incomplete read buffer. (GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT)(FBO - 820)");
		return -1;
	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT:
		strcpy(error_message, "Incomplete multisample buffer. (GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT)(FBO - 820)");
		return -1;
	default:
		//Programming error; will fail on all hardware
		strcpy(error_message, "Some video driver error or programming error occured. Framebuffer object status is invalid. (FBO - 823)");
		return -2;
	}
	return 1;
}