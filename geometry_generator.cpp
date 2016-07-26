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
	//case LOADED_SEQUENCE: return "custom sequence";
	case GEOMETRY_TYPE_SIZE: return "points";
	default: return "unknown type";
	}
}

vector<vec4> geometry_generator::getTriangleVertices(float size) const
{
	float half_height = size / 2.0f;

	vector<vec4> point_sequence;

	point_sequence.push_back(vec4(-1.0f * half_height, -1.0f * half_height, 0.0f, 1.0f));
	point_sequence.push_back(vec4(0.0f, half_height, 0.0f, 1.0f));
	point_sequence.push_back(vec4(half_height, -1.0f * half_height, 0.0f, 1.0f));

	return point_sequence;
}

vector<int> geometry_generator::getTrianglePointIndices() const
{
	vector<int> indices;
	for (int i = 0; i < 3; i++)
	{
		indices.push_back(i);
	}
	return indices;
}

vector<int> geometry_generator::getTriangleLineIndices() const
{
	return vector<int>{ 0, 1, 1, 2, 2, 0 };
}

vector<vec4> geometry_generator::getRectangleVertices(float width, float height) const
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

vector<int> geometry_generator::getRectanglePointIndices() const
{
	vector<int> indices;
	for (int i = 0; i < 4; i++)
	{
		indices.push_back(i);
	}
	return indices;
}

vector<int> geometry_generator::getRectangleLineIndices() const
{
	return vector<int>{ 0, 1, 1, 2, 2, 3, 3, 0 };
}

vector<int> geometry_generator::getRectangleTriangleIndices() const
{
	return vector<int>{ 0, 1, 2, 2, 3, 0 };
}

vector<vec4> geometry_generator::getCuboidVertices(float width, float height, float depth) const
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

vector<int> geometry_generator::getCuboidPointIndices() const
{
	vector<int> indices;
	for (int i = 0; i < 8; i++)
	{
		indices.push_back(i);
	}
	return indices;
}

vector<int> geometry_generator::getCuboidLineIndices() const
{
	return vector<int>{ 0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 1, 0, 4, 7, 6, 6, 5, 4, 7, 2, 6, 7, 3 };
}

vector<int> geometry_generator::getCuboidTriangleIndices() const
{
	return vector<int>{ 
		0, 1, 2,  
		0, 2, 3,
		4, 5, 1, 
		4, 1, 0,
		7, 6, 5,
		7, 5, 4,
		3, 2, 6,
		3, 6, 7,
		1, 5, 6,
		1, 6, 2,
		4, 0, 3,
		4, 3, 7
	};
}

vector<vec4> geometry_generator::getTetrahedronVertices(float size) const
{
	float half_height = size / 2.0f;
	vector<vec4> point_sequence;

	point_sequence.push_back(vec4(-1.0f * half_height, half_height, half_height, 1.0f));	//A
	point_sequence.push_back(vec4(-1.0f * half_height, -1.0f * half_height, -1.0f * half_height, 1.0f));	//B
	point_sequence.push_back(vec4(half_height, -1.0f * half_height, half_height, 1.0f));	//C
	point_sequence.push_back(vec4(half_height, half_height, -1.0f * half_height, 1.0f));	//D

	return point_sequence;
}

vector<int> geometry_generator::getTetrahedronPointIndices() const
{
	vector<int> indices;
	for (int i = 0; i < 4; i++)
	{
		indices.push_back(i);
	}
	return indices;
}

vector<int> geometry_generator::getTetrahedronLineIndices() const
{
	return vector<int>{
		0, 1,
		1, 2,
		2, 0,
		2, 3,
		0, 3,
		3, 1
	};
}

vector<int> geometry_generator::getTetrahedronTriangleIndices() const
{
	return vector<int>{
		0, 1, 2,
		2, 0, 3,
		3, 0, 1,
		3, 2, 1
	};
}

vector<vec4> geometry_generator::getOctahedronVertices(float size) const
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

vector<int> geometry_generator::getOctahedronPointIndices() const
{
	vector<int> indices;
	for (int i = 0; i < 6; i++)
	{
		indices.push_back(i);
	}
	return indices;
}

