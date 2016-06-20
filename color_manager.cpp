#include "color_manager.h"
#include "matrix_creator.h"

string color_manager::toRGBAString(const vec4 &color) const
{
	return "(" + std::to_string(int(255.0f * color.r)) + ", " + std::to_string(int(255.0f * color.g)) + ", " + std::to_string(int(255.0f * color.b)) + ", " + std::to_string(color.a) + ")";
}

float color_manager::calcSaturation(const vec4 &seed) const
{
	// from http://www.rapidtables.com/convert/color/rgb-to-hsl.htm
	float min_value = glm::min<float>(glm::min<float>(seed.r, seed.g), seed.b);
	float max_value = glm::max<float>(glm::max<float>(seed.r, seed.g), seed.b);
	float delta = max_value - min_value;
	float lightness = calcLightness(seed);

	if (0.0f + delta < .00001f)
		return 0.0f;

	else return delta / (1.0f - abs((2.0f * lightness) - 1.0f));
}

int color_manager::calcHue(const vec4 &seed) const
{
	float min_value = glm::min<float>(glm::min<float>(seed.r, seed.g), seed.b);
	float max_value = glm::max<float>(glm::max<float>(seed.r, seed.g), seed.b);

	float hue = 0.0f;
	if (max_value == seed.r) {
		hue = (seed.g - seed.b) / (max_value - min_value);

	}
	else if (max_value == seed.g) {
		hue = 2.0f + (seed.b - seed.r) / (max_value - min_value);

	}
	else {
		hue = 4.0f + (seed.r - seed.g) / (max_value - min_value);
	}

	hue = hue * 60.0f;
	if (hue < 0.0f) 
		hue = hue + 360.0f;

	return int(hue);
}

float color_manager::calcLightness(const vec4 &seed) const
{
	float min_value = glm::min<float>(glm::min<float>(seed.r, seed.g), seed.b);
	float max_value = glm::max<float>(glm::max<float>(seed.r, seed.g), seed.b);

	return (max_value + min_value) / 2.0f;
}

vec4 color_manager::getRGBAFromHSL(const HSL &hsl, float alpha) const
{
	//from http://www.had2know.com/technology/hsl-rgb-color-converter.html
	float d = hsl.S * (1.0f - abs((2.0f * hsl.L) - 1.0f));
	float m = hsl.L - (d * 0.5f);
	float x = d * float(1.0f - abs( ( glm::mod( (float(hsl.H) / 60.0f), 2.0f) ) - 1.0f));
	vec4 rgba;

	if (hsl.H < 60)
		rgba = vec4(d + m, x + m, m, alpha);

	else if (hsl.H < 120)
		rgba = vec4(x + m, d + m, m, alpha);

	else if (hsl.H < 180)
		rgba = vec4(m, d + m, x + m, alpha);

	else if (hsl.H < 240)
		rgba = vec4(m, x + m, d + m, alpha);

	else if (hsl.H < 300)
		rgba = vec4(x + m, m, d + m, alpha);

	else rgba = vec4(d + m, m, x + m, alpha);

	return rgba;
}

HSL color_manager::getHSLFromRGBA(const vec4 &seed) const
{
	return HSL(calcHue(seed), calcSaturation(seed), calcLightness(seed));
}

void color_manager::adjustSaturation(vec4 &color, float saturation) const
{
	HSL hsl = getHSLFromRGBA(color);
	hsl.S = saturation;
	color = getRGBAFromHSL(hsl, color.a);
}

void color_manager::nudgeSaturation(vec4 &seed, float degree) const
{
	float current_saturation = calcSaturation(seed);
	adjustSaturation(seed, current_saturation + degree);
}

void color_manager::adjustLightness(vec4 &color, float lightness) const
{
	lightness = glm::clamp(lightness, 0.0f, 1.0f);
	HSL hsl = getHSLFromRGBA(color);
	hsl.L = lightness;
	color = getRGBAFromHSL(hsl, color.a);
}

vec4 color_manager::getComplementaryColor(const vec4 &color) const
{
	HSL hsl = getHSLFromRGBA(color);
	int new_hue = hsl.H + 180 > 360 ? hsl.H - 180 : hsl.H + 180;
	hsl.H = new_hue;
	return getRGBAFromHSL(hsl, color.a);
}

