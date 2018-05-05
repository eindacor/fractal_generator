#pragma once

#include "header.h"

#define BASE_LENGTH 2.0f * 0.61803398875f
#define HYPOTENUSE_LENGTH 2.0f
#define NGON_SIDE_MAX 15

enum geometry_type { CUBOID, CUBE, TETRAHEDRON, OCTAHEDRON, DODECAHEDRON, ICOSAHEDRON, GEOMETRY_TYPE_SIZE };
enum ngon_type { TRIANGLE, RECTANGLE, PENTAGON, HEXAGON, HEPTAGON, OCTAGON, ENNAGON, DECAGON, HENDECAGON, DODECAGON, NGON_TYPE_SIZE };
enum attribute_index_method { POINT_INDICES, LINE_INDICES, TRIANGLE_INDICES, ATTRIBUTE_INDEX_METHOD_SIZE };

// total number of enumerated geometry types
#define GEOMETRY_ENUM_COUNT ((int)GEOMETRY_TYPE_SIZE + (int)NGON_TYPE_SIZE - 2)

string getStringFromGeometryType(geometry_type gt);
string getStringFromAttributeIndexMethod(attribute_index_method aim);

class geometry_generator
{
public:
	vector<vec4> getNgonVertices(float size, int sides) const;
	vector<vec4> getNgonVerticesWithOrientation(float size, int sides, vec4 normal) const;

	vector<vec4> getCuboidVertices(float width, float height, float depth) const;
	vector<vec4> getCubeVertices(float size) const { return getCuboidVertices(size, size, size); }
	vector<vec4> getTetrahedronVertices(float size) const;
	vector<vec4> getOctahedronVertices(float size) const;
	vector<vec4> getDodecahedronVertices(float size) const;
	vector<vec4> getIcosahedronVertices(float size) const;

	vector<int> getSolidGeometryIndices(geometry_type gt, attribute_index_method aim) const;
	vector<int> getNgonIndices(ngon_type nt, attribute_index_method aim) const;
	vector<int> getNgonIndicesBySideCount(int sides, attribute_index_method aim) const;

private:
	vector<int> getNgonPointIndices(int sides) const;
	vector<int> getNgonLineIndices(int sides) const;
	vector<int> getNgonTriangleIndices(int sides) const;

	vector<int> getCuboidIndices(attribute_index_method aim) const;
	vector<int> getCuboidPointIndices() const;
	vector<int> getCuboidLineIndices() const;
	vector<int> getCuboidTriangleIndices() const;

	vector<int> getTetrahedronIndices(attribute_index_method aim) const;
	vector<int> getTetrahedronPointIndices() const;
	vector<int> getTetrahedronLineIndices() const;
	vector<int> getTetrahedronTriangleIndices() const;

	vector<int> getOctahedronIndices(attribute_index_method aim) const;
	vector<int> getOctahedronPointIndices() const;
	vector<int> getOctahedronLineIndices() const;
	vector<int> getOctahedronTriangleIndices() const;

	vector<int> getIcosahedronIndices(attribute_index_method aim) const;
	vector<int> getIcosahedronPointIndices() const;
	vector<int> getIcosahedronLineIndices() const;
	vector<int> getIcosahedronTriangleIndices() const;

	vector<int> getDodecahedronIndices(attribute_index_method aim) const;
	vector<int> getDodecahedronPointIndices() const;
	vector<int> getDodecahedronLineIndices() const;
	vector<int> getDodecahedronTriangleIndices() const;

	void addAllTriangleGeometryToIcosahedronSequence(int point_index, const vector<vec4> &vertices, vector< vector<int> > &identified_pentagons, vector<int> &sequence) const;
	void addAllLineGeometryToIcosahedronSequence(int point_index, const vector<vec4> &vertices, vector< vector<int> > &identified_lines, vector<int> &sequence) const;
	void addGeometryToSequence(const vector<int> &geometry_indices, vector<int> &sequence) const;

	void addAllTriangleGeometryToDodecahedronSequence(int point_index, const vector<vec4> &vertices, vector< vector<int> > &identified_pentagons, vector<int> &sequence) const;
	void addAllLineGeometryToDodecahedronSequence(int point_index, const vector<vec4> &vertices, vector< vector<int> > &identified_lines, vector<int> &sequence) const;
	
	// used for dodecahedron methods
	bool pentagonAlreadyIdentified(int index_a, int index_b, vector< vector<int> > &identified_pentagons) const;

	bool geometryAlreadyIdentified(const vector<int> &geometry, const vector< vector<int> > &identified_geometry) const;
	
	int findThirdIcosahedronTrianglePoint(int first, int second, const vector<vec4> &vertices) const;
	int findSideFromTargetAndHypotenuse(int target, int hypotenuse, const vector<vec4> &vertices) const;
	int findOtherHypotenuseFromTargetAndHypotenuse(int target, int hypotenuse, const vector<vec4> &vertices) const;
};