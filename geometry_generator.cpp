#include "geometry_generator.h"

string getStringFromGeometryType(geometry_type gt)
{
	switch (gt)
	{
	case TRIANGLE: return "triangle";
	case RECTANGLE: return "rectangle";
	case SQUARE: return "square";
	case CUBOID: return "cuboid";
	case CUBE: return "cube";
	case TETRAHEDRON: return "tetrahedron";
	case DODECAHEDRON: return "dodecahedron";
	case LOADED_SEQUENCE: return "custom_sequence";
	case DEFAULT_GEOMETRY_TYPE: return "points";
	default: return "unknown type";
	}
}

vector<vec4> geometry_generator::getRectangle(float width, float height) const
{
	float half_width = width / 2.0f;
	float half_height = height / 2.0f;

	vector<vec4> point_sequence;

	if (export_as_triangles)
	{
		point_sequence.push_back(vec4(-1.0f * half_width, -1.0f * half_height, 0.0f, 1.0f));
		point_sequence.push_back(vec4(-1.0f * half_width, 1.0f * half_height, 0.0f, 1.0f));
		point_sequence.push_back(vec4(1.0f * half_width, 1.0f * half_height, 0.0f, 1.0f));

		point_sequence.push_back(vec4(1.0f * half_width, 1.0f * half_height, 0.0f, 1.0f));
		point_sequence.push_back(vec4(1.0f * half_width, -1.0f * half_height, 0.0f, 1.0f));
		point_sequence.push_back(vec4(-1.0f * half_width, -1.0f * half_height, 0.0f, 1.0f));
	}

	else
	{
		point_sequence.push_back(vec4(-1.0f * half_width, -1.0f * half_height, 0.0f, 1.0f));
		point_sequence.push_back(vec4(-1.0f * half_width, 1.0f * half_height, 0.0f, 1.0f));
		point_sequence.push_back(vec4(1.0f * half_width, 1.0f * half_height, 0.0f, 1.0f));
		point_sequence.push_back(vec4(1.0f * half_width, -1.0f * half_height, 0.0f, 1.0f));
	}

	return point_sequence;
}

vector<vec4> geometry_generator::getCuboid(float width, float height, float depth) const
{
	float half_width = width / 2.0f;
	float half_height = height / 2.0f;
	float half_depth = depth / 2.0f;

	vector<vec4> point_sequence;

	if (export_as_triangles)
	{
		vector<vec4> front = getRectangle(width, height);
		for (vec4 &point : front)
		{
			mat4 move = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, half_depth));
			point = move * point;
		}

		vector<vec4> back = getRectangle(width, height);
		for (vec4 &point : back)
		{
			mat4 move = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, -1.0f * half_depth));
			mat4 rotate = glm::rotate(mat4(1.0f), PI, vec3(0.0f, 1.0f, 0.0f));
			point = move * rotate * point;
		}

		vector<vec4> left = getRectangle(depth, height);
		for (vec4 &point : left)
		{
			mat4 move = glm::translate(mat4(1.0f), vec3(half_width, 0.0f, 0.0f));
			mat4 rotate = glm::rotate(mat4(1.0f), PI / 2.0f, vec3(0.0f, 1.0f, 0.0f));
			point = move * rotate * point;
		}

		vector<vec4> right = getRectangle(depth, height);
		for (vec4 &point : right)
		{
			mat4 move = glm::translate(mat4(1.0f), vec3(-1.0f * half_width, 0.0f, 0.0f));
			mat4 rotate = glm::rotate(mat4(1.0f), PI / -2.0f, vec3(0.0f, 1.0f, 0.0f));
			point = move * rotate * point;
		}

		vector<vec4> top = getRectangle(width, depth);
		for (vec4 &point : top)
		{
			mat4 move = glm::translate(mat4(1.0f), vec3(0.0f, half_height, 0.0f));
			mat4 rotate = glm::rotate(mat4(1.0f), PI / 2.0f, vec3(1.0f, 0.0f, 0.0f));
			point = move * rotate * point;
		}

		vector<vec4> bottom = getRectangle(width, depth);
		for (vec4 &point : bottom)
		{
			mat4 move = glm::translate(mat4(1.0f), vec3(0.0f, -1.0f * half_height, 0.0f));
			mat4 rotate = glm::rotate(mat4(1.0f), PI / -2.0f, vec3(1.0f, 0.0f, 0.0f));
			point = move * rotate * point;
		}

		point_sequence.insert(point_sequence.end(), front.begin(), front.end());
		point_sequence.insert(point_sequence.end(), back.begin(), back.end());
		point_sequence.insert(point_sequence.end(), left.begin(), left.end());
		point_sequence.insert(point_sequence.end(), right.begin(), right.end());
		point_sequence.insert(point_sequence.end(), top.begin(), top.end());
		point_sequence.insert(point_sequence.end(), bottom.begin(), bottom.end());
	}

	else
	{
		point_sequence.push_back(vec4(-1.0f * half_width, -1.0f * half_height, 1.0f * half_depth, 1.0f));
		point_sequence.push_back(vec4(-1.0f * half_width, 1.0f * half_height, 1.0f * half_depth, 1.0f));
		point_sequence.push_back(vec4(1.0f * half_width, 1.0f * half_height, 1.0f * half_depth, 1.0f));
		point_sequence.push_back(vec4(1.0f * half_width, -1.0f * half_height, 1.0f * half_depth, 1.0f));
		point_sequence.push_back(vec4(-1.0f * half_width, -1.0f * half_height, -1.0f * half_depth, 1.0f));
		point_sequence.push_back(vec4(-1.0f * half_width, 1.0f * half_height, -1.0f * half_depth, 1.0f));
		point_sequence.push_back(vec4(1.0f * half_width, 1.0f * half_height, -1.0f * half_depth, 1.0f));
		point_sequence.push_back(vec4(1.0f * half_width, -1.0f * half_height, -1.0f * half_depth, 1.0f));
	}

	return point_sequence;
}

