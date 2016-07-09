#include "geometry_generator.h"

#define BASE_LENGTH 2.0f * 0.61803398875f
#define HYPOTENUSE_LENGTH 2.0f
#define THETA 1.61803398875f

vector<vec4> geometry_generator::getSquare(float size) const
{
	float half_height = size / 2.0f;

	vector<vec4> point_sequence = {
		vec4(-1.0f * half_height, -1.0f * half_height, 0.0f, 1.0f),
		vec4(-1.0f * half_height, 1.0f * half_height, 0.0f, 1.0f),
		vec4(-1.0f * half_height, 1.0f * half_height, 0.0f, 1.0f),
		vec4(1.0f * half_height, 1.0f * half_height, 0.0f, 1.0f),
		vec4(1.0f * half_height, 1.0f * half_height, 0.0f, 1.0f),
		vec4(1.0f * half_height, -1.0f * half_height, 0.0f, 1.0f),
		vec4(1.0f * half_height, -1.0f * half_height, 0.0f, 1.0f),
		vec4(-1.0f * half_height, -1.0f * half_height, 0.0f, 1.0f)
	};

	return point_sequence;
}

vector<vec4> geometry_generator::getTriangle(float size) const
{
	float half_height = size / 2.0f;

	vector<vec4> point_sequence = {
		vec4(-1.0f * half_height, -1.0f * half_height, 0.0f, 1.0f),
		vec4(0.0f, 1.0f * half_height, 0.0f, 1.0f),
		vec4(0.0f, 1.0f * half_height, 0.0f, 1.0f),
		vec4(1.0f * half_height, 1.0f * half_height, 0.0f, 1.0f),
		vec4(1.0f * half_height, 1.0f * half_height, 0.0f, 1.0f),
		vec4(-1.0f * half_height, -1.0f * half_height, 0.0f, 1.0f),
	};

	return point_sequence;
}

vector<vec4> geometry_generator::getTetrahedron(float size) const
{
	float half_height = size / 2.0f;

	vector<vec4> point_sequence = {
		vec4(-1.0f * half_height, 1.0f * half_height, 1.0f * half_height, 1.0f),
		vec4(-1.0f * half_height, -1.0f * half_height, -1.0f * half_height, 1.0f),
		vec4(1.0f * half_height, -1.0f * half_height, 1.0f * half_height, 1.0f),

		vec4(1.0f * half_height, -1.0f * half_height, 1.0f * half_height, 1.0f),
		vec4(-1.0f * half_height, 1.0f * half_height, 1.0f * half_height, 1.0f),
		vec4(1.0f * half_height, 1.0f * half_height, -1.0f * half_height, 1.0f),

		vec4(1.0f * half_height, 1.0f * half_height, -1.0f * half_height, 1.0f),
		vec4(-1.0f * half_height, 1.0f * half_height, 1.0f * half_height, 1.0f),
		vec4(-1.0f * half_height, -1.0f * half_height, -1.0f * half_height, 1.0f)
	};

	return point_sequence;
}

vector<vec4> geometry_generator::getDodecahedron(float size, bool triangles) const
{
	float theta = 1.61803398875f;

	vector<vec4> unordered_sequence;

	//calcs derived from https://en.wikipedia.org/wiki/Regular_dodecahedron
	for (int i = 0; i < 8; i++)
	{
		float x = i < 4 ? 1.0f : -1.0f;
		float y = i % 4 < 2 ? 1.0f : -1.0f;
		float z = i % 2 == 0 ? 1.0f : -1.0f;

		unordered_sequence.push_back(vec4(x, y, z, 1.0f));
	}

	for (int i = 0; i < 4; i++)
	{
		float x = 0;
		float y = i % 4 < 2 ? 1.0f / theta : -1.0f / theta;
		float z = i % 2 == 0 ? theta : -1.0f * theta;

		unordered_sequence.push_back(vec4(x, y, z, 1.0f));
	}

	for (int i = 0; i < 4; i++)
	{
		float x = i % 4 < 2 ? 1.0f / theta : -1.0f / theta;
		float y = i % 2 == 0 ? theta : -1.0f * theta;
		float z = 0;

		unordered_sequence.push_back(vec4(x, y, z, 1.0f));
	}

	for (int i = 0; i < 4; i++)
	{
		float x = i % 2 == 0 ? theta : -1.0f * theta;
		float y = 0;
		float z = i % 4 < 2 ? 1.0f / theta : -1.0f / theta;

		unordered_sequence.push_back(vec4(x, y, z, 1.0f));
	}

	vector<vec4> point_sequence = orderDodecahedron(unordered_sequence, triangles);

	mat4 scale_matrix = glm::scale(mat4(1.0f), vec3(size, size, size));
	for (vec4 &point : point_sequence)
	{
		point = scale_matrix * point;
	}

	cout << "ordered sequence size: " << point_sequence.size() << endl;

	return point_sequence;
}

