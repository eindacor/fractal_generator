#pragma once

#include "header.h"

#define BASE_LENGTH 2.0f * 0.61803398875f
#define HYPOTENUSE_LENGTH 2.0f

enum geometry_type { TRIANGLE, RECTANGLE, U_RECTANGLE, SQUARE, U_SQUARE, CUBOID, U_CUBOID, CUBE, U_CUBE, TETRAHEDRON, U_TETRAHEDRON, OCTAHEDRON, U_OCTAHEDRON, DODECAHEDRON, U_DODECAHEDRON, ICOSAHEDRON, U_ICOSAHEDRON, LOADED_SEQUENCE, DEFAULT_GEOMETRY_TYPE };
enum attribute_index_method { POINT_INDICES, LINE_INDICES, TRIANGLE_INDICES, ATTRIBUTE_INDEX_METHOD_SIZE };

string getStringFromGeometryType(geometry_type gt);

class geometry_generator
{
public:
	vector<vec4> getTriangleVertices(float size) const;
	vector<vec4> getRectangleVertices(float width, float height) const;
	vector<vec4> getSquareVertices(float size) { return getRectangleVertices(size, size); }
	vector<vec4> getCuboidVertices(float width, float height, float depth) const;
	vector<vec4> getCubeVertices(float size) const { return getCuboidVertices(size, size, size); }
	vector<vec4> getTetrahedronVertices(float size) const;
	vector<vec4> getOctahedronVertices(float size) const;

	vector<vec4> getDodecahedron(float size) const;
	vector<vec4> getDodecahedronVertices(float size) const;
	vector<vec4> getIcosahedron(float size) const;
	vector<vec4> getIcosahedronVertices(float size) const;

	void setExportAsTriangles(bool b) { export_as_triangles = b; }

private:
	vector<int> getTrianglePointIndices() const;
	vector<int> getTriangleLineIndices() const;

	vector<int> getRectanglePointIndices() const;
	vector<int> getRectangleLineIndices() const;
	vector<int> getRectangleTriangleIndices() const;

	vector<int> getCuboidPointIndices() const;
	vector<int> getCuboidLineIndices() const;
	vector<int> getCuboidTriangleIndices() const;

	vector<int> getTetrahedronPointIndices() const;
	vector<int> getTetrahedronLineIndices() const;
	vector<int> getTetrahedronTriangleIndices() const;

	vector<int> getOctahedronPointIndices() const;
	vector<int> getOctahedronLineIndices() const;
	vector<int> getOctahedronTriangleIndices() const;

	vector<vec4> orderIcosahedron(const vector<vec4> &vertices) const;
	void addAllTrianglesToIcosahedronSequence(int point_index, const vector<vec4> &vertices, vector< vector<int> > &identified_pentagons, vector<vec4> &sequence) const;
	void addGeometryToSequence(const vector<int> &geometry_indices, const vector<vec4> &vertices, vector<vec4> &sequence) const;
	vector<vec4> orderDodecahedron(const vector<vec4> &vertices) const;
	void addAllPentagonsToDodecahedronSequence(int point_index, const vector<vec4> &vertices, vector< vector<int> > &identified_pentagons, vector<vec4> &sequence) const;
	bool geometryAlreadyIdentified(const vector<int> &geometry, const vector< vector<int> > &identified_geometry) const;
	bool pentagonAlreadyIdentified(int index_a, int index_b, vector< vector<int> > &identified_pentagons) const;
	int findThirdIcosahedronTrianglePoint(int first, int second, const vector<vec4> &vertices) const;
	int findSideFromTargetAndHypotenuse(int target, int hypotenuse, const vector<vec4> &vertices) const;
	int findOtherHypotenuseFromTargetAndHypotenuse(int target, int hypotenuse, const vector<vec4> &vertices) const;

	bool export_as_triangles = true;
};