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
	case OCTAHEDRON: return "octahedron";
	case DODECAHEDRON: return "dodecahedron";
	case ICOSAHEDRON: return "icosahedron";
	case U_RECTANGLE: return "unordered rectangle";
	case U_SQUARE: return "unordered square";
	case U_CUBOID: return "unordered cuboid";
	case U_CUBE: return "unordered cube";
	case U_TETRAHEDRON: return "unordered tetrahedron";
	case U_OCTAHEDRON: return "unordered octahedron";
	case U_DODECAHEDRON: return "unordered dodecahedron";
	case U_ICOSAHEDRON: return "unordered icosahedron";
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

	point_sequence.push_back(vec4(-1.0f * half_width, -1.0f * half_height, 0.0f, 1.0f));
	point_sequence.push_back(vec4(-1.0f * half_width, half_height, 0.0f, 1.0f));
	point_sequence.push_back(vec4(half_width, half_height, 0.0f, 1.0f));

	point_sequence.push_back(vec4(half_width, half_height, 0.0f, 1.0f));
	point_sequence.push_back(vec4(half_width, -1.0f * half_height, 0.0f, 1.0f));
	point_sequence.push_back(vec4(-1.0f * half_width, -1.0f * half_height, 0.0f, 1.0f));

	return point_sequence;
}

vector<vec4> geometry_generator::getUnorderedRectangle(float width, float height) const
{
	float half_width = width / 2.0f;
	float half_height = height / 2.0f;

	vector<vec4> point_sequence;

	point_sequence.push_back(vec4(-1.0f * half_width, -1.0f * half_height, 0.0f, 1.0f));
	point_sequence.push_back(vec4(-1.0f * half_width, half_height, 0.0f, 1.0f));
	point_sequence.push_back(vec4(half_width, half_height, 0.0f, 1.0f));
	point_sequence.push_back(vec4(half_width, -1.0f * half_height, 0.0f, 1.0f));
	
	return point_sequence;
}

vector<vec4> geometry_generator::getCuboid(float width, float height, float depth) const
{
	float half_width = width / 2.0f;
	float half_height = height / 2.0f;
	float half_depth = depth / 2.0f;

	vector<vec4> point_sequence;

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

	return point_sequence;
}

vector<vec4> geometry_generator::getUnorderedCuboid(float width, float height, float depth) const
{
	float half_width = width / 2.0f;
	float half_height = height / 2.0f;
	float half_depth = depth / 2.0f;

	vector<vec4> point_sequence;

	point_sequence.push_back(vec4(-1.0f * half_width, -1.0f * half_height, half_depth, 1.0f));
	point_sequence.push_back(vec4(-1.0f * half_width, half_height, half_depth, 1.0f));
	point_sequence.push_back(vec4(half_width, half_height, half_depth, 1.0f));
	point_sequence.push_back(vec4(half_width, -1.0f * half_height, half_depth, 1.0f));
	point_sequence.push_back(vec4(-1.0f * half_width, -1.0f * half_height, -1.0f * half_depth, 1.0f));
	point_sequence.push_back(vec4(-1.0f * half_width, half_height, -1.0f * half_depth, 1.0f));
	point_sequence.push_back(vec4(half_width, half_height, -1.0f * half_depth, 1.0f));
	point_sequence.push_back(vec4(half_width, -1.0f * half_height, -1.0f * half_depth, 1.0f));

	return point_sequence;
}

vector<vec4> geometry_generator::getTriangle(float size) const
{
	float half_height = size / 2.0f;
	
	vector<vec4> point_sequence;
	
	point_sequence.push_back(vec4(-1.0f * half_height, -1.0f * half_height, 0.0f, 1.0f));
	point_sequence.push_back(vec4(0.0f, half_height, 0.0f, 1.0f));
	point_sequence.push_back(vec4(half_height, -1.0f * half_height, 0.0f, 1.0f));

	return point_sequence;
}

