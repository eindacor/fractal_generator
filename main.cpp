#include "header.h"
#include "matrix_creator.h"
#include "point_generator.h"

int main()
{
	jep::init();
	string data_path = "c:\\Users\\jpollack\\documents\\github\\fractal_generator\\";
	//string data_path = "j:\\Github\\fractal_generator\\";
	string vert_file = data_path + "VertexShader.glsl";
	string frag_file = data_path + "PixelShader.glsl";

	float eye_level = 0.0f;
	shared_ptr<ogl_context> context(new ogl_context("Fractal Generator", vert_file.c_str(), frag_file.c_str(), 900, 900));
	shared_ptr<key_handler> keys(new key_handler(context));
	shared_ptr<texture_handler> textures(new texture_handler(data_path));
	shared_ptr<ogl_camera_free> camera(new ogl_camera_free(keys, context, vec3(0.0f, eye_level, 1.0f), 45.0f));

	matrix_creator mc;
	point_generator pg(4, 1, 1, 1);

	vector<vec3> point_sequence = {
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 0.1f, 0.0f),
		vec3(0.0f, 0.2f, 0.0f),
		vec3(0.0f, 0.3f, 0.0f),
		vec3(0.0f, 0.4f, 0.0f),
		vec3(0.0f, 0.5f, 0.0f),
		vec3(0.1f, 0.0f, 0.0f),
		vec3(0.2f, 0.0f, 0.0f),
		vec3(0.3f, 0.0f, 0.0f),
		vec3(0.4f, 0.0f, 0.0f),
		vec3(0.5f, 0.0f, 0.0f),
		vec3(0.6f, 0.0f, 0.0f),
	};

	vector<float> vertex_data = pg.getPoints(point_sequence, 300000);
	//vector<float> vertex_data = pg.getPoints(vec3(mc.getRandomFloat(), mc.getRandomFloat(), mc.getRandomFloat()), 10000);

	// create/bind Vertex Array Object
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// create/bind Vertex Buffer Object
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_data.size(), &vertex_data[0], GL_STATIC_DRAW);

	// stride is the total size of each vertex's attribute data (position + color + size)
	int stride = 8 * sizeof(float);

	// load position data
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

	// load color data
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

	// load point size
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void*)(7 * sizeof(float)));

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glEnable(GL_PROGRAM_POINT_SIZE);
	glDepthRange(0, 10000);
	context->setBackgroundColor(vec4(0.0, 0.0, 0.0, 0.0));

	glfwSetTime(0);
	float render_fps = 60.0f;
	bool finished = false;
	clock_t start = clock();
	int frame_counter = 0;

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

			glUniform1i(context->getShaderGLint("enable_growth_animation"), 0);
			glUniform1i(context->getShaderGLint("frame_count"), frame_counter);
			frame_counter++;

			// bind target VAO
			glBindVertexArray(VAO);
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);

			// draw type, offset, number of vertices
			glDrawArrays(GL_POINTS, 0, vertex_data.size() / 7);

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
			glBindVertexArray(0);

			//TODO see why this only works when include_hold is enabled
			if (keys->checkPress(GLFW_KEY_ESCAPE))
				finished = true;

			context->swapBuffers();
			glfwSetTime(0.0f);
		}
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	return 0;
}