vector<vec4> geometry_generator::orderDodecahedron(const vector<vec4> &vertices, bool triangles) const
{
	vector<vec4> ordered_points;
	vector< vector<int> > identified_pentagons;

	for (int i = 0; i < vertices.size(); i++)
	{
		addPentagonsToSequence(i, vertices, identified_pentagons, ordered_points, triangles);
	}

	return ordered_points;
}

void geometry_generator::addPentagonsToSequence(int point_index, const vector<vec4> &vertices, vector< vector<int> > &identified_pentagons, vector<vec4> &sequence, bool triangles) const
{
	float iso_base = 0.61803398875f;
	float hypotenuse = 1.0f;

	int hypotenuse_count = 0;

	for (int i = 0; i < vertices.size() && hypotenuse_count < 6; i++)
	{
		if (i == point_index)
			continue;

		float dist = abs(glm::distance(vertices.at(point_index), vertices.at(i)));

		//identifies pentagon hypotenuses
		if (abs(glm::distance(vertices.at(point_index), vertices.at(i)) - HYPOTENUSE_LENGTH) < .0001f)
		{
			hypotenuse_count++;

			if (pentagonAlreadyIdentified(point_index, i, identified_pentagons))
				continue;

			int side_a = findSideFromTargetAndHypotenuse(point_index, i, vertices);
			int hyp_b = findOtherHypotenuseFromTargetAndHypotenuse(point_index, i, vertices);
			int side_b = findSideFromTargetAndHypotenuse(point_index, hyp_b, vertices);

			identified_pentagons.push_back(vector<int>{point_index, side_a, i, hyp_b, side_b});
			addPentagonToPointSequence(point_index, side_a, i, hyp_b, side_b, vertices, sequence, triangles);
		}

		//find a hypotenuse point -> hyp_a
		//are these two points part of any identified pentagon?
		//	yes -> invalid match
		//find other hypotenuse point (base dist away from hyp_a) -> hyp_b
		//find side_a (base away from target, base away from hyp_a) -> side_a
		//find side_b (base away from target, base away from hyp_b) -> side_b
		//add triangles
		//	target, side_a, hyp_a
		//	target, hyp_a, hyp_b
		//	target, hyp_b, side_b
		//add to identified pentagons
		//	target, side_a, hyp_a, hyp_b, side_b
	}
}

bool geometry_generator::pentagonAlreadyIdentified(int index_a, int index_b, vector< vector<int> > &identified_pentagons) const
{
	for (const vector<int> &pentagon : identified_pentagons)
	{
		if (std::find(pentagon.cbegin(), pentagon.cend(), index_a) != pentagon.cend() && std::find(pentagon.cbegin(), pentagon.cend(), index_b) != pentagon.cend())
			return true;
	}

	return false;
}

int geometry_generator::findSideFromTargetAndHypotenuse(int target, int hypotenuse, const vector<vec4> &vertices) const
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if (i == target || i == hypotenuse)
			continue;

		if (abs(glm::distance(vertices.at(i), vertices.at(target)) - BASE_LENGTH) < .0001f && abs(glm::distance(vertices.at(i), vertices.at(hypotenuse)) - BASE_LENGTH) < .0001f)
			return i;
	}

	return 0;
}

int geometry_generator::findOtherHypotenuseFromTargetAndHypotenuse(int target, int hypotenuse, const vector<vec4> &vertices) const
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if (i == target || i == hypotenuse)
			continue;

		if (abs(glm::distance(vertices.at(i), vertices.at(target)) - HYPOTENUSE_LENGTH) < .0001f && abs(glm::distance(vertices.at(i), vertices.at(hypotenuse)) - BASE_LENGTH) < .0001f)
			return i;
	}

	return 0;
}

void geometry_generator::addPentagonToPointSequence(int a, int b, int c, int d, int e, const vector<vec4> vertices, vector<vec4> &sequence, bool triangles) const
{
	if (triangles)
	{
		sequence.push_back(vertices.at(a));
		sequence.push_back(vertices.at(b));
		sequence.push_back(vertices.at(c));

		sequence.push_back(vertices.at(a));
		sequence.push_back(vertices.at(c));
		sequence.push_back(vertices.at(d));

		sequence.push_back(vertices.at(a));
		sequence.push_back(vertices.at(d));
		sequence.push_back(vertices.at(e));
	}

	else
	{
		sequence.push_back(vertices.at(a));
		sequence.push_back(vertices.at(b));
		sequence.push_back(vertices.at(c));
		sequence.push_back(vertices.at(d));
		sequence.push_back(vertices.at(e));
	}
}