vector<vec4> geometry_generator::getTetrahedron(float size) const
{
	float half_height = size / 2.0f;
	vector<vec4> point_sequence;

	point_sequence.push_back(vec4(-1.0f * half_height, half_height, half_height, 1.0f));	//A
	point_sequence.push_back(vec4(-1.0f * half_height, -1.0f * half_height, -1.0f * half_height, 1.0f));	//B
	point_sequence.push_back(vec4(half_height, -1.0f * half_height, half_height, 1.0f));	//C

	point_sequence.push_back(vec4(half_height, -1.0f * half_height, half_height, 1.0f));	//C
	point_sequence.push_back(vec4(-1.0f * half_height, half_height, half_height, 1.0f));	//A
	point_sequence.push_back(vec4(half_height, half_height, -1.0f * half_height, 1.0f));	//D

	point_sequence.push_back(vec4(half_height, half_height, -1.0f * half_height, 1.0f));	//D
	point_sequence.push_back(vec4(-1.0f * half_height, half_height, half_height, 1.0f));	//A
	point_sequence.push_back(vec4(-1.0f * half_height, -1.0f * half_height, -1.0f * half_height, 1.0f));	//B

	point_sequence.push_back(vec4(half_height, half_height, -1.0f * half_height, 1.0f));	//D
	point_sequence.push_back(vec4(half_height, -1.0f * half_height, half_height, 1.0f));	//C
	point_sequence.push_back(vec4(-1.0f * half_height, -1.0f * half_height, -1.0f * half_height, 1.0f));	//B

	return point_sequence;
}

vector<vec4> geometry_generator::getUnorderedTetrahedron(float size) const
{
	float half_height = size / 2.0f;
	vector<vec4> point_sequence;

	point_sequence.push_back(vec4(-1.0f * half_height, half_height, half_height, 1.0f));	//A
	point_sequence.push_back(vec4(-1.0f * half_height, -1.0f * half_height, -1.0f * half_height, 1.0f));	//B
	point_sequence.push_back(vec4(half_height, -1.0f * half_height, half_height, 1.0f));	//C
	point_sequence.push_back(vec4(half_height, half_height, -1.0f * half_height, 1.0f));	//D

	return point_sequence;
}

vector<vec4> geometry_generator::getOctahedron(float size) const
{
	float half_height = size / 2.0f;
	vector<vec4> point_sequence;

	//AEB
	point_sequence.push_back(vec4(0.0f, half_height, 0.0f, 1.0f));	//A
	point_sequence.push_back(vec4(-1.0f * half_height, 0.0f, 0.0f, 1.0f));	//E
	point_sequence.push_back(vec4(0.0f, 0.0f, half_height, 1.0f));	//B
	//ABC
	point_sequence.push_back(vec4(0.0f, half_height, 0.0f, 1.0f));	//A
	point_sequence.push_back(vec4(0.0f, 0.0f, half_height, 1.0f));	//B
	point_sequence.push_back(vec4(half_height, 0.0f, 0.0f, 1.0f));	//C
	//ACD
	point_sequence.push_back(vec4(0.0f, half_height, 0.0f, 1.0f));	//A
	point_sequence.push_back(vec4(half_height, 0.0f, 0.0f, 1.0f));	//C
	point_sequence.push_back(vec4(0.0f, 0.0f, -1.0f * half_height, 1.0f));	//D
	//ADE
	point_sequence.push_back(vec4(0.0f, half_height, 0.0f, 1.0f));	//A
	point_sequence.push_back(vec4(0.0f, 0.0f, -1.0f * half_height, 1.0f));	//D
	point_sequence.push_back(vec4(-1.0f * half_height, 0.0f, 0.0f, 1.0f));	//E
	//FCB
	point_sequence.push_back(vec4(0.0f, -1.0f * half_height, 0.0f, 1.0f));	//F
	point_sequence.push_back(vec4(half_height, 0.0f, 0.0f, 1.0f));	//C
	point_sequence.push_back(vec4(0.0f, 0.0f, half_height, 1.0f));	//B
	//FDC
	point_sequence.push_back(vec4(0.0f, -1.0f * half_height, 0.0f, 1.0f));	//F
	point_sequence.push_back(vec4(0.0f, 0.0f, -1.0f * half_height, 1.0f));	//D
	point_sequence.push_back(vec4(half_height, 0.0f, 0.0f, 1.0f));	//C
	//FED
	point_sequence.push_back(vec4(0.0f, -1.0f * half_height, 0.0f, 1.0f));	//F
	point_sequence.push_back(vec4(-1.0f * half_height, 0.0f, 0.0f, 1.0f));	//E
	point_sequence.push_back(vec4(0.0f, 0.0f, -1.0f * half_height, 1.0f));	//D
	//FBE
	point_sequence.push_back(vec4(0.0f, -1.0f * half_height, 0.0f, 1.0f));	//F
	point_sequence.push_back(vec4(0.0f, 0.0f, half_height, 1.0f));	//B
	point_sequence.push_back(vec4(-1.0f * half_height, 0.0f, 0.0f, 1.0f));	//E

	return point_sequence;
}

