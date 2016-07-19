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
		const string &randomization_seed,
		const shared_ptr<jep::ogl_context> &con,
		int num_points);

	~fractal_generator() { glDeleteVertexArrays(1, &pg_VAO); glDeleteBuffers(1, &pg_VBO); }

	string getSeed() const { return base_seed; }

	void setMatrices();

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

	void invertColors();
	void newColors();
	void updateBackground();
	void regenerateFractal();

	vec4 getSampleColor(const int &samples, const vector<vec4> &color_pool) const;
	float getLineWidth() const { return sm.line_width; }

	void printMatrices() const;

	vec4 getBackgroundColor() const { return background_color; }
	void adjustBackgroundBrightness(float adjustment);

	void tickAnimation();
	void swapMatrices();
	void cycleColorPalette();
	void loadPointSequence(const vector<vec4> &sequence);
	void printContext();
	void cycleGeometryType();

	vec3 getFocalPoint() const { return focal_point; }
	float getAverageDelta() const { return average_delta; }

	signed int getGeneration() const { return sm.generation; }

	vector<vec4> getColorsFront() const { return colors_front; }
	vector<vec4> getColorsBack() const { return colors_back; }
	float getInterpolationState() const { return sm.interpolation_state; }

	settings_manager getSettings() const { return sm; }
	void setGrowth(bool b) { sm.show_growth = b; }
	void setTwoDimensional(bool b) { sm.two_dimensional = b; }

private:
	settings_manager sm;
	string base_seed;
	string generation_seed;
	vector<unsigned int> matrix_sequence_front;
	vector<unsigned int> matrix_sequence_back;
	vector< pair<string, mat4> > matrices_front;
	vector< pair<string, mat4> > matrices_back;
	vector<vec4> colors_front;
	vector<vec4> colors_back;
	vec4 background_color;
	vec4 seed_color_front;
	vec4 seed_color_back;
	vector<float> sizes_front;
	vector<float> sizes_back;
	geometry_type geo_type_front = DEFAULT_GEOMETRY_TYPE;
	geometry_type geo_type_back = DEFAULT_GEOMETRY_TYPE;
	matrix_creator mc;
	color_manager color_man;
	geometry_generator gm;
	vec3 focal_point;
	float average_delta, max_x, max_y, max_z;
	
	// current gen parameters	
	vec4 origin = vec4(0.0f, 0.0f, 0.0f, 1.0f);	
	vector<vec4> custom_sequence;
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
		int matrix_index_front,
		int matrix_index_back,
		vector<float> &points);

	void addPointSequenceAndIterate(
		mat4 &origin_matrix,
		vec4 &starting_color,
		float &starting_size,
		int matrix_index_front,
		int matrix_index_back,
		vector<float> &points);

	void addNewPoint(
		const vec4 &point,
		const vec4 &color,
		const float &size,
		vector<float> &points);

	void bufferData(const vector<float> &vertex_data);

	vector< pair<string, mat4> > generateMatrixVector(const int &count, geometry_type &geo_type);
	vector<vec4> generateColorVector(const vec4 &seed, color_palette palette, const int &count, color_palette &random_selection) const;
	vector<float> generateSizeVector(const int &count) const;
	vector<float> getPalettePoints();
	void addDataToPalettePoints(const vec2 &point, const vec4 &color, vector<float> &points) const;
	void addPalettePointsAndBufferData(const vector<float> &vertex_data);
};

#endif