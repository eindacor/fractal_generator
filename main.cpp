#include "header.h"
#include "matrix_creator.h"
#include "fractal_generator.h"

int main()
{
	jep::init();
	//string data_path = "c:\\Users\\jpollack\\documents\\github\\fractal_generator\\";
	string data_path = "j:\\Github\\fractal_generator\\";
	string vert_file = data_path + "VertexShader.glsl";
	string frag_file = data_path + "PixelShader.glsl";

	float eye_level = 0.0f;
	shared_ptr<ogl_context> context(new ogl_context("Fractal Generator", vert_file.c_str(), frag_file.c_str(), 1920, 1080));
	shared_ptr<key_handler> keys(new key_handler(context));
	shared_ptr<texture_handler> textures(new texture_handler(data_path));
	shared_ptr<ogl_camera_free> camera(new ogl_camera_free(keys, context, vec3(0.0f, eye_level, 1.0f), 45.0f));

	matrix_creator mc;
	bool two_dimensional = false;
	fractal_generator pg(context, 3, 3, 1, 1, two_dimensional);

	vector<vec4> point_sequence = {
		vec4(0.0f, 0.0f, 0.0f, 1.0f),
		vec4(0.0f, 0.6f, 0.0f, 1.0f),
		vec4(0.0f, 0.0f, 0.0f, 1.0f),
		vec4(0.6f, 0.0f, 0.0f, 1.0f),
	};

	int num_points = 300000;

	//pg.generateFractal(point_sequence, num_points);
	pg.generateFractal(mc.getRandomVec4(), num_points);

	glfwSetTime(0);
	float render_fps = 60.0f;
	bool finished = false;
	clock_t start = clock();
	unsigned int frame_counter = 0;
	unsigned int counter_increment = 1;
	bool show_animation = true;
	bool smooth_lines = true;
	bool paused = false;

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
				frame_counter+= counter_increment;

			pg.drawFractal();
			pg.checkKeys(keys);

			//TODO see why this only works when include_hold is enabled
			if (keys->checkPress(GLFW_KEY_ESCAPE))
				finished = true;

			if (keys->checkPress(GLFW_KEY_I, false))
				show_animation = !show_animation;

			if (keys->checkPress(GLFW_KEY_G, false))
			{
				if (counter_increment == 100)
					counter_increment = 11;

				else counter_increment *= 10;
			}

			if (keys->checkPress(GLFW_KEY_F, false))
			{
				if (counter_increment == 1)
					counter_increment = 100;

				else counter_increment /= 10;
			}

			if (keys->checkPress(GLFW_KEY_N, false))
				pg.generateFractal(mc.getRandomVec4(), num_points);

			if (keys->checkPress(GLFW_KEY_R, false))
			{
				frame_counter = 0;
				show_animation = true;
				paused = false;
			}

			if (keys->checkPress(GLFW_KEY_T, false))
				paused = !paused;

			context->swapBuffers();
			glfwSetTime(0.0f);
		}
	}

	return 0;
}