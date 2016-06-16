#version 330

/*
	NOTE: THIS SHADER IS NOT BEING USED IN THE GENERATOR SHADERS ARE CREATED AS RAW STRINGS IN MAIN.CPP TO AVOID DEPENDENCIES ON EXTERNAL SHADER FILES
*/

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in float point_size;

uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 fractal_scale = mat4(1.0f);

uniform int enable_growth_animation;
uniform int frame_count;
uniform float point_size_scale = 1.0f;
uniform int invert_colors;

out vec4 fragment_color;
out bool invert_colors;

void main()
{
	gl_PointSize = point_size * point_size_scale;
	gl_Position = MVP * fractal_scale * position;

	float alpha_value = (frame_count > gl_VertexID) || (enable_growth_animation == 0) ? color.a : 0.0f;

	if (invert_colors > 0)
	{
		fragment_color = vec4(vec3(1.0) - color.rgb, alpha_value);
	}

	else fragment_color = vec4(color.rgb, alpha_value);
}