vector<vec4> geometry_generator::getTriangle(float size) const
{
	float half_height = size / 2.0f;
	
	vector<vec4> point_sequence;
	
	point_sequence.push_back(vec4(-1.0f * half_height, -1.0f * half_height, 0.0f, 1.0f));
	point_sequence.push_back(vec4(0.0f, 1.0f * half_height, 0.0f, 1.0f));
	point_sequence.push_back(vec4(1.0f * half_height, 1.0f * half_height, 0.0f, 1.0f));

	return point_sequence;
}

vector<vec4> geometry_generator::getTetrahedron(float size) const
{
	float half_height = size / 2.0f;
	vector<vec4> point_sequence;

	if (export_as_triangles)
	{
		point_sequence.push_back(vec4(-1.0f * half_height, 1.0f * half_height, 1.0f * half_height, 1.0f));	//A
		point_sequence.push_back(vec4(-1.0f * half_height, -1.0f * half_height, -1.0f * half_height, 1.0f));	//B
		point_sequence.push_back(vec4(1.0f * half_height, -1.0f * half_height, 1.0f * half_height, 1.0f));	//C

		point_sequence.push_back(vec4(1.0f * half_height, -1.0f * half_height, 1.0f * half_height, 1.0f));	//C
		point_sequence.push_back(vec4(-1.0f * half_height, 1.0f * half_height, 1.0f * half_height, 1.0f));	//A
		point_sequence.push_back(vec4(1.0f * half_height, 1.0f * half_height, -1.0f * half_height, 1.0f));	//D

		point_sequence.push_back(vec4(1.0f * half_height, 1.0f * half_height, -1.0f * half_height, 1.0f));	//D
		point_sequence.push_back(vec4(-1.0f * half_height, 1.0f * half_height, 1.0f * half_height, 1.0f));	//A
		point_sequence.push_back(vec4(-1.0f * half_height, -1.0f * half_height, -1.0f * half_height, 1.0f));	//B

		point_sequence.push_back(vec4(1.0f * half_height, 1.0f * half_height, -1.0f * half_height, 1.0f));	//D
		point_sequence.push_back(vec4(1.0f * half_height, -1.0f * half_height, 1.0f * half_height, 1.0f));	//C
		point_sequence.push_back(vec4(-1.0f * half_height, -1.0f * half_height, -1.0f * half_height, 1.0f));	//B
	}

	else
	{
		point_sequence.push_back(vec4(-1.0f * half_height, 1.0f * half_height, 1.0f * half_height, 1.0f));	//A
		point_sequence.push_back(vec4(-1.0f * half_height, -1.0f * half_height, -1.0f * half_height, 1.0f));	//B
		point_sequence.push_back(vec4(1.0f * half_height, -1.0f * half_height, 1.0f * half_height, 1.0f));	//C
		point_sequence.push_back(vec4(1.0f * half_height, 1.0f * half_height, -1.0f * half_height, 1.0f));	//D

	}

	return point_sequence;
}

vector<vec4> geometry_generator::getDodecahedron(float size) const
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

	vector<vec4> point_sequence = orderDodecahedron(unordered_sequence);

	mat4 scale_matrix = glm::scale(mat4(1.0f), vec3(size, size, size));
	for (vec4 &point : point_sequence)
	{
		point = scale_matrix * point;
	}

	cout << "ordered sequence size: " << point_sequence.size() << endl;

	return point_sequence;
}

vector<vec4> geometry_generator::orderDodecahedron(const vector<vec4> &vertices) const
{
	vector<vec4> ordered_points;
	vector< vector<int> > identified_pentagons;

	for (int i = 0; i < vertices.size(); i++)
	{
		addPentagonsToSequence(i, vertices, identified_pentagons, ordered_points);
	}

	return ordered_points;
}

void geometry_generator::addPentagonsToSequence(int point_index, const vector<vec4> &vertices, vector< vector<int> > &identified_pentagons, vector<vec4> &sequence) const
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
			addPentagonToPointSequence(point_index, side_a, i, hyp_b, side_b, vertices, sequence);
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

void geometry_generator::addPentagonToPointSequence(int a, int b, int c, int d, int e, const vector<vec4> vertices, vector<vec4> &sequence) const
{
	if (export_as_triangles)
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