vector<vec4> color_manager::getSplitComplementarySet(const vec4 &color) const
{
	vector<vec4> split_comp;

	vec4 complementary = getComplementaryColor(color);
	HSL comp_hsl = getHSLFromRGBA(complementary);

	HSL sc1(getNewHue(comp_hsl.H, 210), comp_hsl.S, comp_hsl.L);
	HSL sc2(getNewHue(comp_hsl.H, 150), comp_hsl.S, comp_hsl.L);

	split_comp.push_back(getRGBAFromHSL(sc1, color.a));
	split_comp.push_back(getRGBAFromHSL(sc2, color.a));

	return split_comp;
}

vec4 color_manager::getAnalogousColor(const vec4 &color, int steps) const
{
	HSL hsl = getHSLFromRGBA(color);
	hsl.H = getNewHue(hsl.H, steps * 30);
	return getRGBAFromHSL(hsl, color.a);
}

vector<vec4> color_manager::getTriadSet(const vec4 &color) const
{
	vector<vec4> triad;

	HSL hsl = getHSLFromRGBA(color);

	HSL t1(getNewHue(hsl.H, 120), hsl.S, hsl.L);
	HSL t2(getNewHue(hsl.H, -120), hsl.S, hsl.L);

	triad.push_back(color);
	triad.push_back(getRGBAFromHSL(t1, color.a));
	triad.push_back(getRGBAFromHSL(t2, color.a));

	return triad;
}

vector<vec4> color_manager::getTetradSet(const vec4 &color) const
{
	vector<vec4> tetrad;

	HSL hsl = getHSLFromRGBA(color);

	HSL t1(getNewHue(hsl.H, 60), hsl.S, hsl.L);
	HSL t2(getNewHue(hsl.H, 180), hsl.S, hsl.L);
	HSL t3(getNewHue(hsl.H, 240), hsl.S, hsl.L);

	tetrad.push_back(color);
	tetrad.push_back(getRGBAFromHSL(t1, color.a));
	tetrad.push_back(getRGBAFromHSL(t2, color.a));
	tetrad.push_back(getRGBAFromHSL(t3, color.a));

	return tetrad;
}

vector<vec4> color_manager::getSquareSet(const vec4 &color) const
{
	vector<vec4> square;

	HSL hsl = getHSLFromRGBA(color);

	HSL t1(getNewHue(hsl.H, 90), hsl.S, hsl.L);
	HSL t2(getNewHue(hsl.H, 180), hsl.S, hsl.L);
	HSL t3(getNewHue(hsl.H, 270), hsl.S, hsl.L);

	square.push_back(color);
	square.push_back(getRGBAFromHSL(t1, color.a));
	square.push_back(getRGBAFromHSL(t2, color.a));
	square.push_back(getRGBAFromHSL(t3, color.a));

	return square;
}

int color_manager::getNewHue(int hue, int dist) const
{
	hue = hue % 360;

	if (dist < 0)
		return hue + dist < 0 ? hue + (360 + dist) : hue + dist;

	else return hue + dist > 360 ? hue - (360 - dist) : hue + dist;
}

vector<vec4> color_manager::getMonochromaticPalette(const vec4 &color, int count) const
{
	vector<vec4> colors;
	HSL hsl = getHSLFromRGBA(color);

	float lightness_increment = 1.0f / float(count);
	for (int i = 0; i < count; i++)
	{
		float adjustment_distance = lightness_increment * float(i);
		float brightness = glm::mod(hsl.L + adjustment_distance, 1.0f);

		HSL new_color(hsl.H, hsl.S, brightness);
		colors.push_back(getRGBAFromHSL(new_color, color.a));
	}

	return colors;
}

vector<vec4> color_manager::getComplementaryPalette(const vec4 &color, int count) const
{
	vector<vec4> complementary_set({ color, getComplementaryColor(color) });
	return getPaletteFromColorSet(complementary_set, count);
}

vector<vec4> color_manager::getSplitComplementaryPalette(const vec4 &color, int count) const
{
	vector<vec4> split_complementary_set = getSplitComplementarySet(color);
	return getPaletteFromColorSet(split_complementary_set, count);
}

vector<vec4> color_manager::getAnalogousPalette(const vec4 &color, int count) const
{
	vector<vec4> colors;

	for (int i = 0; i < count; i++)
	{
		int current_step = i % 2 == 0 ? i : i * -1;
		colors.push_back(getAnalogousColor(color, current_step));
	}

	return colors;
}

