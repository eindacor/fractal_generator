#include "header.h"
#include "matrix_creator.h"

int main()
{
	string data_path = "c:\\Users\\jpollack\\documents\\visual studio 2015\\Projects\\fractal_generator\\fractal_generator\\";
	string vert_file = data_path + "VertexShader.glsl";
	string frag_file = data_path + "PixelShader.glsl";

	jep::ogl_context* context = new jep::ogl_context("Fractal Generator", vert_file.c_str(), frag_file.c_str(), 1280, 720);

	matrix_creator mc;

	for (int i = 0; i < 10; i++)
	{
		cout << mc.getRandomFloat() << endl;
	}

	for (int i = 0; i < 10; i++)
	{
		cout << mc.getRandomFloatInRange(0.0f, 10.0f) << endl;
	}

	for (int i = 0; i < 10; i++)
	{
		cout << glm::to_string(mc.getRandomTranslation()) << endl;
	}

	for (int i = 0; i < 10; i++)
	{
		cout << glm::to_string(mc.getRandomTranslation(random_switch(true, 1.0f), random_switch(true, 3.0f), random_switch(false, 1.0f))) << endl;
	}

	return 0;
}