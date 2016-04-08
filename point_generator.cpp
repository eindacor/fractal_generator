#include "point_generator.h"

point_generator::point_generator(const int &num_matrices, const int &translate, const int &rotate, const int &scale)
{
	setMatrices(num_matrices, translate, rotate, scale);
}

void point_generator::setMatrices(const int &num_matrices, const int &translate, const int &rotate, const int &scale)
{
	matrices.clear();

	std::map<string, unsigned int> matrix_map;

	matrix_map["translate"] = translate;
	matrix_map["rotate"] = rotate;
	matrix_map["scale"] = scale;

	for (int i = 0; i < num_matrices; i++)
	{
		string matrix_type = jep::catRoll<string>(matrix_map);
		cout << "adding " << matrix_type << " matrix" << endl;

		if (matrix_type == "translate")
			matrices.push_back(mc.getRandomTranslation());

		else if (matrix_type == "rotate")
			matrices.push_back(mc.getRandomRotation());

		else if (matrix_type == "scale")
			matrices.push_back(mc.getRandomScale());

		colors.push_back(vec4(mc.getRandomUniform(), mc.getRandomUniform(), mc.getRandomUniform(), mc.getRandomUniform()));
		sizes.push_back(mc.getRandomUniform() * 5.0);
	}

	for (const auto &matrix : matrices)
	{
		cout << glm::to_string(matrix) << endl;
	}
}

//vector<float> point_generator::getPoints(const vec3 &origin, const int &num_points)
//{
//	vector<float> points;
//
//	int num_matrices = matrices.size();
//
//	for (int i = 0; i < num_points && num_matrices > 0; i++)
//	{
//		vec4 point_color(0.5f, 0.5f, 0.5f, 0.5f);
//		vec4 new_point = vec4(origin, 1.0f);
//		float new_size = 10.0;
//
//		for (int n = 0; n < 10; n++)
//		{
//			int random_index = (int)(mc.getRandomUniform() * num_matrices);
//			mat4 random_matrix = matrices.at(random_index);
//			vec4 matrix_color = colors.at(random_index);
//			float point_size = sizes.at(random_index);
//			new_point = random_matrix * new_point;
//			point_color = (point_color * float(n + 1) + matrix_color) / float(n + 2);
//			new_size = (new_size * float(n + 1) + point_size) / float(n + 2);
//		}
//		
//		if (i < 10)
//			continue;
//
//		points.push_back((float)new_point.x);
//		points.push_back((float)new_point.y);
//		points.push_back((float)new_point.z);
//		points.push_back((float)point_color.r);
//		points.push_back((float)point_color.g);
//		points.push_back((float)point_color.b);
//		points.push_back((float)point_color.a);
//		points.push_back(new_size);
//	}
//
//	return points;
//}

vector<float> point_generator::getPoints(const vector<vec3> &point_sequence, const int &num_points)
{
	vector<float> points;
	points.reserve(num_points * 7);

	int num_matrices = matrices.size();

	if (point_sequence.size() == 0)
		return points;

	for (int n = 0; n < point_sequence.size() && num_matrices > 0; n++)
	{
		vec3 origin = point_sequence.at(n);
		vec4 point_color(0.5f, 0.5f, 0.5f, 0.5f);
		vec4 new_point = vec4(origin, 1.0f);
		float starting_size = 10.0;

		for (int i = 0; i < num_points / point_sequence.size(); i++)
		{
			int random_index = (int)(mc.getRandomUniform() * num_matrices);
			mat4 random_matrix = matrices.at(random_index);
			vec4 matrix_color = colors.at(random_index);
			float matrix_size = sizes.at(random_index);

			if (i < 10)
				continue;

			addNewPoint(new_point, point_color, starting_size, random_index, random_matrix, matrix_color, matrix_size, points);
		}
	}

	return points;
}

vector<float> point_generator::getPoints(const vec3 &origin, const int &num_points)
{
	vector<float> points;
	points.reserve(num_points * 7);

	int num_matrices = matrices.size();

	vec4 point_color(0.5f, 0.5f, 0.5f, 0.5f);
	vec4 new_point = vec4(origin, 1.0f);
	float starting_size = 10.0;

	for (int i = 0; i < num_points && num_matrices > 0; i++)
	{
		int random_index = (int)(mc.getRandomUniform() * num_matrices);
		mat4 random_matrix = matrices.at(random_index);
		vec4 matrix_color = colors.at(random_index);
		float matrix_size = sizes.at(random_index);

		if (i < 10)
			continue;

		addNewPoint(new_point, point_color, starting_size, random_index, random_matrix, matrix_color, matrix_size, points);
	}

	return points;
}

void point_generator::addNewPoint(
	vec4 &starting_point,
	vec4 &starting_color,
	float &starting_size,
	const int &random_index,
	const mat4 &matrix,
	const vec4 &matrix_color,
	const float &point_size,
	vector<float> &points)
{
	starting_point = matrix * starting_point;
	starting_color = (starting_color + matrix_color) / 2.0f;
	starting_size = (starting_size + point_size) / 2.0f;

	points.push_back((float)starting_point.x);
	points.push_back((float)starting_point.y);
	points.push_back((float)starting_point.z);
	points.push_back((float)starting_color.r);
	points.push_back((float)starting_color.g);
	points.push_back((float)starting_color.b);
	points.push_back((float)starting_color.a);
	points.push_back(starting_size);
}