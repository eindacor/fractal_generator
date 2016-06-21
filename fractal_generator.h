#pragma once

#ifndef FRACTAL_GENERATOR_H
#define FRACTAL_GENERATOR_H

#include "header.h"
#include "matrix_creator.h"
#include "color_manager.h"

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

	fractal_generator(
		const string &randomization_seed,
		const shared_ptr<jep::ogl_context> &con, 
		const int &num_matrices, 
		const int &translate, 
		const int &rotate, 
		const int &scale, 
		int num_points,
		bool two_dimensional);

	fractal_generator(
		const shared_ptr<jep::ogl_context> &con, 
		const int &num_matrices, 
		const int &translate, 
		const int &rotate, 
		const int &scale, 
		int num_points,
		bool two_dimensional);

	~fractal_generator() { glDeleteVertexArrays(1, &pg_VAO); glDeleteBuffers(1, &pg_VBO); }

	string getSeed() const { return seed; }

	void setMatrices(const int &num_matrices);

	mat4 generateInterpolatedMatrix(int index) const;
	vec4 generateInterpolatedColor(int front_index, int back_index) const;
	float generateInterpolatedSize(int index) const;

	void generateFractal();
	void generateFractalFromPointSequence();
	void generateFractalWithRefresh();
	void generateFractalFromPointSequenceWithRefresh();

	void renderFractal(const int &image_width, const int &image_height, const int &matrix_sequence_count);

	//vector<mat4> generateMatrixSequence(const vector<int> &matrix_indices) const;
	vector<mat4> generateMatrixSequence(const int &sequence_size) const;

	void applyBackground(const int &num_samples);
	void checkKeys(const shared_ptr<key_handler> &keys);
	void drawFractal() const;

	void invertColor(vec4 &original);
	void invertColors();
	void fractal_generator::newColors();
	void regenerateFractal();

	vec4 getSampleColor(const int &samples, const vector<vec4> &color_pool) const;
	float getLineWidth() const { return line_width; }

	void printMatrices() const;

	vec4 getBackgroundColor() const { return background_color; }
	void adjustBackgroundBrightness(float adjustment);

	void tickAnimation();
	void swapMatrices();

	void toggleSmooth() { smooth_render = !smooth_render; }

	void cycleColorPalette();

	void printContext();

	vec3 getFocalPoint() const { return focal_point; }
	float getAverageDelta() const { return average_delta; }

private:
	string seed;
	vector< pair<string, mat4> > matrices_front;
	vector< pair<string, mat4> > matrices_back;
	vector<vec4> colors_front;
	vector<vec4> colors_back;
	vec4 background_color;
	bool inverted = false;
	vector<float> sizes_front;
	vector<float> sizes_back;
	matrix_creator mc;
	matrix_creator mc_persistent_seed;
	color_manager color_man;
	vec3 focal_point;
	float average_delta;

	vector<int> random_matrix_order;

	//weights determine probability of each matrix type
	int translate_weight;
	int rotate_weight;
	int scale_weight;

	// rendering parameters
	float line_width = 1.0f;
	bool size_enabled = true;
	GLenum line_mode = GL_LINES;
	GLenum triangle_mode = 0;
	bool show_points = true;
	bool enable_triangles = false;
	bool enable_lines = false;
	float interpolation_state = 0.0f;
	float interpolation_increment = 0.02f;
	bool front_buffer_first = true;
	bool smooth_render = true;
	color_palette palette_front = DEFAULT_COLOR_PALETTE;
	color_palette palette_back = DEFAULT_COLOR_PALETTE;
	color_palette random_palette_front = DEFAULT_COLOR_PALETTE;
	color_palette random_palette_back = DEFAULT_COLOR_PALETTE;
	int background_front_index = 0;
	int background_back_index = 0;
	bool randomize_lightness = true;
	bool randomize_alpha = true;
	vec4 seed_color_front;
	vec4 seed_color_back;
	float alpha_min;
	float alpha_max;

	// current gen parameters
	bool refresh_loaded;
	int refresh_value;
	bool is_2D;
	vec4 origin = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	bool sequence_loaded;
	vector<vec4> preloaded_sequence;

	float fractal_scale = 1.0f;
	mat4 fractal_scale_matrix;
	unsigned short discard_count = 50;

	bool initialized = false;

	unsigned short vertex_size = 9;
	int vertex_count;

	GLuint pg_VBO;
	GLuint pg_VAO;

	shared_ptr<ogl_context> context;

	void addNewPointAndIterate(
		vec4 &starting_point,
		vec4 &starting_color,
		float &starting_size,
		const mat4 &matrix,
		const vec4 &matrix_color,
		const float &point_size,
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
};

#endif