
/*
	NOTE: THIS SHADER IS NOT BEING USED IN THE GENERATOR SHADERS ARE CREATED AS RAW STRINGS IN MAIN.CPP TO AVOID DEPENDENCIES ON EXTERNAL SHADER FILES
*/
#version 330
layout(location = 0) in vec4 position; 
layout(location = 1) in vec4 color; 
layout(location = 2) in float point_size; 
uniform mat4 MVP; 
uniform mat4 MV; 
uniform mat4 model_matrix; 
uniform mat4 view_matrix; 
uniform vec3 camera_position; 
uniform vec3 centerpoint; 
uniform mat4 projection_matrix; 
uniform mat4 fractal_scale = mat4(1.0f); 
uniform int enable_growth_animation; 
uniform int lighting_mode; 
uniform int frame_count; 
out vec4 fragment_color; 
uniform float point_size_scale = 1.0f; 
uniform int invert_colors; 
uniform int palette_vertex_id; 

void main()
{
	if (gl_VertexID >= palette_vertex_id)
	{
		gl_Position = position; 
		fragment_color = vec4(color.rgb, 1.0f); 
		if (invert_colors > 0)
		{
			fragment_color = vec4(vec3(1.0) - fragment_color.rgb, 1.0f); 
		}
	}
	else 
	{
		gl_PointSize = point_size * point_size_scale; 
		gl_Position = MVP * fractal_scale * position; 
		float alpha_value = (frame_count > gl_VertexID) || (enable_growth_animation == 0) ? color.a : 0.0f; 
		fragment_color = vec4(color.rgb, alpha_value); 
		if (lighting_mode > 0)
		{
			float light_distance = 30.0f; 
			float distance_modifier; 
			if (lighting_mode == 1)
			{
				float distance_from_camera = length(position - vec4(camera_position, 1.0)); 
				distance_modifier = clamp(1.0f - (distance_from_camera / light_distance), 0.0f, 1.0f); 
			}
			else if (lighting_mode == 2)
			{
				float distance_from_origin = length(position); 
				distance_modifier = clamp(1.0f - (distance_from_origin / light_distance), 0.0f, 1.0f); 
			}
			else if (lighting_mode == 3)
			{
				float distance_from_centerpoint = length(position - vec4(centerpoint, 1.0)); 
				distance_modifier = clamp(1.0f - (distance_from_centerpoint / light_distance), 0.0f, 1.0f); 
			}
			fragment_color = vec4(fragment_color.rgb, fragment_color.a * distance_modifier); 
		}
		if (invert_colors > 0)
		{
			fragment_color = vec4(vec3(1.0) - fragment_color.rgb, alpha_value); 
		}
	}
}