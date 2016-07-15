#pragma once

#include "header.h"

#define BASE_LENGTH 2.0f * 0.61803398875f
#define HYPOTENUSE_LENGTH 2.0f

enum geometry_type { TRIANGLE, RECTANGLE, SQUARE, CUBOID, CUBE, TETRAHEDRON, OCTAHEDRON, DODECAHEDRON, ICOSAHEDRON, LOADED_SEQUENCE, DEFAULT_GEOMETRY_TYPE };

string getStringFromGeometryType(geometry_type gt);

class geometry_generator
{
public:
	vector<vec4> getSquare(float size) const { return getRectangle(size, size); }
	vector<vec4> getRectangle(float width, float height) const;
	vector<vec4> getCuboid(float width, float height, float depth) const;
	vector<vec4> getCube(float size) const { return getCuboid(size, size, size); }
	vector<vec4> getTriangle(float size) const;
	vector<vec4> getTetrahedron(float size) const;
	vector<vec4> getOctahedron(float size) const;
	vector<vec4> getDodecahedron(float size) const;
	vector<vec4> getUnorderedDodecahedron(float size) const;
	vector<vec4> getIcosahedron(float size) const;

	void setExportAsTriangles(bool b) { export_as_triangles = b; }

private:
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