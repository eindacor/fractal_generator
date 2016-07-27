#pragma once

#ifndef FRACTAL_GENERATOR_H
#define FRACTAL_GENERATOR_H

#include "header.h"
#include "random_generator.h"
#include "color_manager.h"
#include "settings_manager.h"
#include "geometry_generator.h"

typedef std::pair<GLenum, attribute_index_method> render_style;

#define SEGMENTED_LINES render_style(GL_LINES, POINTS)
#define CONTINUOUS_LINES render_style(GL_LINE_STRIP, POINTS)
#define WIREFRAME_SOLIDS render_style(GL_LINES, LINES)
#define SEGMENTED_SOLIDS render_style(GL_LINES, TRIANGLES)
#define WIREFRAME_CONNECTED render_style(GL_LINE_STRIP, TRIANGLES)

class fractal_generator
{
public:
	fractal_generator(
		const string &randomization_seed,
		const shared_ptr<jep::ogl_context> &con,
		int num_points);

	~fractal_generator() { 
		glDeleteVertexArrays(1, &VAO); 
		glDeleteBuffers(1, &vertices_vbo); 
		glDeleteBuffers(1, &point_indices); 
		glDeleteBuffers(1, &line_indices); 
		glDeleteBuffers(1, &triangle_indices); 
		glDeleteVertexArrays(1, &palette_vao);
		glDeleteBuffers(1, &palette_vbo);
	}

	string getSeed() const { return base_seed; }

	void setMatrices();

	void generateFractal();
	void generateFractalFromPointSequence();
	void generateFractalWithRefresh();
	void generateFractalFromPointSequenceWithRefresh();

	void renderFractal(const int &image_width, const int &image_height, const int &matrix_sequence_count);

	//vector<mat4> generateMatrixSequence(const vector<int> &matrix_indices) const;
	vector<mat4> generateMatrixSequence(const int &sequence_size) const;

	void updateLineColorOverride();
	void applyBackground(const int &num_samples);
	void checkKeys(const shared_ptr<key_handler> &keys);
	void drawFractal() const;
	
	// keeps track of how many indices are called by draw command, set by geometry index pattern generated in geometry_generator.cpp
	int point_index_count;
	int line_index_count;
	int triangle_index_count;

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
	void loadPointSequence(string name, const vector<vec4> &sequence);
	void printContext();
	void cycleGeometryType();
	void cycleBackgroundColorIndex();
	void cycleLineOverride();
	void setBackgroundColorIndex(int index);
	int getBackgroundColorIndex() const { return sm.background_front_index; }

	vec3 getFocalPoint() const { return focal_point; }
	float getAverageDelta() const { return average_delta; }

	signed int getGeneration() const { return sm.generation; }

	vector<vec4> getColorsFront() const { return colors_front; }
	vector<vec4> getColorsBack() const { return colors_back; }
	float getInterpolationState() const { return sm.interpolation_state; }

	settings_manager getSettings() const { return sm; }
	void setGrowth(bool b) { sm.show_growth = b; }
	void setTwoDimensional(bool b) { sm.two_dimensional = b; }
	void setCurrentCustomSequence(int sequence_index) { current_sequence = sequence_index; }
	string getStringFromGeometryType(geometry_type gt) const;

	vector < pair<string, vector<vec4> > > getLoadedSequences() const { return loaded_sequences; }

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
	geometry_type geo_type_front = GEOMETRY_TYPE_SIZE;
	geometry_type geo_type_back = GEOMETRY_TYPE_SIZE;
	random_generator mc;
	color_manager color_man;
	geometry_generator gm;
	vec3 focal_point;
	float average_delta, max_x, max_y, max_z;
	// -3 = no override, -2 = black, -1 = white, 0 - n for each interpolated matrix_color
	int color_override_index = -3;
	
	// current gen parameters	
	vec4 origin = vec4(0.0f, 0.0f, 0.0f, 1.0f);	
	// vector instead of a map to make cycling easy
	vector < pair<string, vector<vec4> > > loaded_sequences;
	int current_sequence = 0;
	mat4 fractal_scale_matrix;

	bool initialized = false;

	const unsigned short vertex_size = 9;
	int vertex_count;
	int palette_vertex_count;

	GLuint vertices_vbo;
	GLuint palette_vbo;
	GLuint point_indices;
	GLuint line_indices;
	GLuint triangle_indices;

	GLuint VAO;
	GLuint palette_vao;

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
		vector<float> &points,
		vector<unsigned short> &point_indices,
		vector<unsigned short> &line_indices,
		vector<unsigned short> &triangle_indices);

	void addNewPoint(
		const vec4 &point,
		const vec4 &color,
		const float &size,
		vector<float> &points);

	void bufferData(const vector<float> &vertex_data, const vector<unsigned short> &point_indices, const vector<unsigned short> &line_indices, const vector<unsigned short> &triangle_indices);
	void bufferPalette(const vector<float> &vertex_data);

	vector< pair<string, mat4> > generateMatrixVector(const int &count, geometry_type &geo_type);
	vector<vec4> generateColorVector(const vec4 &seed, color_palette palette, const int &count, color_palette &random_selection) const;
	vector<float> generateSizeVector(const int &count) const;
	vector<float> getPalettePoints();
	void addDataToPalettePoints(const vec2 &point, const vec4 &color, vector<float> &points) const;
	void addPalettePointsAndBufferData(const vector<float> &vertex_data, const vector<unsigned short> &point_indices, const vector<unsigned short> &line_indices, const vector<unsigned short> &triangle_indices);

	void drawVertices() const;
	void drawLines() const;
	void drawTriangles() const;
	void drawPalette() const;
};

#endif