vector<vec4> color_manager::getTriadPalette(const vec4 &color, int count) const
{
	return getPaletteFromColorSet(getTriadSet(color), count);
}

vector<vec4> color_manager::getTetradPalette(const vec4 &color, int count) const
{
	return getPaletteFromColorSet(getTetradSet(color), count);
}

vector<vec4> color_manager::getSquarePalette(const vec4 &color, int count) const
{
	return getPaletteFromColorSet(getSquareSet(color), count);
}

vector<vec4> color_manager::getRandomPalette(int count) const
{
	vector<vec4> palette;
	for (int i = 0; i < count; i++)
	{
		palette.push_back(mc.getRandomVec4());
	}

	return palette;
}

vector<vec4> color_manager::getPrimaryPalette(int count) const
{
	vector<vec4> primaries({
		vec4(1.0f, 0.0f, 0.0f, 1.0f),
		vec4(1.0f, 1.0f, 0.0f, 1.0f),
		vec4(0.0f, 0.0f, 1.0f, 1.0f),
	});

	return getPaletteFromColorSet(primaries, count);

}

vector<vec4> color_manager::getSecondaryPalette(int count) const
{
	vec4 red(1.0f, 0.0f, 0.0f, 1.0f);
	
	vector<vec4> base_palette = getPaletteFromEqualHueDivisions(red, 6);

	return getPaletteFromColorSet(base_palette, count);
}

vector<vec4> color_manager::getTertiaryPalette(int count) const
{
	vec4 red(1.0f, 0.0f, 0.0f, 1.0f);

	vector<vec4> base_palette = getPaletteFromEqualHueDivisions(red, 12);

	return getPaletteFromColorSet(base_palette, count);	
}

vector<vec4> color_manager::getPaletteFromColorSet(const vector<vec4> &base_set, int count) const
{
	vector<vec4> palette;

	if (base_set.size() == count)
		return base_set;

	else if (count < base_set.size())
	{
		for (int i = 0; i < count; i++)
		{
			palette.push_back(base_set.at(i));
		}
	}

	else
	{
		int required_monochrome_steps = (count / base_set.size()) + 1;

		vector< vector<vec4> > mono_palettes;

		for (int i = 0; i < base_set.size(); i++)
		{
			mono_palettes.push_back(getMonochromaticPalette(base_set.at(i), required_monochrome_steps));
		}

		for (int i = 0; i < count; i++)
		{
			int base_color = i % base_set.size();
			int current_step = i / base_set.size();
			palette.push_back(mono_palettes.at(base_color).at(current_step));
		}
	}

	return palette;
}

vector<vec4> color_manager::getPaletteFromEqualHueDivisions(const vec4 &color, int count) const
{
	vector<vec4> colors;

	int hue_offset = 360 / count;
	HSL hsl = getHSLFromRGBA(color);

	for (int i = 0; i < count; i++)
	{
		HSL new_color(getNewHue(hsl.H, i * hue_offset), hsl.S, hsl.L);
		colors.push_back(getRGBAFromHSL(new_color, color.a));
	}

	return colors;
}

void color_manager::modifySaturation(vector<vec4> &color_set, float saturation) const
{
	float clamped_saturation = glm::clamp(saturation, 0.0f, 1.0f);
	for (auto &color : color_set)
	{
		HSL hsl = getHSLFromRGBA(color);
		hsl.S = clamped_saturation;
		color = getRGBAFromHSL(hsl, color.a);
	}
}

void color_manager::modifyLightness(vector<vec4> &color_set, float amplification) const
{
	for (auto &color : color_set)
	{
		HSL hsl = getHSLFromRGBA(color);
		hsl.L = glm::clamp(hsl.L * amplification, 0.0f, 1.0f);
		color = getRGBAFromHSL(hsl, color.a);
	}
}

void color_manager::modifyAlpha(vector<vec4> &color_set, float alpha) const
{
	float clamped_alpha = glm::clamp(alpha, 0.0f, 1.0f);
	for (auto &color : color_set)
	{
		color.a = clamped_alpha;
	}
}

