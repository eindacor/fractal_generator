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
uniform vec4 light_positions[LIGHT_COUNT];
uniform vec4 light_colors[LIGHT_COUNT];
uniform int light_count;

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

float getAttenuationFromLight(float illumination_dist, vec4 light_pos, vec4 light_color, vec4 vertex_pos, float cutoff)
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

vec4 getReflectedLight(vec4 diffuse_color, float illumination_dis, vec4 light_pos, vec4 light_color, vec4 vertex_pos, float cutoff)
{
	float attenuation = getAttenuationFromLight(illumination_dis, light_pos, light_color, vertex_pos, cutoff);

	float new_r = light_color.r * diffuse_color.r;
	float new_g = light_color.g * diffuse_color.g;
	float new_b = light_color.b * diffuse_color.b;

	vec4 new_color = vec4(new_r, new_g, new_b, diffuse_color.a);

	return new_color * attenuation;
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

vec4 clampColor(vec4 color)
{
	return vec4(clamp(color.r, 0.0f, 1.0f), clamp(color.g, 0.0f, 1.0f), clamp(color.b, 0.0f, 1.0f), clamp(color.a, 0.0f, 1.0f));
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

	gl_PointSize = point_size * point_size_scale;
	gl_Position = MVP * scaled_position;

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
		if (lighting_mode < 4)
		{
			float attenuation = getAttenuation(scaled_position);
			fragment_color = (fragment_color * attenuation) + (background_color * (1.0f - attenuation));
		}

		else
		{
			float total_attenuation = 0.0f;
			int light_count = 0;
			vec4 final_color = vec4(0.0f);
			for (int i = 0; i < LIGHT_COUNT; i++)
			{
				if (light_positions[i].w < .001f)
					continue;

				final_color += getReflectedLight(fragment_color, illumination_distance, light_positions[i], light_colors[i], scaled_position, light_cutoff);
				//total_attenuation += getAttenuationFromLight(illumination_distance, fractal_scale * light_positions[i], light_colors[i], scaled_position, light_cutoff);
				//fragment_color += getReflectedColor(fragment_color, light_positions[i], scaled_position, light_colors[i], light_colors[i].a);
				//light_count++;
			}
			
			final_color += getAmbientReflection(fragment_color, background_color);
			fragment_color = clampColor(final_color);
			//fragment_color.a = alpha_value;

			//fragment_color += getAmbientReflection(fragment_color, background_color, 0.2f);

			// add one for background color
			//fragment_color /= (float(light_count));

			//total_attenuation = clamp(total_attenuation, 0.0f, 1.0f);
			//fragment_color = (fragment_color * total_attenuation) + (background_color * (1.0f - total_attenuation));
		}
	}

	if (render_quadrant > 0)
	{
		gl_Position = quadrant_matrix * gl_Position;
	}
}