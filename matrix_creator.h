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
	string generateAlphanumericString(int num_chars);

	unsigned int getRandomIntInRange(const unsigned int &min, const unsigned int &max) const;

	void seed(const string &seed_string);

	vector<mat4> getMatricesFromPointSequence(const vector<vec4> &vertices, int count) const;

	template <typename T>
	void shuffleVector(vector<T> &vec) const
	{
		int original_size = vec.size();
		vector<T> shuffled;

		while (shuffled.size() < original_size)
		{
			unsigned int random_index = getRandomIntInRange(0, vec.size());
			shuffled.push_back(vec.at(random_index));
			vec.erase(vec.begin() + random_index);
		}

		vec = shuffled;
	}

	template <typename T>
	bool catRoll(const std::map<T, unsigned int> &m, T &t)
	{
		unsigned int seedRange = 0;

		std::map<T, unsigned int> ranges;

		for (const auto &T_pair : m)
		{
			seedRange += T_pair.second;
			ranges[T_pair.first] = seedRange;
		}

		unsigned int random_number = getRandomIntInRange(0, seedRange);

		for (const auto &T_pair : ranges)
		{
			if (random_number < T_pair.second)
			{
				t = T_pair.first;
				return true;
			}
		}

		return false;
	}

private:
	boost::mt19937 rng;
	boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > *uniform_generator = nullptr;

	float translation_adjustment = 1.0f;
	float scale_min = 0.95f;
	float scale_max = 1.05f;
};

#endif