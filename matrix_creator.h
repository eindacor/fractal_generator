#pragma once

#ifndef MATRIX_CREATOR_H
#define MATRIX_CREATOR_H

#include "header.h"

// TODO refactor class, class doesn't merely create matrices anymore

class matrix_creator
{
public:

	matrix_creator();
	matrix_creator(const string &seed_string);
	~matrix_creator() { delete uniform_generator; }

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

	float getRandomFloat() const { return (*uniform_generator)(); }
	float getRandomFloatInScope() const { return ((*uniform_generator)() * 2.0f) - 1.0f; }
	float getRandomFloatInRange(const float &min, const float &max) const;
	float getRandomUniform() const;
	vec4 getRandomVec4() const { return vec4(getRandomFloat(), getRandomFloat(), getRandomFloat(), 1.0f); }
	vec4 getRandomVec4AlphaClamp(float alpha_min, float alpha_max) const { return vec4(getRandomFloat(), getRandomFloat(), getRandomFloat(), getRandomFloatInRange(alpha_min, alpha_max)); }
	vec4 getRandomVec4FromColorRanges(float r_min, float r_max, float g_min, float g_max, float b_min, float b_max, float a_min, float a_max) const 
		{ return vec4(getRandomFloatInRange(r_min, r_max), getRandomFloatInRange(g_min, g_max), getRandomFloatInRange(b_min, b_max), getRandomFloatInRange(a_min, a_max)); }
	string generateAlphanumericString(int num_chars, bool print_values = false);

	void seed(const string &seed_string);

private:
	boost::mt19937 rng;
	boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > *uniform_generator = nullptr;
};

#endif