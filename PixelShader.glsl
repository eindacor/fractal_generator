#version 330

/*
NOTE: THIS SHADER IS NOT BEING USED IN THE GENERATOR SHADERS ARE CREATED AS RAW STRINGS IN MAIN.CPP TO AVOID DEPENDENCIES ON EXTERNAL SHADER FILES
*/

in vec4 fragment_color;
out vec4 output_color;

in bool inverted_colors;

layout(location = 0) out vec4 color;

void main()
{
	output_color = fragment_color;
	color = fragment_color;
}