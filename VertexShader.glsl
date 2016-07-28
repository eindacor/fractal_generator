#version 430
layout(location = 0) in vec4 position; 
layout(location = 1) in vec4 color; 
layout(location = 2) in float point_size; 
layout(location = 3) in vec2 palette_position; 
uniform mat4 MVP; 
uniform mat4 MV; 
uniform mat4 model_matrix; 
uniform mat4 view_matrix; 
uniform vec3 camera_position; 
uniform vec3 centerpoint; 
uniform vec4 background_color; 
uniform mat4 projection_matrix; 
uniform mat4 fractal_scale = mat4(1.0f); 
uniform int enable_growth_animation; 
uniform int light_effects_transparency; 
uniform int lighting_mode; 
uniform int frame_count; 
out vec4 fragment_color; 
uniform float point_size_scale = 1.0f; 
uniform float illumination_distance; 
uniform int invert_colors; 
uniform float light_cutoff = float(0.3f);
uniform mat4 quadrant_matrix; 
uniform int render_quadrant; 
uniform int render_palette; 
uniform int geometry_type; //0 = vertices, 1 = lines, 2 = triangles
uniform int override_line_color_enabled;
uniform vec4 line_override_color;
uniform vec4 lights[256];
uniform vec4 light_colors[256];
uniform float light_illumination_distances[256];
uniform int light_count;

vec4 modifyLight(vec4 vertex_color, float illumination_dist, vec4 light_pos, vec4 vertex_pos, vec4 light_color, float cutoff)
{
	float attenuation;
	vec4 L = light_pos - vertex_pos;
	float distance = length(L);
	float d = max(distance - illumination_dist, 0);
	L /= distance;
	float denom = d / (illumination_dist * illumination_distance) + 1.0f;
	attenuation = 1.0f / (denom * denom);
	attenuation = (attenuation - cutoff) / (1.0f - cutoff);
	attenuation = max(attenuation, 0.0f);

	vec4 new_color = vertex_color;
	new_color += (light_color * attenuation);

	new_color.r = clamp(new_color.r, 0.0f, 1.0f);
	new_color.g = clamp(new_color.g, 0.0f, 1.0f);
	new_color.b = clamp(new_color.b, 0.0f, 1.0f);
	new_color.a = clamp(new_color.a, 0.0f, 1.0f);

	return new_color;
}

float getAttenuationFromPosition(float illumination_dist, vec4 light_pos, vec4 vertex_pos, float cutoff)
{
	float attenuation;
	vec4 L = light_pos - vertex_pos;
	float distance = length(L);
	float d = max(distance - illumination_dist, 0);
	L /= distance;
	float denom = d / (illumination_dist) + 1.0f;
	attenuation = 1.0f / (denom * denom);
	attenuation = (attenuation - cutoff) / (1.0f - cutoff);
	
	return max(attenuation, 0);
}

float getAttenuation()
{
	vec4 light_position;
	float light_intensity = 1.0f;
	float distance_from_light;

	if (lighting_mode == 1)
	{
		light_position = vec4(camera_position.xyz, light_intensity);
	}

	else if (lighting_mode == 2)
	{
		light_position = vec4(0.0f, 0.0f, 0.0f, light_intensity);
	}

	else if (lighting_mode == 3)
	{
		light_position = vec4(centerpoint.xyz, light_intensity);
	}

	float global_attenuation = getAttenuationFromPosition(illumination_distance, light_position, position, light_cutoff);
	global_attenuation = 0.0f;

	for (int i = 0; i < 256; i++)
	{
		if (lights[i].w < .001f)
			continue;

		global_attenuation += getAttenuationFromPosition(illumination_distance, lights[i], position, light_cutoff);
	}

	return clamp(global_attenuation, 0.0f, 1.0f);
}

void main()
{
	if (render_palette > 0)
	{
		gl_Position = vec4(palette_position.x, palette_position.y, 0.0f, 1.0f);
		float alpha_value = color.a;
		fragment_color = vec4(color.rgb, alpha_value);
		if (invert_colors > 0)
		{
			fragment_color = vec4(vec3(1.0) - fragment_color.rgb, alpha_value); 
		}
		return;
	}

	gl_PointSize = point_size * point_size_scale;
	gl_Position = MVP * fractal_scale * position;

	if (frame_count < gl_VertexID && enable_growth_animation > 0)
	{
		fragment_color = vec4(color.rgb, 0.0f);
		return;
	}

	float alpha_value;
	if (override_line_color_enabled == 1 && geometry_type == 1)
	{
		alpha_value = 1.0f;
		fragment_color = line_override_color;
	}

	else
	{
		alpha_value = color.a;
		fragment_color = vec4(color.rgb, alpha_value);
	}

	if (invert_colors > 0)
	{
		fragment_color = vec4(vec3(1.0) - fragment_color.rgb, alpha_value); 
	}

	if (lighting_mode > 0)
	{
		/*float attenuation = getAttenuation();
		fragment_color = (fragment_color * attenuation) + (background_color * (1.0f - attenuation));*/

		for (int i = 0; i < 256; i++)
		{
			if (lights[i].w < .001f)
				continue;

			fragment_color = modifyLight(fragment_color, light_illumination_distances[i], lights[i], position, light_colors[i], light_cutoff);
		}
		
		if (light_effects_transparency == 0)
			fragment_color.a = alpha_value;
	}

	if (render_quadrant > 0)
	{
		gl_Position = quadrant_matrix * gl_Position;
	}
}