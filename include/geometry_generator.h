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
	std::vector<vec4> getNgonVertices(float size, int sides) const;
	std::vector<vec4> getNgonVerticesWithOrientation(float size, int sides, vec4 normal) const;

	std::vector<vec4> getCuboidVertices(float width, float height, float depth) const;
	std::vector<vec4> getCubeVertices(float size) const { return getCuboidVertices(size, size, size); }
	std::vector<vec4> getTetrahedronVertices(float size) const;
	std::vector<vec4> getOctahedronVertices(float size) const;
	std::vector<vec4> getDodecahedronVertices(float size) const;
	std::vector<vec4> getIcosahedronVertices(float size) const;

	std::vector<int> getSolidGeometryIndices(geometry_type gt, attribute_index_method aim) const;
	std::vector<int> getNgonIndices(ngon_type nt, attribute_index_method aim) const;
	std::vector<int> getNgonIndicesBySideCount(int sides, attribute_index_method aim) const;

private:
	std::vector<int> getNgonPointIndices(int sides) const;
	std::vector<int> getNgonLineIndices(int sides) const;
	std::vector<int> getNgonTriangleIndices(int sides) const;

	std::vector<int> getCuboidIndices(attribute_index_method aim) const;
	std::vector<int> getCuboidPointIndices() const;
	std::vector<int> getCuboidLineIndices() const;
	std::vector<int> getCuboidTriangleIndices() const;

	std::vector<int> getTetrahedronIndices(attribute_index_method aim) const;
	std::vector<int> getTetrahedronPointIndices() const;
	std::vector<int> getTetrahedronLineIndices() const;
	std::vector<int> getTetrahedronTriangleIndices() const;

	std::vector<int> getOctahedronIndices(attribute_index_method aim) const;
	std::vector<int> getOctahedronPointIndices() const;
	std::vector<int> getOctahedronLineIndices() const;
	std::vector<int> getOctahedronTriangleIndices() const;

	std::vector<int> getIcosahedronIndices(attribute_index_method aim) const;
	std::vector<int> getIcosahedronPointIndices() const;
	std::vector<int> getIcosahedronLineIndices() const;
	std::vector<int> getIcosahedronTriangleIndices() const;

	std::vector<int> getDodecahedronIndices(attribute_index_method aim) const;
	std::vector<int> getDodecahedronPointIndices() const;
	std::vector<int> getDodecahedronLineIndices() const;
	std::vector<int> getDodecahedronTriangleIndices() const;

	void addAllTriangleGeometryToIcosahedronSequence(int point_index, const std::vector<vec4> &vertices, std::vector< std::vector<int> > &identified_pentagons, std::vector<int> &sequence) const;
	void addAllLineGeometryToIcosahedronSequence(int point_index, const std::vector<vec4> &vertices, std::vector< std::vector<int> > &identified_lines, std::vector<int> &sequence) const;
	void addGeometryToSequence(const std::vector<int> &geometry_indices, std::vector<int> &sequence) const;

	void addAllTriangleGeometryToDodecahedronSequence(int point_index, const std::vector<vec4> &vertices, std::vector< std::vector<int> > &identified_pentagons, std::vector<int> &sequence) const;
	void addAllLineGeometryToDodecahedronSequence(int point_index, const std::vector<vec4> &vertices, std::vector< std::vector<int> > &identified_lines, std::vector<int> &sequence) const;
	
	// used for dodecahedron methods
	bool pentagonAlreadyIdentified(int index_a, int index_b, std::vector< std::vector<int> > &identified_pentagons) const;

	bool geometryAlreadyIdentified(const std::vector<int> &geometry, const std::vector< std::vector<int> > &identified_geometry) const;
	
	int findThirdIcosahedronTrianglePoint(int first, int second, const std::vector<vec4> &vertices) const;
	int findSideFromTargetAndHypotenuse(int target, int hypotenuse, const std::vector<vec4> &vertices) const;
	int findOtherHypotenuseFromTargetAndHypotenuse(int target, int hypotenuse, const std::vector<vec4> &vertices) const;
};