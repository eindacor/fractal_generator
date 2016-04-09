#pragma once

#ifndef MATRIX_CREATOR_H
#define MATRIX_CREATOR_H

#include "header.h"

class matrix_creator
{
public:

	matrix_creator();
	~matrix_creator() { delete in_scope_generator; delete uniform_generator; }

	mat4 getRandomTranslation() const;
	mat4 getRandomTranslation2D() const;
	mat4 getRandomTranslation(const random_switch &x, const random_switch &y, const random_switch &z) const;
	mat4 getRandomTranslation2D(const random_switch &x, const random_switch &y) const;

	mat4 getRandomRotation() const;
	mat4 getRandomRotation2D() const;
	mat4 getRandomRotation(const random_switch &x, const random_switch &y, const random_switch &z, const random_switch &rotation_radians) const;
	mat4 getRandomRotation2D(const random_switch &z, const random_switch &rotation_radians) const;

	mat4 getRandomScale() const;
	mat4 getRandomScale2D() const;
	mat4 getRandomScale(const random_switch &x, const random_switch &y, const random_switch &z) const;
	mat4 getRandomScale2D(const random_switch &x, const random_switch &y) const;

	float getRandomFloat() const;
	float getRandomFloatInRange(const float &min, const float &max) const;
	float getRandomUniform() const;

	boost::variate_generator<boost::mt19937, boost::uniform_real<float> > *in_scope_generator;
	boost::variate_generator<boost::mt19937, boost::uniform_real<float> > *uniform_generator;
};

#endif