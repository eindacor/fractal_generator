#pragma once

#include "random_generator.h"
#include "color_manager.h"
#include "geometry_generator.h"

class settings_manager
{
public:
	settings_manager() {};
	~settings_manager() {};

	std::string base_seed = "";

	int refresh_value = 5;
#ifdef _DEBUG
	int num_points = 2000;
#else
	int num_points = 10000;
#endif

#ifdef _DEBUG
	int window_width = 400;
	int window_height = 400;
#else
	int window_width = 800;
	int window_height = 800;
#endif
	int background_front_index = 0;
	int background_back_index = 0;
	int generation = 0;
	int refresh_min = 3;
	int refresh_max = 15;
	int translate_weight = 1;
	int rotate_weight = 1;
	int scale_weight = 1;
	int num_matrices = 5;
	int num_lights = 4;
	int point_sequence_index = 0;

	float line_width = 0.1f;
	float interpolation_state = 0.0f;
	float interpolation_increment = 0.02f;
	float alpha_min = 0.0f;
	float alpha_max = 1.0f;
	float fractal_scale = 1.0f;
	float matrix_geometry_coefficient = 0.25f;
	float bias_coefficient = 0.5f;
	float illumination_distance = 0.5f;
	float matrix_geometry_uses_solid_geometry_coefficient = 0.5f;

	bool auto_tracking = false;
	bool use_point_sequence = false;
	bool two_dimensional = false;
	bool refresh_enabled = false;
	bool show_points = true;
	bool enable_triangles = false;
	bool enable_lines = false;
	bool show_palette = false;
	bool smooth_render = true;
	bool randomize_lightness = true;
	bool randomize_alpha = true;
	bool reverse = false;
	bool print_context_on_swap = false;
	bool inverted = false;
	bool scale_matrices = true;
	bool solid_geometry = true;
	bool no_background = false;
	bool light_effects_transparency = false;

	void randomize(const random_generator &mc);

	color_palette palette_front = RANDOM_PALETTE;
	color_palette palette_back = RANDOM_PALETTE;
	color_palette random_palette_front = DEFAULT_COLOR_PALETTE;
	color_palette random_palette_back = DEFAULT_COLOR_PALETTE;
	GLenum line_mode = GL_LINES;
	GLenum triangle_mode = 0;
	std::vector<vec4> point_sequence;
	std::vector<int> line_indices;
	std::vector<int> triangle_indices;
	lighting_mode lm;

	std::map<int, unsigned int> matrix_geometry_weights;

	std::string toString() const;

	geometry_generator gm;

	void setPointSequenceGeometry(int index, const random_generator &rg);

private:
    std::string parseFloat(float f) const;
};