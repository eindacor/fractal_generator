#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 model_matrix;
uniform mat4 view_matrix;

out vec3 fragment_color;

void main()
{
	gl_Position = MVP * vec4(position, 1.0);
	fragment_color = color;
}