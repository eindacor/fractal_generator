#include "header.h"
#include "matrix_creator.h"
#include "fractal_generator.h"
#include "screencap.h"

const char* vertex_shader_string = "\
#version 330\n\
layout(location = 0) in int matrix_index;\n\
uniform mat4 MVP;\n\
uniform mat4 MV;\n\
uniform mat4 model_matrix;\n\
uniform mat4 view_matrix;\n\
uniform mat4 projection_matrix;\n\
uniform mat4 fractal_scale = mat4(1.0f);\n\
uniform int enable_growth_animation;\n\
uniform int frame_count;\n\
out vec4 fragment_color;\n\
uniform float point_size_scale = 1.0f;\n\
uniform int invert_colors;\n\
\n\
uniform mat4 transformation_matrices_front[16];\n\
uniform mat4 transformation_matrices_back[16];\n\
uniform vec4 colors_front[16];\n\
uniform vec4 colors_back[16];\n\
uniform float sizes_front[16];\n\
uniform float sizes_back[16];\n\
uniform int matrix_count;\n\
uniform int front_buffer_first;\n\
uniform float interpolation_state;\n\
vec4 output_point = vec4(0.0, 0.0, 0.0, 1.0);\n\
vec4 output_color = vec4(0.5, 0.5, 0.5, 1.0);\n\
float output_size = 1.0;\n\
\n\
void main()\n\
{\n\
	float alpha_value = (frame_count > gl_VertexID) || (enable_growth_animation == 0) ? 1.0 : 0.0;\n\
	mat4 transformation_matrix;\n\
	vec4 transformation_color;\n\
	float transformation_size;\n\
	for (int i = 0; i < gl_VertexID; i++)\n\
	{\n\
		int matrix_index = int(((noise1(1.0) + 1.0) / 2.0) * matrix_count);\n\
\n\
		if (front_buffer_first > 0)\n\
		{\n\
			transformation_matrix = (transformation_matrices_front[matrix_index] * interpolation_state) + (transformation_matrices_back[matrix_index] * (1.0 - interpolation_state));\n\
			transformation_color = (colors_front[matrix_index] * interpolation_state) + (colors_back[matrix_index] * (1.0 - interpolation_state));\n\
			transformation_size = (sizes_front[matrix_index] * interpolation_state) + (sizes_back[matrix_index] * (1.0 - interpolation_state));\n\
		}\n\
\n\
		else if ()\n\
		{\n\
			transformation_matrix = (transformation_matrices_back[matrix_index] * interpolation_state) + (transformation_matrices_front[matrix_index] * (1.0 - interpolation_state));\n\
			transformation_color = (colors_back[matrix_index] * interpolation_state) + (colors_front[matrix_index] * (1.0 - interpolation_state));\n\
			transformation_size = (sizes_back[matrix_index] * interpolation_state) + (sizes_front[matrix_index] * (1.0 - interpolation_state));\n\
		}\n\
\n\
		output_point = transformation_matrix * output_point;\n\
		output_color = (output_color * float(gl_VertexID) + transformation_color) / float(gl_VertexID + 1);\n\
		output_size = (output_size * float(gl_VertexID) + transformation_size) / float(gl_VertexID + 1);\n\
	}\n\
\n\
		mat4 MVP = projection_matrix * view_matrix * transformation_matrix;\n\
		gl_Position = MVP * fractal_scale * output_point;\n\
		if (invert_colors > 0)\n\
		{\n\
			fragment_color = vec4(vec3(1.0) - output_color.rgb, output_color.a);\n\
		}\n\
		else { fragment_color = output_color; }\n\
		gl_PointSize = output_size * point_size_scale;\n\
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

