#version 330

in vec3 fragment_color;
out vec4 output_color;

void main()
{
	output_color = vec4(fragment_color, 1.0);
}