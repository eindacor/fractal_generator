#pragma once

#ifndef POINT_GENERATOR_H
#define POINT_GENERATOR_H

#include "header.h"
#include "matrix_creator.h"

class point_generator
{
public:
	point_generator(const int &num_matrices, const int &translate, const int &rotate, const int &scale);
	~point_generator() {};

	vector<float> getPoints(const vec3 &origin, const int &num_points);
	vector<float> getPoints(const vector<vec3> &point_sequence, const int &num_points);
	void setMatrices(const int &num_matrices, const int &translate, const int &rotate, const int &scale);

	vector<mat4> generateMatrixSequence(const vector<int> &matrix_indices) const;
	vector<mat4> generateMatrixSequence(const int &sequence_size) const;

	void setMatrices(const vector<mat4> &new_matrices) { matrices.clear(); matrices = new_matrices; }

private:
	vector<mat4> matrices;
	vector<vec4> colors;
	vector<float> sizes;
	matrix_creator mc;

	void addNewPoint(
		vec4 &starting_point, 
		vec4 &starting_color, 
		float &starting_size,
		const int &random_index, 
		const mat4 &matrix, 
		const vec4 &matrix_color, 
		const float &point_size,
		vector<float> &points);
};

#endif