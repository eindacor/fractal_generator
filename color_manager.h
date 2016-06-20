#pragma once

#ifndef COLOR_MANAGER_H
#define COLOR_MANAGER_H

#include "header.h"
#include "matrix_creator.h"

enum color_palette { RANDOM_PALETTE, PRIMARY_PALETTE, SECONDARY_PALETTE, TERTIARY_PALETTE, RANDOM_COLORS, MONOCHROMATIC_PALETTE, COMPLEMENTARY_PALETTE, SPLIT_COMPLEMENTARY_PALETTE, TRIAD_PALETTE, TETRAD_PALETTE, SQUARE_PALETTE, DEFAULT_COLOR_PALETTE
};

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
	color_manager() {};
	color_manager(string seed) : mc(seed) {};
	~color_manager() {};

	float calcSaturation(const vec4 &seed) const;
	int calcHue(const vec4 &seed) const;
	float calcLightness(const vec4 &seed) const;
	vec4 getRGBAFromHSL(const HSL &hsl, float alpha) const;
	HSL getHSLFromRGBA(const vec4 &seed) const;
	void adjustLightness(vec4 &color, float degree) const;
	void adjustSaturation(vec4 &color, float saturation) const;
	void nudgeSaturation(vec4 &color, float degree) const;

	string toRGBAString(const vec4 &color) const;

	vec4 getComplementaryColor(const vec4 &color) const;
	vector<vec4> getSplitComplementarySet(const vec4 &color) const;
	vec4 getAnalogousColor(const vec4 &color, int steps) const;
	vector<vec4> getTriadSet(const vec4 &color) const;
	vector<vec4> getTetradSet(const vec4 &color) const;
	vector<vec4> getSquareSet(const vec4 &color) const;

	vector<vec4> getMonochromaticPalette(const vec4 &color, int count) const;
	vector<vec4> getComplementaryPalette(const vec4 &color, int count) const;
	vector<vec4> getSplitComplementaryPalette(const vec4 &color, int count) const;
	vector<vec4> getAnalogousPalette(const vec4 &color, int count) const;
	vector<vec4> getTriadPalette(const vec4 &color, int count) const;
	vector<vec4> getTetradPalette(const vec4 &color, int count) const;
	vector<vec4> getSquarePalette(const vec4 &color, int count) const;
	vector<vec4> getRandomPalette(int count) const;
	vector<vec4> getPrimaryPalette(int count) const;
	vector<vec4> getSecondaryPalette(int count) const;
	vector<vec4> getTertiaryPalette(int count) const;

	vector<vec4> getPaletteFromColorSet(const vector<vec4> &base_set, int count) const;
	vector<vec4> getPaletteFromEqualHueDivisions(const vec4 &color, int count) const;

	void modifySaturation(vector<vec4> &color_set, float saturation) const;
	void modifyLightness(vector<vec4> &color_set, float amplification) const;
	void modifyAlpha(vector<vec4> &color_set, float alpha) const;
	void randomizeSaturation(vector<vec4> &color_set, float saturation_min, float saturation_max) const;
	void randomizeLightness(vector<vec4> &color_set, float lightness_min, float lightness_max) const;
	void randomizeAlpha(vector<vec4> &color_set, float alpha_min, float alpha_max) const;

	string getPaletteName(color_palette p) const;

	int getNewHue(int hue, int dist) const;

	vector<vec4> generatePaletteFromSeed(const vec4 &seed, color_palette palette_type, int count) const;

	void printColorSet(const vector<vec4> &set) const;

	void seed(const string seed) { mc.seed(seed); }

private:
	matrix_creator mc;
};

#endif