#pragma once

#include "header.h"

class geometry_generator
{
public:
	vector<vec4> getSquare(float size) const;
	vector<vec4> getTriangle(float size) const;
	vector<vec4> getTetrahedron(float size) const;
	vector<vec4> getDodecahedron(float size, bool triangles) const;

private:
	vector<vec4> orderDodecahedron(const vector<vec4> &vertices, bool triangles) const;
	void addPentagonsToSequence(int point_index, const vector<vec4> &vertices, vector< vector<int> > &identified_pentagons, vector<vec4> &sequence, bool triangles) const;
	bool pentagonAlreadyIdentified(int index_a, int index_b, vector< vector<int> > &identified_pentagons) const;
	int findSideFromTargetAndHypotenuse(int target, int hypotenuse, const vector<vec4> &vertices) const;
	int findOtherHypotenuseFromTargetAndHypotenuse(int target, int hypotenuse, const vector<vec4> &vertices) const;
	void addPentagonToPointSequence(int a, int b, int c, int d, int e, const vector<vec4> vertices, vector<vec4> &sequence, bool triangles) const;
};