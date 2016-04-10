#include "matrix_creator.h"

matrix_creator::matrix_creator()
{
	rng.seed(clock());
	boost::uniform_real<> random_range(0, 1);
	uniform_generator = new boost::variate_generator<boost::mt19937&, boost::uniform_real<> >(rng, random_range);
}

matrix_creator::matrix_creator(const string &seed_string)
{
	seed(seed_string);
}

mat4 matrix_creator::getRandomTranslation() const
{
	return glm::translate(mat4(1.0f), vec3(getRandomFloat(), getRandomFloat(), getRandomFloat()));
}

mat4 matrix_creator::getRandomTranslation2D() const
{
	return glm::translate(mat4(1.0f), vec3(getRandomFloat(), getRandomFloat(), 0.0f));
}

mat4 matrix_creator::getRandomTranslation(const random_switch &x, const random_switch &y, const random_switch &z) const
{
	float generated_x = x.first ? x.second : getRandomFloat();
	float generated_y = y.first ? y.second : getRandomFloat();
	float generated_z = z.first ? z.second : getRandomFloat();

	return glm::translate(mat4(1.0f), vec3(generated_x, generated_y, generated_z));
}

mat4 matrix_creator::getRandomTranslation2D(const random_switch &x, const random_switch &y) const
{
	float generated_x = x.first ? x.second : getRandomFloat();
	float generated_y = y.first ? y.second : getRandomFloat();

	return glm::translate(mat4(1.0f), vec3(generated_x, generated_y, 0.0f));
}

mat4 matrix_creator::getRandomRotation() const
{
	return glm::rotate(mat4(1.0f), getRandomUniform() * 6.28318530718f, vec3(getRandomFloat(), getRandomFloat(), getRandomFloat()));
}

mat4 matrix_creator::getRandomRotation2D() const
{
	return glm::rotate(mat4(1.0f), getRandomUniform() * 6.28318530718f, vec3(0.0f, 0.0f, 1.0f));
}

mat4 matrix_creator::getRandomRotation(const random_switch &x, const random_switch &y, const random_switch &z, const random_switch &rotation_radians) const
{
	float generated_x = x.first ? x.second : getRandomFloat();
	float generated_y = y.first ? y.second : getRandomFloat();
	float generated_z = z.first ? z.second : getRandomFloat();
	float generated_radians = rotation_radians.first ? rotation_radians.second : getRandomFloatInRange(0.0f, 6.28318530718f);

	return glm::rotate(mat4(1.0f), generated_radians, vec3(generated_x, generated_y, generated_z));
}

mat4 matrix_creator::getRandomRotation2D(const random_switch &z, const random_switch &rotation_radians) const
{
	float generated_z = z.first ? z.second : getRandomFloat();
	float generated_radians = rotation_radians.first ? rotation_radians.second : getRandomFloatInRange(0.0f, 6.28318530718f);

	return glm::rotate(mat4(1.0f), generated_radians, vec3(0.0f, 0.0f, generated_z));
}

mat4 matrix_creator::getRandomScale() const
{
	return glm::scale(mat4(1.0f), vec3(getRandomFloat(), getRandomFloat(), getRandomFloat()));
}

mat4 matrix_creator::getRandomScale2D() const
{
	return glm::scale(mat4(1.0f), vec3(getRandomFloat(), getRandomFloat(), 1.0f));
}

mat4 matrix_creator::getRandomScale(const random_switch &x, const random_switch &y, const random_switch &z) const
{
	float generated_x = x.first ? x.second : getRandomFloat();
	float generated_y = y.first ? y.second : getRandomFloat();
	float generated_z = z.first ? z.second : getRandomFloat();

	return glm::scale(mat4(1.0f), vec3(generated_x, generated_y, generated_z));
}

mat4 matrix_creator::getRandomScale2D(const random_switch &x, const random_switch &y) const
{
	float generated_x = x.first ? x.second : getRandomFloat();
	float generated_y = y.first ? y.second : getRandomFloat();

	return glm::scale(mat4(1.0f), vec3(generated_x, generated_y, 1.0f));
}

float matrix_creator::getRandomFloat() const 
{ 
	return ((*uniform_generator)() * 2.0) - 1.0f;
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
	boost::uniform_real<> random_range(0, 1);
	uniform_generator = new boost::variate_generator<boost::mt19937&, boost::uniform_real<> >(rng, random_range);
}

string matrix_creator::generateAlphanumericString(int num_chars, bool print_values)
{
	string random_string;

	const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

	const size_t set_size = sizeof(charset) - 1;

	for (int i = 0; i < num_chars; i++)
	{
		unsigned short random_index = getRandomUniform() * (float)set_size;
		random_string += charset[random_index];

		if (print_values)
			cout << random_index << " ";
	}

	if (print_values)
		cout << endl;

	return random_string;
}