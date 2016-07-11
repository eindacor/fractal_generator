#pragma once

#ifndef FRACTAL_GENERATOR_H
#define FRACTAL_GENERATOR_H

#include "header.h"
#include "matrix_creator.h"
#include "color_manager.h"
#include "settings_manager.h"
#include "geometry_generator.h"

class fractal_generator
{
public:
	fractal_generator(
		const shared_ptr<jep::ogl_context> &con,
		int num_points,
		bool two_dimensional);

	fractal_generator(
		const string &randomization_seed,
		const shared_ptr<jep::ogl_context> &con,
		int num_points,
		bool two_dimensional);

	~fractal_generator() { glDeleteVertexArrays(1, &pg_VAO); glDeleteBuffers(1, &pg_VBO); }

	string getSeed() const { return base_seed; }

	void setMatrices();

	vec4 generateInterpolatedColor(int front_index, int back_index) const;
	float generateInterpolatedSize(int index) const;

	void generateFractal();
	void generateFractalFromPointSequence();
	void generateFractalWithRefresh();
	void generateFractalFromPointSequenceWithRefresh();

	void enableRefreshMode() { refresh_loaded = true; }
	void setRefreshValue(int value) { refresh_value = value; }

	void renderFractal(const int &image_width, const int &image_height, const int &matrix_sequence_count);

	//vector<mat4> generateMatrixSequence(const vector<int> &matrix_indices) const;
	vector<mat4> generateMatrixSequence(const int &sequence_size) const;

	void applyBackground(const int &num_samples);
	void checkKeys(const shared_ptr<key_handler> &keys);
	void drawFractal() const;

	void invertColors();
	void newColors();
	void updateBackground();
	void regenerateFractal();

	vec4 getSampleColor(const int &samples, const vector<vec4> &color_pool) const;
	float getLineWidth() const { return line_width; }

	void printMatrices() const;

	vec4 getBackgroundColor() const { return background_color; }
	void adjustBackgroundBrightness(float adjustment);

	void tickAnimation();
	void swapMatrices();
	void changeDirection();
	void toggleSmooth() { smooth_render = !smooth_render; }
	void cycleColorPalette();
	void loadPointSequence(const vector<vec4> &sequence);
	void printContext();
	void cycleGeometryType();

	vec3 getFocalPoint() const { return focal_point; }
	float getAverageDelta() const { return average_delta; }

	signed int getGeneration() const { return generation; }

	vector<vec4> getColorsFront() const { return colors_front; }
	vector<vec4> getColorsBack() const { return colors_back; }
	float getInterpolationState() const { return interpolation_state; }

	settings_manager getSettings() const { return sm; }

private:
	settings_manager sm;
	string generation_seed;
	vector<unsigned int> matrix_sequence;
	vector< pair<string, mat4> > matrices_front;
	vector< pair<string, mat4> > matrices_back;
	vector<vec4> colors_front;
	vector<vec4> colors_back;
	vec4 background_color;
	vector<float> sizes_front;
	vector<float> sizes_back;
	matrix_creator mc;
	color_manager color_man;
	geometry_generator gm;
	vec3 focal_point;
	float average_delta, max_x, max_y, max_z;
	
	// rendering parameters
	color_palette palette_front = RANDOM_PALETTE;
	color_palette palette_back = RANDOM_PALETTE;
	color_palette random_palette_front = DEFAULT_COLOR_PALETTE;
	color_palette random_palette_back = DEFAULT_COLOR_PALETTE;
	GLenum line_mode = GL_LINES;
	GLenum triangle_mode = 0;
	geometry_type gt = DEFAULT_GEOMETRY_TYPE;

	// all fields below have been moved to settings manager
	float line_width = 1.0f;
	bool size_enabled = true;	
	bool show_points = true;
	bool enable_triangles = false;
	bool enable_lines = false;
	bool show_palette = false;
	float interpolation_state = 0.0f;
	float interpolation_increment = 0.02f;
	bool smooth_render = true;	
	int background_front_index = 0;
	int background_back_index = 0;
	bool randomize_lightness = true;
	bool randomize_alpha = true;
	vec4 seed_color_front;
	vec4 seed_color_back;
	float alpha_min;
	float alpha_max;
	signed int generation = 0;
	bool reverse = false;
	bool print_context_on_swap = false;
	signed int refresh_min = 3;
	signed int refresh_max = 15;
	bool lighting_enabled = false;
	bool use_point_sequence = false;
	bool refresh_loaded = false;
	int refresh_value = 5;
	bool is_2D;
	float fractal_scale = 1.0f;
	int translate_weight;
	int rotate_weight;
	int scale_weight;
	bool inverted = false;
	string base_seed;
	bool scale_matrices_enabled = false;
	bool solid_geometry = true;
	// above fields have been moved to settings manager

	// current gen parameters	
	vec4 origin = vec4(0.0f, 0.0f, 0.0f, 1.0f);	
	vector<vec4> custom_sequence;
	vector<vec4> point_sequence;
	mat4 fractal_scale_matrix;

	bool initialized = false;

	const unsigned short vertex_size = 9;
	int vertex_count;
	int palette_vertex_count;

	GLuint pg_VBO;
	GLuint pg_VAO;

	shared_ptr<ogl_context> context;

	void addNewPointAndIterate(
		vec4 &starting_point,
		vec4 &starting_color,
		float &starting_size,
		int matrix_index,
		vector<float> &points);

	void addPointSequenceAndIterate(
		mat4 &origin_matrix,
		vec4 &starting_color,
		float &starting_size,
		int matrix_index,
		vector<float> &points);

	void addNewPoint(
		const vec4 &point,
		const vec4 &color,
		const float &size,
		vector<float> &points);

	void bufferData(const vector<float> &vertex_data);

	vector< pair<string, mat4> > generateMatrixVector(const int &count) const;
	vector<vec4> generateColorVector(const vec4 &seed, color_palette palette, const int &count, color_palette &random_selection) const;
	vector<float> generateSizeVector(const int &count) const;
	vector<float> getPalettePoints();
	void addDataToPalettePoints(const vec2 &point, const vec4 &color, vector<float> &points) const;
	void addPalettePointsAndBufferData(const vector<float> &vertex_data);
};

#endif