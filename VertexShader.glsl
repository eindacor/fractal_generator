#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in float point_size;

uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 model_matrix;
uniform mat4 view_matrix;

uniform int enable_growth_animation;
uniform int frame_count;

out vec4 fragment_color;

void main()
{
	gl_PointSize = point_size;
	gl_Position = MVP * vec4(position, 1.0);

	float alpha_value = (frame_count > gl_VertexID) || (enable_growth_animation == 0) ? color.a : 0.0f;

	fragment_color = vec4(color.rgb, alpha_value);
}