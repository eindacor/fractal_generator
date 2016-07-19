#include "header.h"
#include "matrix_creator.h"
#include "fractal_generator.h"
#include "screencap.h"
#include "geometry_generator.h"
#include "settings_manager.h"

const char* vertex_shader_string = "\
#version 330\n\
layout(location = 0) in vec4 position;\n\
layout(location = 1) in vec4 color;\n\
layout(location = 2) in float point_size;\n\
uniform mat4 MVP;\n\
uniform mat4 MV;\n\
uniform mat4 model_matrix;\n\
uniform mat4 view_matrix;\n\
uniform vec3 camera_position;\n\
uniform vec3 centerpoint;\n\
uniform vec4 background_color;\n\
uniform mat4 projection_matrix;\n\
uniform mat4 fractal_scale = mat4(1.0f);\n\
uniform int enable_growth_animation;\n\
uniform int light_effects_transparency;\n\
uniform int lighting_mode;\n\
uniform int frame_count;\n\
out vec4 fragment_color;\n\
uniform float point_size_scale = 1.0f;\n\
uniform float illumination_distance;\n\
uniform int invert_colors;\n\
uniform int palette_vertex_id;\n\
uniform float light_cutoff = float(0.005f);\n\
void main()\n\
{\n\
	if (frame_count < gl_VertexID && enable_growth_animation > 0)\n\
	{\n\
		fragment_color = vec4(color.rgb, 0.0f);\n\
		gl_Position = MVP * fractal_scale * position;\n\
		return;\n\
	}\n\
	if (gl_VertexID >= palette_vertex_id)\n\
	{\n\
		gl_Position = position;\n\
		fragment_color = vec4(color.rgb, 1.0f);\n\
		if (invert_colors > 0)\n\
		{\n\
			fragment_color = vec4(vec3(1.0) - fragment_color.rgb, 1.0f); \n\
		}\n\
	}\n\
	else \n\
	{\n\
		gl_PointSize = point_size * point_size_scale;\n\
		gl_Position = MVP * fractal_scale * position;\n\
		float alpha_value = color.a;\n\
		fragment_color = vec4(color.rgb, alpha_value);\n\
		if (invert_colors > 0)\n\
		{\n\
			fragment_color = vec4(vec3(1.0) - fragment_color.rgb, alpha_value); \n\
		}\n\
		if (lighting_mode > 0)\n\
		{\n\
			vec3 light_position;\n\
			float distance_from_light;\n\
			if (lighting_mode == 1)\n\
			{\n\
				light_position = camera_position;\n\
			}\n\
			else if (lighting_mode == 2)\n\
			{\n\
				light_position = vec3(0.0f, 0.0f, 0.0f);\n\
			}\n\
			else if (lighting_mode == 3)\n\
			{\n\
				light_position = centerpoint;\n\
			}\n\
			// lighting calcs from https://imdoingitwrong.wordpress.com/2011/01/31/light-attenuation/ \n\
			float r = illumination_distance;\n\
			vec3 L = light_position - vec3(position);\n\
			float distance = length(L);\n\
			float d = max(distance - r, 0);\n\
			L /= distance;\n\
			float denom = d/r + 1.0f;\n\
			float attenuation = 1.0f / (denom * denom);\n\
			attenuation = (attenuation - light_cutoff) / (1.0f - light_cutoff);\n\
			attenuation = max(attenuation, 0);\n\
			fragment_color = (fragment_color * attenuation) + (background_color * (1.0f - attenuation));\n\
			if (light_effects_transparency == 0)\n\
 				fragment_color.a = alpha_value;\n\
		}\n\
	}\n\
}\n\
";

const char* fragment_shader_string = "\
#version 330\n\
in vec4 fragment_color;\n\
out vec4 output_color;\n\
void main()\n\
{\n\
	output_color = fragment_color;\n\
}\n\
";

bool getYesOrNo(string prompt, bool default)
{
	string response;
	cout << prompt + ": ";
	std::getline(std::cin, response);
	cout << endl;
	if (response == "" || response == "\n")
		return default;

	else return response == "y" || response == "yes" || response == "true";
}

void getSettings(settings_manager &settings)
{
	bool use_defaults = getYesOrNo("use default settings?", true);
	if (use_defaults)
		return;

	string seed;
	cout << "enter seed: ";
	std::getline(std::cin, seed);
	cout << endl;
	seed.erase(std::remove(seed.begin(), seed.end(), '\n'), seed.end());
	settings.base_seed = seed;

	string point_count;
	cout << "point count: ";
	std::getline(std::cin, point_count);
	cout << endl;
	settings.num_points = (point_count == "" || point_count == "\n" || std::stoi(point_count) <= 0) ? 10000 : std::stoi(point_count);

	string window_width_input;
	cout << "window width: ";
	std::getline(std::cin, window_width_input);
	cout << endl;
	int window_width = (window_width_input == "" || window_width_input == "\n") ? 800 : std::stoi(window_width_input);
	settings.window_width = glm::clamp(window_width, 300, 4096);

	string window_height_input;
	cout << "window height: ";
	std::getline(std::cin, window_height_input);
	cout << endl;
	int window_height = (window_height_input == "" || window_height_input == "\n") ? 800 : std::stoi(window_height_input);
	settings.window_height = glm::clamp(window_height, 300, 4096);
}