vector<vec4> geometry_generator::getUnorderedOctahedron(float size) const
{
	float half_height = size / 2.0f;
	vector<vec4> point_sequence;

	point_sequence.push_back(vec4(0.0f, half_height, 0.0f, 1.0f));	//A
	point_sequence.push_back(vec4(0.0f, 0.0f, half_height, 1.0f));	//B
	point_sequence.push_back(vec4(half_height, 0.0f, 0.0f, 1.0f));	//C
	point_sequence.push_back(vec4(0.0f, 0.0f, -1.0f * half_height, 1.0f));	//D
	point_sequence.push_back(vec4(-1.0f * half_height, 0.0f, 0.0f, 1.0f));	//E
	point_sequence.push_back(vec4(0.0f, -1.0f * half_height, 0.0f, 1.0f));	//F

	return point_sequence;
}

vector<vec4> geometry_generator::getDodecahedron(float size) const
{
	vector<vec4> point_sequence = orderDodecahedron(getUnorderedDodecahedron(size));

	mat4 scale_matrix = glm::scale(mat4(1.0f), vec3(size, size, size));
	for (vec4 &point : point_sequence)
	{
		point = scale_matrix * point;
	}

	return point_sequence;
}

vector<vec4> geometry_generator::getUnorderedDodecahedron(float size) const
{
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
		float y = i % 4 < 2 ? 1.0f / THETA : -1.0f / THETA;
		float z = i % 2 == 0 ? THETA : -1.0f * THETA;

		unordered_sequence.push_back(vec4(x, y, z, 1.0f));
	}

	for (int i = 0; i < 4; i++)
	{
		float x = i % 4 < 2 ? 1.0f / THETA : -1.0f / THETA;
		float y = i % 2 == 0 ? THETA : -1.0f * THETA;
		float z = 0;

		unordered_sequence.push_back(vec4(x, y, z, 1.0f));
	}

	for (int i = 0; i < 4; i++)
	{
		float x = i % 2 == 0 ? THETA : -1.0f * THETA;
		float y = 0;
		float z = i % 4 < 2 ? 1.0f / THETA : -1.0f / THETA;

		unordered_sequence.push_back(vec4(x, y, z, 1.0f));
	}

	return unordered_sequence;
}

vector<vec4> geometry_generator::getIcosahedron(float size) const
{
	vector<vec4> point_sequence = orderIcosahedron(getUnorderedIcosahedron(size));

	mat4 scale_matrix = glm::scale(mat4(1.0f), vec3(size, size, size));
	for (vec4 &point : point_sequence)
	{
		point = scale_matrix * point;
	}

	return point_sequence;
}

vector<vec4> geometry_generator::getUnorderedIcosahedron(float size) const
{
	vector<vec4> unordered_sequence;

	//calcs derived from http://math.wikia.com/wiki/Icosahedron
	for (int i = 0; i < 4; i++)
	{
		float x = 0;
		float y = i % 4 < 2 ? 1.0f : -1.0f;
		float z = i % 2 == 0 ? THETA : -1.0f * THETA;

		unordered_sequence.push_back(vec4(x, y, z, 1.0f));
	}

	for (int i = 0; i < 4; i++)
	{
		float x = i % 4 < 2 ? 1.0f : -1.0f;
		float y = i % 2 == 0 ? THETA : -1.0f * THETA;
		float z = 0;

		unordered_sequence.push_back(vec4(x, y, z, 1.0f));
	}

	for (int i = 0; i < 4; i++)
	{
		float x = i % 2 == 0 ? THETA : -1.0f * THETA;
		float y = 0;
		float z = i % 4 < 2 ? 1.0f : -1.0f;

		unordered_sequence.push_back(vec4(x, y, z, 1.0f));
	}

	return unordered_sequence;
}

