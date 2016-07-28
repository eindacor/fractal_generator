#pragma once

#include "header.h"
#include "random_generator.h"
#include "color_manager.h"
#include "geometry_generator.h"

class settings_manager
{
public:
	settings_manager() {};
	~settings_manager() {};

	string base_seed = "";

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

	float line_width = 1.0f;
	float interpolation_state = 0.0f;
	float interpolation_increment = 0.02f;
	float alpha_min = 0.0f;
	float alpha_max = 1.0f;
	float fractal_scale = 1.0f;
	float matrix_geometry_coefficient = 0.25f;
	float bias_coefficient = 0.5f;
	float illumination_distance = 0.5f;

	bool auto_tracking = false;
	bool use_point_sequence = false;
	bool two_dimensional = false;
	bool refresh_enabled = false;
	bool size_enabled = true;
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
	bool show_growth = false;
	bool no_background = false;
	bool light_effects_transparency = false;

	void randomize(const random_generator &mc);

	color_palette palette_front = RANDOM_PALETTE;
	color_palette palette_back = RANDOM_PALETTE;
	color_palette random_palette_front = DEFAULT_COLOR_PALETTE;
	color_palette random_palette_back = DEFAULT_COLOR_PALETTE;
	GLenum line_mode = GL_LINES;
	GLenum triangle_mode = 0;
	geometry_type geo_type = GEOMETRY_TYPE_SIZE;
	vector<vec4> point_sequence;
	vector<int> point_indices;
	vector<int> line_indices;
	vector<int> triangle_indices;
	lighting_mode lm;
	attribute_index_method lines_aim = ATTRIBUTE_INDEX_METHOD_SIZE;
	attribute_index_method triangles_aim = ATTRIBUTE_INDEX_METHOD_SIZE;

	std::map<geometry_type, unsigned int> matrix_geometry_weights;

	string toString() const;
	void settings_manager::setWithString(string settings);

	geometry_generator gm;

private:
	string parseFloat(float f) const;
};