void getSettings(string &seed, bool &refresh_enabled, bool &two_dimensional, int &num_points, int &window_width, int &window_height)
{
	string use_defaults;
	cout << "use default settings?: ";
	std::getline(std::cin, use_defaults);
	std::transform(use_defaults.begin(), use_defaults.end(), use_defaults.begin(), ::tolower);
	cout << endl;

	if (use_defaults == "y" || use_defaults == "yes" || use_defaults == "true" || use_defaults == "" || use_defaults == "\n")
		return;

	cout << "enter seed: ";
	std::getline(std::cin, seed);
	cout << endl;
	seed.erase(std::remove(seed.begin(), seed.end(), '\n'), seed.end());

	string refresh_enabled_input;
	cout << "refresh enabled?: ";
	std::getline(std::cin, refresh_enabled_input);
	std::transform(refresh_enabled_input.begin(), refresh_enabled_input.end(), refresh_enabled_input.begin(), ::tolower);
	refresh_enabled = refresh_enabled_input == "y" || refresh_enabled_input == "yes" || refresh_enabled_input == "true";
	cout << endl;

	string two_dimensional_enabled_input;
	cout << "2D?: ";
	std::getline(std::cin, two_dimensional_enabled_input);
	std::transform(two_dimensional_enabled_input.begin(), two_dimensional_enabled_input.end(), two_dimensional_enabled_input.begin(), ::tolower);
	cout << endl;
	two_dimensional = (two_dimensional_enabled_input == "y" || two_dimensional_enabled_input == "yes" || two_dimensional_enabled_input == "true");

	string point_count;
	cout << "point count: ";
	std::getline(std::cin, point_count);
	cout << endl;
	num_points = (point_count == "" || point_count == "\n" || std::stoi(point_count) <= 0) ? 10000 : std::stoi(point_count);

	string window_width_input;
	cout << "window width: ";
	std::getline(std::cin, window_width_input);
	cout << endl;
	window_width = (window_width_input == "" || window_width_input == "\n") ? 1366 : std::stoi(window_width_input);
	window_width = glm::clamp(window_width, 600, 4096);

	string window_height_input;
	cout << "window height: ";
	std::getline(std::cin, window_height_input);
	cout << endl;
	window_height = (window_height_input == "" || window_height_input == "\n") ? 768 : std::stoi(window_height_input);
	window_height = glm::clamp(window_height, 600, 4096);
}

int main()
{
	string seed = "";
	bool refresh_enabled = false;
	bool two_dimensional = false;
	int num_points = 10000;
	int window_width = 1366;
	int window_height = 768;

	getSettings(seed, refresh_enabled, two_dimensional, num_points, window_width, window_height);

	float eye_level = 0.0f;
	shared_ptr<ogl_context> context(new ogl_context("Fractal Generator", vertex_shader_string, fragment_shader_string, window_width, window_height, true));
	shared_ptr<key_handler> keys(new key_handler(context));
	shared_ptr<ogl_camera_flying> camera(new ogl_camera_flying(keys, context, vec3(0.0f, eye_level, 2.0f), 45.0f));

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
		
	fractal_generator *generator;
	float inter_start = 0.0f;
	bool smooth = true;

	glfwWindowHint(GLFW_SAMPLES, 4);
	glEnable(GL_MULTISAMPLE);

	// TODO create factory class that creates generators, only keep one constructor that requires all parameters
	if (seed.size() == 0)
		generator = new fractal_generator(context, num_points, two_dimensional);

	else generator = new fractal_generator(seed, context, num_points, two_dimensional);

	if (refresh_enabled)
		generator->generateFractalWithRefresh();

	else generator->generateFractal();

	glfwSetTime(0);
	float render_fps = 60.0f;
	bool finished = false;
	clock_t start = clock();
	unsigned int frame_counter = 0;
	unsigned int counter_increment = 1;
	bool show_growth = true;
	bool smooth_lines = true;
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

			camera->updateCamera();
			camera->setMVP(context, mat4(1.0f), jep::NORMAL);
			//TODO update ogl_tools to allow a setProjectionMatrixFunction
			mat4 view_matrix = camera->getViewMatrix();
			mat4 projection_matrix = camera->getProjectionMatrix();
			glUniformMatrix4fv(context->getShaderGLint("projection_matrix"), 1, GL_FALSE, &projection_matrix[0][0]);
			glUniformMatrix4fv(context->getShaderGLint("view_matrix"), 1, GL_FALSE, &view_matrix[0][0]);

			glUniform1i(context->getShaderGLint("enable_growth_animation"), show_growth ? 1 : 0);
			glUniform1i(context->getShaderGLint("frame_count"), frame_counter);

			if (!paused && show_growth)
			{
				if (reverse && frame_counter > counter_increment)
					frame_counter -= counter_increment;

				else if (!reverse)
					frame_counter += counter_increment;
			}

			if (!paused)
				generator->tickAnimation();

			generator->drawFractal();
			generator->checkKeys(keys);

			//TODO see why this only works when include_hold is enabled
			if (keys->checkPress(GLFW_KEY_ESCAPE))
				finished = true;

			if (keys->checkPress(GLFW_KEY_J, false))
				show_growth = !show_growth;

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
				show_growth = true;
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