void color_manager::randomizeSaturation(vector<vec4> &color_set, float saturation_min, float saturation_max) const
{
	float clamped_min = glm::clamp(saturation_min, 0.0f, 1.0f);
	float clamped_max = glm::clamp(saturation_max, 0.0f, 1.0f);

	for (auto &color : color_set)
	{
		HSL hsl = getHSLFromRGBA(color);
		hsl.S = mc.getRandomFloatInRange(clamped_min, clamped_max);
		color = getRGBAFromHSL(hsl, color.a);
	}
}

void color_manager::randomizeLightness(vector<vec4> &color_set, float lightness_min, float lightness_max) const
{
	float clamped_min = glm::clamp(lightness_min, 0.0f, 1.0f);
	float clamped_max = glm::clamp(lightness_max, 0.0f, 1.0f);

	for (auto &color : color_set)
	{
		HSL hsl = getHSLFromRGBA(color);
		hsl.L = mc.getRandomFloatInRange(clamped_min, clamped_max);
		color = getRGBAFromHSL(hsl, color.a);
	}
}

void color_manager::randomizeAlpha(vector<vec4> &color_set, float alpha_min, float alpha_max) const
{
	float clamped_min = glm::clamp(alpha_min, 0.0f, 1.0f);
	float clamped_max = glm::clamp(alpha_max, 0.0f, 1.0f);

	for (auto &color : color_set)
	{
		color.a = mc.getRandomFloatInRange(clamped_min, clamped_max);
	}
}

string color_manager::getPaletteName(color_palette p) const
{
	switch (p)
	{
	case RANDOM_PALETTE: return "random palette";
	case DEFAULT_COLOR_PALETTE: return "default palette";
	case PRIMARY_PALETTE: return "primary palette";
	case SECONDARY_PALETTE: return "secondary palette";
	case TERTIARY_PALETTE: return "tertiary palette";
	case ANALOGOUS_PALETTE: return "analogous palette";
	case RANDOM_COLORS: return "random colors";
	case MONOCHROMATIC_PALETTE: return "monochromatic palette";
	case COMPLEMENTARY_PALETTE: return "complementary palette";
	case SPLIT_COMPLEMENTARY_PALETTE: return "split complementary palette";
	case TRIAD_PALETTE: return "triad palette";
	case TETRAD_PALETTE: return "tetrad palette";
	case SQUARE_PALETTE: return "square palette";
	default: return "unknown palette";
	}
}

vector<vec4> color_manager::generatePaletteFromSeed(const vec4 &seed, color_palette palette_type, int count, color_palette &random_palette_selected) const
{
	vector<vec4> color_vector;

	switch (palette_type)
	{
	case RANDOM_PALETTE: 
	{
		// random_palette_type + 1 to ensure DEFAULT_COLOR_PALETTE is possible and RANDOM_PALETTE cannot be called recursively
		int random_palette_type = int(mc.getRandomFloatInRange(0.0f, float(DEFAULT_COLOR_PALETTE))) + 1;		
		random_palette_selected = color_palette(random_palette_type);
		return generatePaletteFromSeed(seed, color_palette(random_palette_type), count, random_palette_selected);
	}
	case PRIMARY_PALETTE: return getPrimaryPalette(count);
	case SECONDARY_PALETTE: return getSecondaryPalette(count);
	case TERTIARY_PALETTE: return getTertiaryPalette(count);
	case ANALOGOUS_PALETTE: return getAnalogousPalette(seed, count);
	case RANDOM_COLORS: return getRandomPalette(count);
	case MONOCHROMATIC_PALETTE: return getMonochromaticPalette(seed, count);
	case COMPLEMENTARY_PALETTE: return getComplementaryPalette(seed, count);
	case SPLIT_COMPLEMENTARY_PALETTE: return getSplitComplementaryPalette(seed, count);
	case TRIAD_PALETTE: return getTriadPalette(seed, count);
	case TETRAD_PALETTE: return getTetradPalette(seed, count);
	case SQUARE_PALETTE: return getSquarePalette(seed, count);
	case DEFAULT_COLOR_PALETTE:
	default:
		for (int i = 0; i < count; i++)
		{
			color_vector.push_back(mc.getRandomVec4FromColorRanges(
				0.5f, 1.0f,		// red range
				0.5f, 1.0f,		// green range
				0.5f, 1.0f,		// blue range
				0.5f, 1.0f		// alpha range
				));
		}
		return color_vector;
	}
}
void color_manager::printColorSet(const vector<vec4> &set) const
{
	for (const auto &color : set)
	{
		cout << toRGBAString(color) << endl;
	}
}
