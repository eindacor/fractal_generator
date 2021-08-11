#pragma once

#include "header.h"

// TODO refactor class, class doesn't merely create matrices anymore

enum nonlinear_transformation { SINUSOIDAL, SPHERICAL, SWIRL, HORSESHOE, NONLINEAR_TRANSFORMATION_SIZE };

class random_generator
{
public:

	random_generator();
	random_generator(const std::string &seed_string);
	~random_generator() { delete uniform_generator; }

	glm::mat4 getRandomTranslation() const;
	glm::mat4 getRandomTranslation2D() const;
	glm::mat4 getRandomTranslation(const random_switch &x, const random_switch &y, const random_switch &z) const;
	glm::mat4 getRandomTranslation2D(const random_switch &x, const random_switch &y) const;

	glm::mat4 getRandomRotation() const;
	glm::mat4 getRandomRotation2D() const;
	glm::mat4 getRandomRotation(const random_switch &x, const random_switch &y, const random_switch &z, const random_switch &rotation_radians) const;
	glm::mat4 getRandomRotation2D(const random_switch &z, const random_switch &rotation_radians) const;

	glm::mat4 getRandomScale() const;
	glm::mat4 getRandomScale2D() const;
	glm::mat4 getRandomScale(const random_switch &x, const random_switch &y, const random_switch &z) const;
	glm::mat4 getRandomScale2D(const random_switch &x, const random_switch &y) const;

	float getRandomFloat() const { return (*uniform_generator)(); }
	float getRandomFloatInScope() const { return ((*uniform_generator)() * 2.0f) - 1.0f; }
	float getRandomFloatInRange(const float &min, const float &max) const;
	float getRandomUniform() const;
	glm::vec4 getRandomVec4() const { return glm::vec4(getRandomFloat(), getRandomFloat(), getRandomFloat(), 1.0f); }
	glm::vec4 getRandomVec4AlphaClamp(float alpha_min, float alpha_max) const { return glm::vec4(getRandomFloat(), getRandomFloat(), getRandomFloat(), getRandomFloatInRange(alpha_min, alpha_max)); }
	glm::vec4 getRandomVec4FromColorRanges(float r_min, float r_max, float g_min, float g_max, float b_min, float b_max, float a_min, float a_max) const
	{
	    return glm::vec4(getRandomFloatInRange(r_min, r_max),
                         getRandomFloatInRange(g_min, g_max),
                         getRandomFloatInRange(b_min, b_max),
                         getRandomFloatInRange(a_min, a_max));
	}

	string generateAlphanumericString(int num_chars);

	unsigned int getRandomIntInRange(const unsigned int &min, const unsigned int &max) const;
	bool getBool(const float &odds) const { return getRandomFloat() < odds; }

	void seed(const string &seed_string);

	std::vector<glm::mat4> getMatricesFromPointSequence(const std::vector<glm::vec4> &vertices, int count) const;

	template <typename T>
	void shuffleVector(std::vector<T> &vec) const
	{
		int original_size = vec.size();
		std::vector<T> shuffled;

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

	glm::vec4 getSinusoidal(const glm::vec4 &vertex) const { return vec4(sin(vertex.x), sin(vertex.y), sin(vertex.z), vertex.w); }
	glm::vec4 getSpherical(const glm::vec4 &vertex) const { return glm::vec4(1.0f / pow(getR(vertex), 2)) * vertex; }
	glm::vec4 getSwirl(const glm::vec4 &vertex) const;
	glm::vec4 getHorseShoe(const glm::vec4 &vertex) const;

	glm::vec4 getNonlinear(nonlinear_transformation nt, const vec4 &vertex) const;

private:
	boost::mt19937 rng;
	boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > *uniform_generator = nullptr;
	float getR(const vec4 &vertex) const { return sqrt(pow(vertex.x, 2) + pow(vertex.y, 2) + pow(vertex.z, 2)); }

	float translation_adjustment = 1.0f;
	float scale_min = 0.95f;
	float scale_max = 1.05f;
};