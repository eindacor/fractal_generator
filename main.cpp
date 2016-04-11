#include "header.h"
#include "matrix_creator.h"
#include "fractal_generator.h"

void saveTGA(int width, int height)
{
	char cFileName[64];
	FILE *fScreenshot;
	int nSize = width * height * 3;


	GLubyte *pixels = new GLubyte[nSize];
	if (pixels == NULL) return;

	int nShot = 0;

	while (nShot < 64)
	{
		sprintf(cFileName, "screenshot_%d.tga", nShot);
		fScreenshot = fopen(cFileName, "rb");
		if (fScreenshot == NULL) break;
		else fclose(fScreenshot);

		++nShot;

		if (nShot > 63)
		{
			cout << "Screenshot limit of 64 reached. Remove some shots if you want to take more." << endl;
			return;
		}
	}

	fScreenshot = fopen(cFileName, "wb");

	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	//convert to BGR format    
	unsigned char temp;
	int i = 0;
	while (i < nSize)
	{
		temp = pixels[i];       //grab blue
		pixels[i] = pixels[i + 2];//assign red to blue
		pixels[i + 2] = temp;     //assign blue to red
		i += 3;     //skip to next blue byte
	}

	unsigned char TGAheader[12] = { 0,0,2,0,0,0,0,0,0,0,0,0 };
	unsigned char header[6] = { width % 256,width / 256, height % 256,height / 256,24,0 };

	fwrite(TGAheader, sizeof(unsigned char), 12, fScreenshot);
	fwrite(header, sizeof(unsigned char), 6, fScreenshot);
	fwrite(pixels, sizeof(GLubyte), nSize, fScreenshot);
	fclose(fScreenshot);

	delete[] pixels;

	return;
}

int main()
{
	jep::init();
	//string data_path = "c:\\Users\\jpollack\\documents\\github\\fractal_generator\\";
	string data_path = "j:\\Github\\fractal_generator\\";
	string vert_file = data_path + "VertexShader.glsl";
	string frag_file = data_path + "PixelShader.glsl";

	float eye_level = 0.0f;
	shared_ptr<ogl_context> context(new ogl_context("Fractal Generator", vert_file.c_str(), frag_file.c_str(), 4000, 2000));
	shared_ptr<key_handler> keys(new key_handler(context));
	shared_ptr<texture_handler> textures(new texture_handler(data_path));
	shared_ptr<ogl_camera_free> camera(new ogl_camera_free(keys, context, vec3(0.0f, eye_level, 1.0f), 45.0f));

	/*
	SEEDS
	f6ujfV4rTtvN991MBr5gOCiaQ6TrAPaJ
	3LE1GoVEb1W9jV1GwrTyfMLVMYs8ipYT
	*/

	bool two_dimensional = false;
	//fractal_generator fg("mbaEQ8JUpjuk22uY3sBN46S9h9TX1K4k", context, two_dimensional);
	fractal_generator fg(context, two_dimensional);

	vector<vec4> point_sequence = {
		vec4(-1.0f, -1.0f, 0.0f, 1.0f),
		vec4(-1.0f, 1.0f, 0.0f, 1.0f),
		vec4(-1.0f, 1.0f, 0.0f, 1.0f),
		vec4(1.0f, 1.0f, 0.0f, 1.0f),
		vec4(1.0f, 1.0f, 0.0f, 1.0f),
		vec4(1.0f, -1.0f, 0.0f, 1.0f),
		vec4(1.0f, -1.0f, 0.0f, 1.0f),
		vec4(-1.0f, -1.0f, 0.0f, 1.0f)
	};

	int num_points = 50000;

	//fg.generateFractal(point_sequence, num_points);
	//fg.generateFractal(num_points, 5);
	fg.generateFractal(num_points);

	glfwSetTime(0);
	float render_fps = 60.0f;
	bool finished = false;
	clock_t start = clock();
	unsigned int frame_counter = 0;
	unsigned int counter_increment = 1;
	bool show_animation = true;
	bool smooth_lines = true;
	bool paused = false;
	bool reverse = false;

	while (!finished)
	{
		if (glfwGetTime() > 1.0f / render_fps)
		{
			if ((clock() - start) / CLOCKS_PER_SEC > 2.0f)
				start = clock();

			glfwPollEvents();
			context->clearBuffers();

			camera->updateCamera();
			camera->setMVP(context, mat4(1.0f), jep::NORMAL);

			glUniform1i(context->getShaderGLint("enable_growth_animation"), show_animation ? 1 : 0);
			glUniform1i(context->getShaderGLint("frame_count"), frame_counter);

			if (!paused && show_animation)
			{
				if (reverse && frame_counter > counter_increment)
					frame_counter -= counter_increment;

				else if (!reverse)
					frame_counter += counter_increment;
			}

			fg.drawFractal();
			fg.checkKeys(keys);

			//TODO see why this only works when include_hold is enabled
			if (keys->checkPress(GLFW_KEY_ESCAPE))
				finished = true;

			if (keys->checkPress(GLFW_KEY_J, false))
				show_animation = !show_animation;

			if (keys->checkPress(GLFW_KEY_G, false))
			{
				if (counter_increment != 100)
					counter_increment *= 10;
			}

			if (keys->checkPress(GLFW_KEY_F, false))
			{
				if (counter_increment != 1)
					counter_increment /= 10;
			}

			if (keys->checkPress(GLFW_KEY_H, false))
				reverse = !reverse;

			if (keys->checkPress(GLFW_KEY_R, false))
			{
				frame_counter = 0;
				show_animation = true;
				paused = false;
			}

			if (keys->checkPress(GLFW_KEY_T, false))
				paused = !paused;

			if (keys->checkPress(GLFW_KEY_X, false))
			{
				saveTGA(context->getWindowWidth(), context->getWindowHeight());
			}

			context->swapBuffers();
			glfwSetTime(0.0f);
		}
	}

	return 0;
}