vector<int> geometry_generator::getOctahedronLineIndices() const
{
	return vector<int>{
		0, 4,
		0, 1,
		4, 1,
		0, 2,
		1, 2,
		0, 3,
		2, 3,
		3, 4,
		5, 2,
		5, 1,
		5, 3,
		5, 4
	};
}

vector<int> geometry_generator::getOctahedronTriangleIndices() const
{
	return vector<int>{
		0, 4, 1,
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		5, 2, 1,
		5, 3, 2,
		5, 4, 3,
		5, 1, 4
	};
}

vector<vec4> geometry_generator::getIcosahedronVertices(float size) const
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

vector<int> geometry_generator::getIcosahedronPointIndices() const
{
	vector<int> indices;
	for (int i = 0; i < 12; i++)
	{
		indices.push_back(i);
	}
	return indices;
}

vector<int> geometry_generator::getIcosahedronLineIndices() const
{
	vector<int> index_sequence;
	vector<vec4> vertices = getIcosahedronVertices(1.0f);
	vector< vector<int> > identified_lines;

	for (int i = 0; i < vertices.size(); i++)
	{
		addAllLineGeometryToIcosahedronSequence(i, vertices, identified_lines, index_sequence);
	}

	return index_sequence;
}

vector<int> geometry_generator::getIcosahedronTriangleIndices() const
{
	vector<int> index_sequence;
	vector<vec4> vertices = getIcosahedronVertices(1.0f);
	vector< vector<int> > identified_pentagons;

	for (int i = 0; i < vertices.size(); i++)
	{
		addAllTriangleGeometryToIcosahedronSequence(i, vertices, identified_pentagons, index_sequence);
	}

	return index_sequence;
}

void geometry_generator::addAllTriangleGeometryToIcosahedronSequence(int point_index, const vector<vec4> &vertices, vector< vector<int> > &identified_triangles, vector<int> &sequence) const
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
				addGeometryToSequence(triangle, sequence);
			}
		}
	}
}

void geometry_generator::addAllLineGeometryToIcosahedronSequence(int point_index, const vector<vec4> &vertices, vector< vector<int> > &identified_lines, vector<int> &sequence) const
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
			vector<int> line = { point_index, i };
			if (geometryAlreadyIdentified(line, identified_lines))
				continue;

			else
			{
				identified_lines.push_back(line);
				addGeometryToSequence(line, sequence);
			}
		}
	}
}

vector<vec4> geometry_generator::getDodecahedronVertices(float size) const
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

vector<int> geometry_generator::getDodecahedronPointIndices() const
{
	vector<int> indices;
	for (int i = 0; i < 20; i++)
	{
		indices.push_back(i);
	}
	return indices;
}

vector<int> geometry_generator::getDodecahedronLineIndices() const
{
	vector<int> index_sequence;
	vector<vec4> vertices = getDodecahedronVertices(1.0f);
	vector< vector<int> > identified_lines;

	for (int i = 0; i < vertices.size(); i++)
	{
		addAllLineGeometryToDodecahedronSequence(i, vertices, identified_lines, index_sequence);
	}

	return index_sequence;
}

