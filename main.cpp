#include "header.h"
#include "random_generator.h"
#include "fractal_generator.h"
#include "screencap.h"
#include "geometry_generator.h"
#include "settings_manager.h"

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
	random_generator mc;

	if (settings.base_seed.size() == 0)
		settings.base_seed = mc.generateAlphanumericString(32);

	float eye_level = 0.0f;
	shared_ptr<ogl_context> context(new ogl_context("Fractal Generator", "VertexShader.glsl", "PixelShader.glsl", settings.window_width, settings.window_height, false));

	shared_ptr<fractal_generator> generator(new fractal_generator(settings.base_seed, context, settings.num_points));
	shared_ptr<key_handler> keys(new key_handler(context));

	float camera_fov = 45.0f;

	shared_ptr<ogl_camera_flying> camera(new ogl_camera_flying(keys, context, vec3(0.0f, eye_level, 5.0f), camera_fov));
	camera->setStepDistance(0.02f);
	camera->setStrafeDistance(0.02f);
	camera->setRotateAngle(1.0f);
	camera->setTiltAngle(1.0f);

	/*jep::obj_contents torus("torus.obj");
	jep::obj_contents helix("helix.obj");
	jep::obj_contents sphere("sphere.obj");
	generator->loadPointSequence("torus", torus.getAllVerticesOfAllMeshes());
	generator->loadPointSequence("helix", helix.getAllVerticesOfAllMeshes());
	generator->loadPointSequence("sphere", sphere.getAllVerticesOfAllMeshes());*/

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
	bool pause_on_swap = false;
	bool growth_paused = false;
	bool recording = false;
	int gif_frame_count = 150;
	int current_gif_frame = 0;

	generator->printContext();

	while (!finished)
	{
		if (glfwGetTime() > 1.0f / render_fps)
		{
			if ((clock() - start) / CLOCKS_PER_SEC > 2.0f)
				start = clock();

			glfwPollEvents();
			context->clearBuffers();

			if (recording)
			{
				batchRender(4.0f, *generator, context, BMP, 4, 1, 1, 720, false);
				current_gif_frame++;

				if (current_gif_frame == gif_frame_count)
				{
					recording = false;
					current_gif_frame = 0;
				}
			}

			glUniform1i(context->getShaderGLint("enable_growth_animation"), generator->getSettings().show_growth ? 1 : 0);
			glUniform1i(context->getShaderGLint("frame_count"), frame_counter);

			if (keys->checkPress(GLFW_KEY_SLASH, false))
			{
				pause_on_swap = !pause_on_swap;
			}

			if (pause_on_swap && (generator->getInterpolationState() < 0.0001f || generator->getInterpolationState() > 0.9999f))
			{
				paused = true;
				pause_on_swap = false;
			}

			if (!paused && generator->getSettings().show_growth && !growth_paused)
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

			if (keys->checkPress(GLFW_KEY_5, false))
			{
				growth_paused = !growth_paused;
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
				camera->setPosition(vec3(0.0f, eye_level, 5.0f));
				camera->setFocus(vec3(0.0f, 0.0f, 0.0f));
			}

			if (keys->checkPress(GLFW_KEY_INSERT, true))
			{
				camera_fov = glm::clamp(camera_fov + 0.5f, 1.0f, 180.0f);
				camera->setFOV(camera_fov);
				cout << camera_fov << endl;
			}

			if (keys->checkPress(GLFW_KEY_DELETE, true))
			{
				camera_fov = glm::clamp(camera_fov - 0.5f, 1.0f, 180.0f);
				camera->setFOV(camera_fov);
				cout << camera_fov << endl;
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

			if (keys->checkPress(GLFW_KEY_X, false)) 
			{
				if (keys->checkShiftHold())
				{
					string x_quadrants_input;
					cout << "x quadrants: ";
					std::getline(std::cin, x_quadrants_input);
					cout << endl;
					int x_quadrants = (x_quadrants_input == "" || x_quadrants_input == "\n") ? 1 : std::stoi(x_quadrants_input);
					x_quadrants = glm::clamp(x_quadrants, 1, 100);

					string y_quadrants_input;
					cout << "y quadrants: ";
					std::getline(std::cin, y_quadrants_input);
					cout << endl;
					int y_quadrants = (y_quadrants_input == "" || y_quadrants_input == "\n") ? 1 : std::stoi(y_quadrants_input);
					y_quadrants = glm::clamp(y_quadrants, 1, 100);

					string quadrant_size_input;
					cout << "quadrant size: ";
					std::getline(std::cin, quadrant_size_input);
					cout << endl;
					int quadrant_size = (quadrant_size_input == "" || quadrant_size_input == "\n") ? 1024 : std::stoi(quadrant_size_input);
					quadrant_size = glm::clamp(quadrant_size, 128, 2048);

					bool mix_background = getYesOrNo("varied background?", false);

					batchRender(4.0f, *generator, context, BMP, 4, x_quadrants, y_quadrants, quadrant_size, mix_background);
				}

				else if (keys->checkCtrlHold())
				{
					bool mix_background = getYesOrNo("varied background?", false);

					batchRender(4.0f, *generator, context, BMP, 4, 4, 4, 2250, mix_background); // 30x30
					batchRender(4.0f, *generator, context, BMP, 4, 4, 4, 1800, mix_background);	// 24x24
					batchRender(4.0f, *generator, context, BMP, 4, 2, 2, 2400, mix_background);	// 16x16
					batchRender(4.0f, *generator, context, BMP, 4, 2, 2, 1800, mix_background);	// 12x12
				}

				else if (keys->checkAltHold())
				{
					string frame_record_count;
					cout << "frames to record: ";
					std::getline(std::cin, frame_record_count);
					cout << endl;
					gif_frame_count = (frame_record_count == "" || frame_record_count == "\n") ? 150 : std::stoi(frame_record_count);
					gif_frame_count = glm::clamp(gif_frame_count, 30, 900);

					recording = true;
					paused = false;
				}

				else
				{
					//saveImage(4.0f, *generator, context, JPG);
					//saveImage(4.0f, *generator, context, PNG);
					saveImage(4.0f, *generator, context, BMP, 4);
					//saveImage(4.0f, *generator, context, TIFF);
				}
				
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
				/*generator->loadPointSequence("torus", torus.getAllVerticesOfAllMeshes());
				generator->loadPointSequence("helix", helix.getAllVerticesOfAllMeshes());
				generator->loadPointSequence("sphere", sphere.getAllVerticesOfAllMeshes());*/
				frame_counter = 0;
				counter_increment = 1;
				paused = false;
				reverse = false;
				growth_paused = false;
				camera_fov = 45.0f;
				camera->setFOV(camera_fov);
			}

			if (keys->checkPress(GLFW_KEY_R, false))
			{
				shared_ptr<fractal_generator> new_generator(new fractal_generator(settings.base_seed, context, settings.num_points));
				generator = new_generator;
				generator->printContext();
				/*generator->loadPointSequence("torus", torus.getAllVerticesOfAllMeshes());
				generator->loadPointSequence("helix", helix.getAllVerticesOfAllMeshes());
				generator->loadPointSequence("sphere", sphere.getAllVerticesOfAllMeshes());*/
				frame_counter = 0;
				counter_increment = 1;
				paused = false;
				reverse = false;
				growth_paused = false;
				camera_fov = 45.0f;
				camera->setFOV(camera_fov);
			}

			glfwSetTime(0.0f);
		}
	}

	return 0;
}