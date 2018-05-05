#version 430

#define LIGHT_COUNT 256

layout(location = 0) in vec4 position; 
layout(location = 1) in vec4 color; 
layout(location = 2) in float point_size; 
layout(location = 3) in vec2 palette_position; 
uniform mat4 MVP; 
uniform mat4 model_matrix; 
uniform mat4 view_matrix; 
uniform vec3 camera_position; 
uniform vec3 centerpoint; 
uniform vec4 background_color; 
uniform mat4 projection_matrix; 
uniform mat4 fractal_scale = mat4(1.0f); 
uniform int light_effects_transparency; 
uniform int lighting_mode; 
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
uniform int override_triangle_color_enabled;
uniform vec4 triangle_override_color;
uniform int override_point_color_enabled;
uniform vec4 point_override_color;
uniform int override_light_color_enabled;
uniform vec4 light_override_color;
uniform vec4 light_positions[LIGHT_COUNT];
uniform vec4 light_colors[LIGHT_COUNT];
uniform int max_point_size;
uniform float point_size_modifier;

vec4 clampColor(vec4 color)
{
	return vec4(clamp(color.r, 0.0f, 1.0f), clamp(color.g, 0.0f, 1.0f), clamp(color.b, 0.0f, 1.0f), clamp(color.a, 0.0f, 1.0f));
}

vec4 getDiffusedColor(vec4 diffuse_color, vec4 light_color)
{
	diffuse_color = clampColor(diffuse_color);
	light_color = clampColor(light_color);

	vec4 absorbed_color = vec4(1.0f) - diffuse_color;

	return clampColor(light_color - absorbed_color);
}

vec4 getReflectedColor(vec4 diffuse_color, vec4 light_pos, vec4 vertex_pos, vec4 light_color, float light_intensity)
{
	vec4 L = light_pos - vertex_pos;
	float distance = length(L);
	float attenuation = (1.0f / (distance * distance)) * illumination_distance;

	attenuation = clamp(max(attenuation, 0.0f), 0.0f, 1.0f);

	vec4 absorption_color = vec4(1.0f) - diffuse_color;
	vec4 reflected_color = light_color - absorption_color;

	reflected_color.r = clamp(reflected_color.r, 0.0f, 1.0f);
	reflected_color.g = clamp(reflected_color.g, 0.0f, 1.0f);
	reflected_color.b = clamp(reflected_color.b, 0.0f, 1.0f);

	reflected_color *= attenuation;
	reflected_color.a = attenuation;

	return reflected_color;
}

vec4 getAmbientReflection(vec4 diffuse_color, vec4 background_color)
{
	float new_r = background_color.r * diffuse_color.r;
	float new_g = background_color.g * diffuse_color.g;
	float new_b = background_color.b * diffuse_color.b;

	vec4 new_color = vec4(new_r, new_g, new_b, 0.0f);

	return new_color * 0.2f;
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

vec4 combineLights(vec4 light_a, vec4 light_b)
{
	vec3 actual_a = vec3(light_a.rgb * light_a.a);
	vec3 actual_b = vec3(light_b.rgb * light_b.a);

	return vec4(max(actual_a.r, actual_b.r), max(actual_a.g, actual_b.g), max(actual_a.b, actual_b.b), 1.0f);
}

float getAttenuation(vec4 scaled_position)
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

	float global_attenuation = getAttenuationFromPosition(illumination_distance, light_position, scaled_position, light_cutoff);

	return clamp(global_attenuation, 0.0f, 1.0f);
}

void main()
{
	vec4 scaled_position = fractal_scale * position;
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

	gl_Position = MVP * scaled_position;

	float alpha_value;
	if (override_line_color_enabled == 1 && geometry_type == 1)
	{
		alpha_value = 1.0f;
		fragment_color = line_override_color;
	}

	else if (override_triangle_color_enabled == 1 && geometry_type == 2)
	{
		alpha_value = 1.0f;
		fragment_color = triangle_override_color;
	}

	else if (override_point_color_enabled == 1 && geometry_type == 0)
	{
		alpha_value = 1.0f;
		fragment_color = point_override_color;
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
		if (lighting_mode < 4)
		{
			float attenuation = getAttenuation(scaled_position);
			fragment_color = getDiffusedColor(fragment_color, (override_light_color_enabled == 1 ? light_override_color : vec4(1.0f)) * attenuation);
			fragment_color += background_color * 0.5f;
			fragment_color = clampColor(fragment_color);
		}

		//dynamic lighting calcs
		else
		{
			vec4 total_light = vec4(0.0f);
			for (int i = 0; i < LIGHT_COUNT; i++)
			{
				if (light_positions[i].w < .001f)
					continue;

				float attenuation = getAttenuationFromPosition(illumination_distance, light_positions[i], scaled_position, light_cutoff);

				if (attenuation <= .001f)
					continue;

				total_light = combineLights(total_light, (override_light_color_enabled == 1 ? light_override_color : light_colors[i]) * attenuation);

				if (total_light.r >= 1.0f && total_light.g >= 1.0f && total_light.b >= 1.0f)
					break;
			}

			vec4 ambient_light = background_color;
			ambient_light.a = 0.5f;

			fragment_color = getDiffusedColor(fragment_color, combineLights(clampColor(total_light), ambient_light));
			fragment_color.a = alpha_value;
		}
	}

	if (render_quadrant > 0)
	{
		gl_Position = quadrant_matrix * gl_Position;
	}

	float distance = length(position - vec4(camera_position, 1.0f));
	gl_PointSize = int(point_size * float(max_point_size) * point_size_modifier * clamp(1.0f / distance, 0.1f, float(max_point_size)));
}