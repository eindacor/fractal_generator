#include "header.h"
#include "matrix_creator.h"
#include "fractal_generator.h"
#include "screencap.h"

const char* vertex_shader_string = "\
#version 330\n\
layout(location = 0) in vec4 position;\n\
layout(location = 1) in vec4 color;\n\
layout(location = 2) in float point_size;\n\
uniform mat4 MVP;\n\
uniform mat4 MV;\n\
uniform mat4 model_matrix;\n\
uniform mat4 view_matrix;\n\
uniform mat4 fractal_scale = mat4(1.0f);\n\
uniform int enable_growth_animation;\n\
uniform int frame_count;\n\
out vec4 fragment_color;\n\
void main()\n\
{\n\
	gl_PointSize = point_size;\n\
	gl_Position = MVP * fractal_scale * position;\n\
	float alpha_value = (frame_count > gl_VertexID) || (enable_growth_animation == 0) ? color.a : 0.0f;\n\
	fragment_color = vec4(color.rgb, alpha_value);\n\
}\n\
";

const char* fragment_shader_string = "\
#version 330\n\
in vec4 fragment_color;\n\
out vec4 output_color;\n\
layout(location = 0) out vec4 color;\n\
void main()\n\
{\n\
	output_color = fragment_color;\n\
	color = fragment_color;\n\
}\n\
";

int main()
{
	//string data_path = "c:\\Users\\jpollack\\documents\\github\\fractal_generator\\";
	//string data_path = "j:\\Github\\fractal_generator\\";
	//string vert_file = data_path + "VertexShader.glsl";
	//string frag_file = data_path + "PixelShader.glsl";

	string entered_seed;

	cout << "enter seed: ";
	std::getline(std::cin, entered_seed);
	cout << endl;

	entered_seed.erase(std::remove(entered_seed.begin(), entered_seed.end(), '\n'), entered_seed.end());

	string refresh_enabled;
	cout << "refresh enabled?: ";
	std::getline(std::cin, refresh_enabled);
	std::transform(refresh_enabled.begin(), refresh_enabled.end(), refresh_enabled.begin(), ::tolower);
	cout << endl;

	float eye_level = 0.0f;
	shared_ptr<ogl_context> context(new ogl_context("Fractal Generator", vertex_shader_string, fragment_shader_string, 1024, 512, true));
	shared_ptr<key_handler> keys(new key_handler(context));
	shared_ptr<ogl_camera_flying> camera(new ogl_camera_flying(keys, context, vec3(0.0f, eye_level, 1.0f), 45.0f));

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

	/*
	SEEDS
	f6ujfV4rTtvN991MBr5gOCiaQ6TrAPaJ
	3LE1GoVEb1W9jV1GwrTyfMLVMYs8ipYT
	bdUUhVCQm5hLMzy85HPY30Ipzjv3S9uN
	fiUppj1hoBZyIZ2Vzq0NlGkdNUUKvcOM
	*/

	bool two_dimensional = false;
	fractal_generator *generator;

	if (entered_seed.size() == 0)
		generator = new fractal_generator(context, two_dimensional);

	else generator = new fractal_generator(entered_seed, context, two_dimensional);

	int num_points = 50000;

	//generator->generateFractal(point_sequence, num_points);

	if (refresh_enabled == "y" || refresh_enabled == "yes" || refresh_enabled == "true")
		generator->generateFractal(num_points, 5);

	else generator->generateFractal(num_points);

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

			generator->drawFractal();
			generator->checkKeys(keys);

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

			context->swapBuffers();

			if (keys->checkPress(GLFW_KEY_X, false))
				saveImage(8.0f, *generator, context);

			glfwSetTime(0.0f);
		}
	}

	delete generator;
	return 0;
}