vector<vec4> geometry_generator::orderIcosahedron(const vector<vec4> &vertices) const
{
	vector<vec4> ordered_points;
	vector< vector<int> > identified_triangles;

	for (int i = 0; i < vertices.size(); i++)
	{
		addAllTrianglesToIcosahedronSequence(i, vertices, identified_triangles, ordered_points);
	}
	return ordered_points;
}

void geometry_generator::addAllTrianglesToIcosahedronSequence(int point_index, const vector<vec4> &vertices, vector< vector<int> > &identified_triangles, vector<vec4> &sequence) const
{
	int neighbor_count = 0;

	for (int i = 0; i < vertices.size() && neighbor_count < 5; i++)
	{
		if (i == point_index)
			continue;

		float dist = abs(glm::distance(vertices.at(point_index), vertices.at(i)));

		//identifies neighbor point
		if (abs(dist - 2.0f) < .0001f)
		{
			neighbor_count++;
			int third_point = findThirdIcosahedronTrianglePoint(point_index, i, vertices);
			vector<int> triangle = { point_index, i, third_point };
			if (geometryAlreadyIdentified(triangle, identified_triangles))
				continue;

			else
			{
				identified_triangles.push_back(triangle);
				addGeometryToSequence(triangle, vertices, sequence);
			}
		}
	}
}

bool geometry_generator::geometryAlreadyIdentified(const vector<int> &geometry, const vector< vector<int> > &identified_geometry) const
{
	for (const vector<int> &identified : identified_geometry)
	{
		if (geometry.size() != identified.size())
			continue;

		bool match = true;

		for (const int &index : geometry)
		{
			if (std::find(identified.begin(), identified.end(), index) == identified.end())
			{
				match = false;
				break;
			}
		}

		if (match)
			return true;
	}

	return false;
}

void geometry_generator::addGeometryToSequence(const vector<int> &geometry_indices, const vector<vec4> &vertices, vector<vec4> &sequence) const
{
	for (const int &index : geometry_indices)
	{
		sequence.push_back(vertices.at(index));
	}
}

vector<vec4> geometry_generator::orderDodecahedron(const vector<vec4> &vertices) const
{
	vector<vec4> ordered_points;
	vector< vector<int> > identified_pentagons;

	for (int i = 0; i < vertices.size(); i++)
	{
		addAllPentagonsToDodecahedronSequence(i, vertices, identified_pentagons, ordered_points);
	}

	return ordered_points;
}

void geometry_generator::addAllPentagonsToDodecahedronSequence(int point_index, const vector<vec4> &vertices, vector< vector<int> > &identified_pentagons, vector<vec4> &sequence) const
{
	int hypotenuse_count = 0;

	for (int i = 0; i < vertices.size() && hypotenuse_count < 6; i++)
	{
		if (i == point_index)
			continue;

		float dist = abs(glm::distance(vertices.at(point_index), vertices.at(i)));

		//identifies pentagon hypotenuses
		if (abs(dist - HYPOTENUSE_LENGTH) < .0001f)
		{
			hypotenuse_count++;

			if (pentagonAlreadyIdentified(point_index, i, identified_pentagons))
				continue;

			int side_a = findSideFromTargetAndHypotenuse(point_index, i, vertices);
			int hyp_b = findOtherHypotenuseFromTargetAndHypotenuse(point_index, i, vertices);
			int side_b = findSideFromTargetAndHypotenuse(point_index, hyp_b, vertices);

			vector<int> pentagon = { point_index, side_a, i, hyp_b, side_b };
			identified_pentagons.push_back(pentagon);

			if (export_as_triangles)
			{
				vector<int> triangle_a = { point_index, side_a, i };
				addGeometryToSequence(triangle_a, vertices, sequence);
				vector<int> triangle_b = { point_index, i, hyp_b };
				addGeometryToSequence(triangle_b, vertices, sequence);
				vector<int> triangle_c = { point_index, hyp_b, side_b };
				addGeometryToSequence(triangle_c, vertices, sequence);
			}

			else addGeometryToSequence(pentagon, vertices, sequence);
		}
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

int geometry_generator::findThirdIcosahedronTrianglePoint(int first, int second, const vector<vec4> &vertices) const
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if (i == first || i == second)
			continue;

		if (abs(glm::distance(vertices.at(i), vertices.at(first)) - 1.0f) < .0001f && abs(glm::distance(vertices.at(i), vertices.at(second)) - 1.0f) < .0001f)
			return i;
	}

	return 0;
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