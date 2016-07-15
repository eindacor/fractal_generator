#include "matrix_creator.h"

matrix_creator::matrix_creator()
{
	std::chrono::time_point<std::chrono::system_clock> seed = std::chrono::system_clock::now();
	rng.seed(seed.time_since_epoch().count());
	boost::uniform_real<float> random_range(0, 1);
	uniform_generator = new boost::variate_generator<boost::mt19937&, boost::uniform_real<float> >(rng, random_range);
}

matrix_creator::matrix_creator(const string &seed_string)
{
	seed(seed_string);
}

mat4 matrix_creator::getRandomTranslation() const
{
	return glm::translate(mat4(1.0f), vec3(getRandomFloatInScope() * translation_adjustment, getRandomFloatInScope() * translation_adjustment, getRandomFloatInScope() * translation_adjustment));
}

mat4 matrix_creator::getRandomTranslation2D() const
{
	return glm::translate(mat4(1.0f), vec3(getRandomFloatInScope() * translation_adjustment, getRandomFloatInScope() * translation_adjustment, 0.0f));
}

mat4 matrix_creator::getRandomTranslation(const random_switch &x, const random_switch &y, const random_switch &z) const
{
	float generated_x = x.first ? x.second : getRandomFloatInScope() * translation_adjustment;
	float generated_y = y.first ? y.second : getRandomFloatInScope() * translation_adjustment;
	float generated_z = z.first ? z.second : getRandomFloatInScope() * translation_adjustment;

	return glm::translate(mat4(1.0f), vec3(generated_x, generated_y, generated_z));
}

mat4 matrix_creator::getRandomTranslation2D(const random_switch &x, const random_switch &y) const
{
	float generated_x = x.first ? x.second : getRandomFloatInScope() * translation_adjustment;
	float generated_y = y.first ? y.second : getRandomFloatInScope() * translation_adjustment;

	return glm::translate(mat4(1.0f), vec3(generated_x, generated_y, 0.0f));
}

mat4 matrix_creator::getRandomRotation() const
{
	return glm::rotate(mat4(1.0f), getRandomUniform() * 6.28318530718f, vec3(getRandomFloatInScope(), getRandomFloatInScope(), getRandomFloatInScope()));
}

mat4 matrix_creator::getRandomRotation2D() const
{
	return glm::rotate(mat4(1.0f), getRandomUniform() * 6.28318530718f, vec3(0.0f, 0.0f, 1.0f));
}

mat4 matrix_creator::getRandomRotation(const random_switch &x, const random_switch &y, const random_switch &z, const random_switch &rotation_radians) const
{
	float generated_x = x.first ? x.second : getRandomFloatInScope();
	float generated_y = y.first ? y.second : getRandomFloatInScope();
	float generated_z = z.first ? z.second : getRandomFloatInScope();
	float generated_radians = rotation_radians.first ? rotation_radians.second : getRandomFloatInRange(0.0f, 6.28318530718f);

	return glm::rotate(mat4(1.0f), generated_radians, vec3(generated_x, generated_y, generated_z));
}

mat4 matrix_creator::getRandomRotation2D(const random_switch &z, const random_switch &rotation_radians) const
{
	float generated_z = z.first ? z.second : getRandomFloatInScope();
	float generated_radians = rotation_radians.first ? rotation_radians.second : getRandomFloatInRange(0.0f, 6.28318530718f);

	return glm::rotate(mat4(1.0f), generated_radians, vec3(0.0f, 0.0f, generated_z));
}

mat4 matrix_creator::getRandomScale() const
{
	vec3 scale_vector = vec3(getRandomFloatInRange(scale_min, scale_max), getRandomFloatInRange(scale_min, scale_max), getRandomFloatInRange(scale_min, scale_max));

	if (getRandomFloat() < 0.5f)
		scale_vector.x *= -1.0f;

	if (getRandomFloat() < 0.5f)
		scale_vector.y *= -1.0f;

	if (getRandomFloat() < 0.5f)
		scale_vector.z *= -1.0f;

	return glm::scale(mat4(1.0f), scale_vector);
}

mat4 matrix_creator::getRandomScale2D() const
{
	vec3 scale_vector = vec3(getRandomFloatInRange(scale_min, scale_max), getRandomFloatInRange(scale_min, scale_max), 1.0f);

	if (getRandomFloat() < 0.5f)
		scale_vector.x *= -1.0f;

	if (getRandomFloat() < 0.5f)
		scale_vector.y *= -1.0f;

	return glm::scale(mat4(1.0f), scale_vector);
}

mat4 matrix_creator::getRandomScale(const random_switch &x, const random_switch &y, const random_switch &z) const
{
	float generated_x = x.first ? x.second : getRandomFloatInRange(scale_min, scale_max);
	float generated_y = y.first ? y.second : getRandomFloatInRange(scale_min, scale_max);
	float generated_z = z.first ? z.second : getRandomFloatInRange(scale_min, scale_max);

	if (!x.first && getRandomFloat() < 0.5f)
		generated_x *= -1.0f;

	if (!y.first && getRandomFloat() < 0.5f)
		generated_y *= -1.0f;

	if (!z.first && getRandomFloat() < 0.5f)
		generated_z *= -1.0f;

	return glm::scale(mat4(1.0f), vec3(generated_x, generated_y, generated_z));
}

mat4 matrix_creator::getRandomScale2D(const random_switch &x, const random_switch &y) const
{
	float generated_x = x.first ? x.second : getRandomFloatInRange(scale_min, scale_max);
	float generated_y = y.first ? y.second : getRandomFloatInRange(scale_min, scale_max);

	if (!x.first && getRandomFloat() < 0.5f)
		generated_x *= -1.0f;

	if (!y.first && getRandomFloat() < 0.5f)
		generated_y *= -1.0f;

	return glm::scale(mat4(1.0f), vec3(generated_x, generated_y, 1.0f));
}

float matrix_creator::getRandomFloatInRange(const float &min, const float &max) const 
{ 
	if (max < min)
		return 0.0f;

	float difference = max - min;

	return min + ((*uniform_generator)() * difference);
}

float matrix_creator::getRandomUniform() const
{
	return (*uniform_generator)();
}

void matrix_creator::seed(const string &seed_string)
{
	if (nullptr != uniform_generator)
		delete uniform_generator;

	boost::hash<std::string> string_hash;
	unsigned int hashed_string = string_hash(seed_string);

	rng.seed(hashed_string);
	boost::uniform_real<float> random_range(0, 1);
	uniform_generator = new boost::variate_generator<boost::mt19937&, boost::uniform_real<float> >(rng, random_range);
}

string matrix_creator::generateAlphanumericString(int num_chars)
{
	string random_string;

	const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

	const size_t set_size = sizeof(charset) - 1;

	for (int i = 0; i < num_chars; i++)
	{
		unsigned short random_index = getRandomUniform() * (float)set_size;
		random_string += charset[random_index];
	}

	return random_string;
}

vector<mat4> matrix_creator::getMatricesFromPointSequence(const vector<vec4> &vertices) const
{
	vector<mat4> matrices;

	for (const vec4 vertex : vertices)
	{
		matrices.push_back(glm::translate(mat4(1.0f), vec3(vertex)));
	}

	return matrices;
}