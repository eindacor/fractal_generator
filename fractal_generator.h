#pragma once

#ifndef FRACTAL_GENERATOR_H
#define FRACTAL_GENERATOR_H

#include "header.h"
#include "matrix_creator.h"

class fractal_generator
{
public:
	fractal_generator(
		const string &randomization_seed,
		const shared_ptr<jep::ogl_context> &con, 
		bool two_dimensional = false);

	fractal_generator(
		const string &randomization_seed,
		const shared_ptr<jep::ogl_context> &con, 
		const int &num_matrices, 
		const int &translate, 
		const int &rotate, 
		const int &scale, 
		bool two_dimensional = false);

	fractal_generator(
		const shared_ptr<jep::ogl_context> &con, 
		const int &num_matrices, 
		const int &translate, 
		const int &rotate, 
		const int &scale, 
		bool two_dimensional = false);

	~fractal_generator() { glDeleteVertexArrays(1, &pg_VAO); glDeleteBuffers(1, &pg_VBO); }

	void setMatrices(const int &num_matrices, const int &translate, const int &rotate, const int &scale);

	void generateFractal(const int &num_points);
	void generateFractal(vec4 origin, const int &num_points);
	void generateFractal(const int &num_points, const int &transformation_refresh);
	void generateFractal(vec4 origin, const int &num_points, const int &transformation_refresh);
	void generateFractal(vector<vec4> point_sequence, const int &num_points);
	void generateFractal(vector<vec4> point_sequence, const int &num_points, const int &transformation_refresh);

	vector<mat4> generateMatrixSequence(const vector<int> &matrix_indices) const;
	vector<mat4> generateMatrixSequence(const int &sequence_size) const;

	void setMatrices(const vector<mat4> &new_matrices) { matrices.clear(); matrices = new_matrices; }

	vec4 getSampleColor(const int &samples) const;

	void checkKeys(const shared_ptr<key_handler> &keys);

	void drawFractal() const;

private:
	vector<mat4> matrices;
	vector<vec4> colors;
	vector<float> sizes;
	matrix_creator mc;
	bool is_2D;

	// rendering parameters
	float line_width = 1.0f;
	bool size_enabled = true;
	GLenum line_mode = GL_LINES;
	bool show_points = true;

	float fractal_scale = 1.0f;
	mat4 fractal_scale_matrix;

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
};

#endif