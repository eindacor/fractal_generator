#include "matrix_creator.h"

matrix_creator::matrix_creator()
{
	boost::mt19937 rng;
	rng.seed(clock());
	boost::uniform_real<float> random_in_scope(-1.0f, 1.0f);
	in_scope_generator = new boost::variate_generator<boost::mt19937, boost::uniform_real<float> >(rng, random_in_scope);

	boost::uniform_real<float> random(0.0f, 1.0f);
	uniform_generator = new boost::variate_generator<boost::mt19937, boost::uniform_real<float> >(rng, random);
}

mat4 matrix_creator::getRandomTranslation() const
{
	return glm::translate(mat4(1.0f), vec3(getRandomFloat(), getRandomFloat(), getRandomFloat()));
}

mat4 matrix_creator::getRandomTranslation(const random_switch &x, const random_switch &y, const random_switch &z) const
{
	float generated_x = x.first ? x.second : getRandomFloat();
	float generated_y = y.first ? y.second : getRandomFloat();
	float generated_z = z.first ? z.second : getRandomFloat();

	return glm::translate(mat4(1.0f), vec3(generated_x, generated_y, generated_z));
}

mat4 matrix_creator::getRandomRotation() const
{
	return glm::rotate(mat4(1.0f), rand() * 6.28318530718f, vec3(getRandomFloat(), getRandomFloat(), getRandomFloat()));
}

mat4 matrix_creator::getRandomRotation(const random_switch &x, const random_switch &y, const random_switch &z, const random_switch &rotation_radians) const
{
	float generated_x = x.first ? x.second : getRandomFloat();
	float generated_y = y.first ? y.second : getRandomFloat();
	float generated_z = z.first ? z.second : getRandomFloat();
	float generated_radians = rotation_radians.first ? rotation_radians.second : getRandomFloatInRange(0.0f, 6.28318530718f);

	return glm::rotate(mat4(1.0f), generated_radians, vec3(generated_x, generated_y, generated_z));
}

mat4 matrix_creator::getRandomScale() const
{
	return glm::scale(mat4(1.0f), vec3(getRandomFloat(), getRandomFloat(), getRandomFloat()));
}

mat4 matrix_creator::getRandomScale(const random_switch &x, const random_switch &y, const random_switch &z) const
{
	float generated_x = x.first ? x.second : getRandomFloat();
	float generated_y = y.first ? y.second : getRandomFloat();
	float generated_z = z.first ? z.second : getRandomFloat();

	return glm::scale(mat4(1.0f), vec3(generated_x, generated_y, generated_z));
}

float matrix_creator::getRandomFloat() const 
{ 
	return (*in_scope_generator)();
}

float matrix_creator::getRandomFloatInRange(const float &min, const float &max) const 
{ 
	if (max < min)
		return 0.0f;

	float difference = max - min;

	return min + ((*uniform_generator)() * max);
}