vector<int> geometry_generator::getDodecahedronTriangleIndices() const
{
	vector<int> index_sequence;
	vector<vec4> vertices = getDodecahedronVertices(1.0f);
	vector< vector<int> > identified_pentagons;

	for (int i = 0; i < vertices.size(); i++)
	{
		addAllTriangleGeometryToDodecahedronSequence(i, vertices, identified_pentagons, index_sequence);
	}

	return index_sequence;
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

void geometry_generator::addGeometryToSequence(const vector<int> &geometry_indices, vector<int> &sequence) const
{
	for (const int &index : geometry_indices)
	{
		sequence.push_back(index);
	}
}

void geometry_generator::addAllLineGeometryToDodecahedronSequence(int point_index, const vector<vec4> &vertices, vector< vector<int> > &identified_lines, vector<int> &sequence) const
{
	int neighbor_count = 0;

	for (int i = 0; i < vertices.size() && neighbor_count < 3; i++)
	{
		if (i == point_index)
			continue;

		float dist = abs(glm::distance(vertices.at(point_index), vertices.at(i)));

		//identifies neighboring points
		if (abs(dist - BASE_LENGTH) < .0001f)
		{
			neighbor_count++;

			vector<int> line = { point_index, i };
			if (geometryAlreadyIdentified(line, identified_lines))
				continue;

			addGeometryToSequence(line, sequence);
		}
	}
}

void geometry_generator::addAllTriangleGeometryToDodecahedronSequence(int point_index, const vector<vec4> &vertices, vector< vector<int> > &identified_pentagons, vector<int> &sequence) const
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

			vector<int> triangle_a = { point_index, side_a, i };
			addGeometryToSequence(triangle_a, sequence);
			vector<int> triangle_b = { point_index, i, hyp_b };
			addGeometryToSequence(triangle_b, sequence);
			vector<int> triangle_c = { point_index, hyp_b, side_b };
			addGeometryToSequence(triangle_c, sequence);
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

vector<int> geometry_generator::getIndices(geometry_type gt, attribute_index_method aim) const
{
	switch (gt)
	{
	case TRIANGLE: return getTriangleIndices(aim);
	case RECTANGLE: return getRectangleIndices(aim);
	case SQUARE: return getRectangleIndices(aim);
	case CUBOID: return getCuboidIndices(aim);
	case CUBE: return getCuboidIndices(aim);
	case TETRAHEDRON: return getTetrahedronIndices(aim);
	case OCTAHEDRON: return getOctahedronIndices(aim);
	case DODECAHEDRON: return getDodecahedronIndices(aim);
	case ICOSAHEDRON: return getIcosahedronIndices(aim);
	//case LOADED_SEQUENCE:
	case GEOMETRY_TYPE_SIZE:
	default: 
		cout << "unable to generate indices for specified geometry type" << endl;
		throw;
	}
}

vector<int> geometry_generator::getTriangleIndices(attribute_index_method aim) const
{
	switch (aim)
	{
	case LINE_INDICES: return getTriangleLineIndices();
	default: return getTrianglePointIndices();
	}
}

vector<int> geometry_generator::getRectangleIndices(attribute_index_method aim) const
{
	switch (aim)
	{
	case POINT_INDICES: return getRectanglePointIndices();
	case LINE_INDICES: return getRectanglePointIndices();
	case TRIANGLE_INDICES: return getRectangleTriangleIndices();
	default: return getRectanglePointIndices();
	}
}

vector<int> geometry_generator::getCuboidIndices(attribute_index_method aim) const
{
	switch (aim)
	{
	case POINT_INDICES: return getCuboidPointIndices();
	case LINE_INDICES: return getCuboidPointIndices();
	case TRIANGLE_INDICES: return getCuboidTriangleIndices();
	default: return getCuboidPointIndices();
	}
}

vector<int> geometry_generator::getTetrahedronIndices(attribute_index_method aim) const
{
	switch (aim)
	{
	case POINT_INDICES: return getTetrahedronPointIndices();
	case LINE_INDICES: return getTetrahedronPointIndices();
	case TRIANGLE_INDICES: return getTetrahedronTriangleIndices();
	default: return getTetrahedronPointIndices();
	}
}

vector<int> geometry_generator::getOctahedronIndices(attribute_index_method aim) const
{
	switch (aim)
	{
	case POINT_INDICES: return getOctahedronPointIndices();
	case LINE_INDICES: return getOctahedronPointIndices();
	case TRIANGLE_INDICES: return getOctahedronTriangleIndices();
	default: return getOctahedronPointIndices();
	}
}

vector<int> geometry_generator::getDodecahedronIndices(attribute_index_method aim) const
{
	switch (aim)
	{
	case POINT_INDICES: return getDodecahedronPointIndices();
	case LINE_INDICES: return getDodecahedronPointIndices();
	case TRIANGLE_INDICES: return getDodecahedronTriangleIndices();
	default: return getDodecahedronPointIndices();
	}
}

vector<int> geometry_generator::getIcosahedronIndices(attribute_index_method aim) const
{
	switch (aim)
	{
	case POINT_INDICES: return getIcosahedronPointIndices();
	case LINE_INDICES: return getIcosahedronPointIndices();
	case TRIANGLE_INDICES: return getIcosahedronTriangleIndices();
	default: return getIcosahedronPointIndices();
	}
}