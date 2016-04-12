#version 330

in vec4 fragment_color;
out vec4 output_color;

layout(location = 0) out vec4 color;

void main()
{
	output_color = fragment_color;
	color = fragment_color;
}