int main()
{
	settings_manager settings;
	getSettings(settings);
	matrix_creator mc;

	if (settings.base_seed.size() == 0)
		settings.base_seed = mc.generateAlphanumericString(32);

	float eye_level = 0.0f;
	shared_ptr<ogl_context> context(new ogl_context("Fractal Generator", vertex_shader_string, fragment_shader_string, settings.window_width, settings.window_height, true));

	shared_ptr<fractal_generator> generator(new fractal_generator(settings.base_seed, context, settings.num_points));
	shared_ptr<key_handler> keys(new key_handler(context));

	shared_ptr<ogl_camera_flying> camera(new ogl_camera_flying(keys, context, vec3(0.0f, eye_level, 2.0f), 45.0f));

	/*
	SEEDS
	f6ujfV4rTtvN991MBr5gOCiaQ6TrAPaJ
	3LE1GoVEb1W9jV1GwrTyfMLVMYs8ipYT
	bdUUhVCQm5hLMzy85HPY30Ipzjv3S9uN
	fiUppj1hoBZyIZ2Vzq0NlGkdNUUKvcOM
	DeOBqMmyPGjnqXzxyCXyOD0mMt8QMkFn	<-animated
	TwolaeAd6aG4scUaEZcKpbctGn6t1HMz	<-animated
	X2u1ZkbGw00mdWSK1ZRktRluL9y0H9CH	<-animated (disable lines)
	*/

	glfwWindowHint(GLFW_SAMPLES, 4);
	glEnable(GL_MULTISAMPLE);

	glfwSetTime(0);
	float render_fps = 60.0f;
	bool finished = false;
	clock_t start = clock();
	unsigned int frame_counter = 0;
	unsigned int counter_increment = 1;
	bool paused = false;
	bool reverse = false;

	generator->printContext();

	while (!finished)
	{
		if (glfwGetTime() > 1.0f / render_fps)
		{
			if ((clock() - start) / CLOCKS_PER_SEC > 2.0f)
				start = clock();

			glfwPollEvents();
			context->clearBuffers();

			glUniform1i(context->getShaderGLint("enable_growth_animation"), generator->getSettings().show_growth ? 1 : 0);
			glUniform1i(context->getShaderGLint("frame_count"), frame_counter);

			if (!paused && generator->getSettings().show_growth)
			{
				if (generator->getSettings().show_growth && reverse && frame_counter > counter_increment)
					frame_counter -= counter_increment;

				else if (generator->getSettings().show_growth && !reverse)
					frame_counter += counter_increment;
			}

			if (!paused)
			{
				generator->tickAnimation();
			}

			if (settings.auto_tracking && !paused)
			{
				float average_delta = generator->getAverageDelta();
				camera->setPosition(generator->getFocalPoint() + vec3(average_delta * 6.0f));
				camera->setFocus(generator->getFocalPoint());
			}

			camera->updateCamera();
			vec3 camera_pos = camera->getPosition();
			glUniform3fv(context->getShaderGLint("camera_position"), 1, &camera_pos[0]);
			camera->setMVP(context, mat4(1.0f), jep::NORMAL);

			generator->drawFractal();
			generator->checkKeys(keys);

			if (keys->checkPress(GLFW_KEY_0, false))
			{
				settings.auto_tracking = !settings.auto_tracking;
				settings.auto_tracking ? cout << "auto tracking enabled" << endl : cout << "auto tracking disabled" << endl;
			}

			if (keys->checkPress(GLFW_KEY_9, false))
			{
				camera->setPosition(vec3(0.0f, eye_level, 2.0f));
				camera->setFocus(vec3(0.0f, 0.0f, 0.0f));
			}

			//TODO see why this only works when include_hold is enabled
			if (keys->checkPress(GLFW_KEY_ESCAPE))
				finished = true;

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

			if (keys->checkPress(GLFW_KEY_T, false))
				paused = !paused;

			context->swapBuffers();

			if (keys->checkPress(GLFW_KEY_X, false)) {
				//saveImage(4.0f, *generator, context, JPG);
				//saveImage(4.0f, *generator, context, PNG);
				saveImage(4.0f, *generator, context, BMP, 6);
				//saveImage(4.0f, *generator, context, TIFF);
			}

			if (keys->checkPress(GLFW_KEY_SPACE, false))
			{
				settings.base_seed = "";

				if (keys->checkShiftHold())
				{
					string seed;
					cout << "enter seed: ";
					std::getline(std::cin, seed);
					cout << endl;
					seed.erase(std::remove(seed.begin(), seed.end(), '\n'), seed.end());
					settings.base_seed = seed;
				}

				if (settings.base_seed.size() == 0)
					settings.base_seed = mc.generateAlphanumericString(32);

				shared_ptr<fractal_generator> new_generator(new fractal_generator(settings.base_seed, context, settings.num_points));
				generator = new_generator;
				generator->printContext();
				frame_counter = 0;
				counter_increment = 1;
				paused = false;
				reverse = false;
			}

			if (keys->checkPress(GLFW_KEY_R, false))
			{
				shared_ptr<fractal_generator> new_generator(new fractal_generator(settings.base_seed, context, settings.num_points));
				generator = new_generator;
				generator->printContext();
				frame_counter = 0;
				counter_increment = 1;
				paused = false;
				reverse = false;
			}

			glfwSetTime(0.0f);
		}
	}

	return 0;
}