#pragma once

#ifndef COLOR_MANAGER_H
#define COLOR_MANAGER_H

#include "header.h"
#include "matrix_creator.h"

class HSL
{
public:
	int H;
	float S;
	float L;

	HSL(int h, float s, float l)
	{
		H = h;
		S = s;
		L = l;
	}

	bool Equals(HSL hsl)
	{
		return (H == hsl.H) && (S == hsl.S) && (L == hsl.L);
	}

	string to_string() const { return std::to_string(H) + ", " + std::to_string(S) + ", " + std::to_string(L); }
};

class color_manager
{
public:
	color_manager();
	~color_manager() {};

	vec4 generateRandomColorFromSaturation(const matrix_creator &mc, const vec4 &seed) const;

	float calcSaturation(const vec4 &seed) const;
	int calcHue(const vec4 &seed) const;
	float calcLightness(const vec4 &seed) const;
	vec4 getRGBAFromHSL(const HSL &hsl, float alpha = 1.0f) const;
	HSL getHSLFromRGBA(const vec4 &seed) const;
	void adjustLightness(vec4 &color, float degree) const;
	void adjustSaturation(vec4 &color, float saturation) const;
	void nudgeSaturation(vec4 &color, float degree) const;

	vec4 getComplementary(const vec4 &color) const;
	vector<vec4> getSplitComplementary(const vec4 &color, int steps) const;
	vec4 getAnalogous(const vec4 &color, int steps) const;
	vector<vec4> getTriad(const vec4 &color) const;
	vector<vec4> getDoubleComplementary(const vec4 &color, int steps) const;

	int getNewHue(int hue, int dist) const;

private:

};

#endif