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
	void setMatrices(const int &num_matrices, const int &translate, const int &rotate, const int &scale);

private:
	vector<mat4> matrices;
	vector<vec4> colors;
	vector<float> sizes;
	matrix_creator mc;
};

#endif