#include "color_manager.h"
#include "matrix_creator.h"

color_manager::color_manager()
{
	vec4 color_test(255.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 1.0f);
	cout << "rgba: " + toRGBAString(color_test) << endl;
	HSL hsl = getHSLFromRGBA(color_test);
	cout << "hsl: " + hsl.to_string() << endl;
	cout << "reverted: " + toRGBAString(getRGBAFromHSL(hsl, 1.0f)) << endl;

	cout << "complementary: " + toRGBAString(getComplementary(color_test)) << endl;

	vector<vec4> triad = getTriad(color_test);
	cout << "triad: " << endl;
	for (const auto &color : triad)
	{
		cout << toRGBAString(color) << endl;
	}

	vector<vec4> split = getSplitComplementary(color_test, 1);
	cout << "split complementary: " << endl;
	for (const auto &color : split)
	{
		cout << toRGBAString(color) << endl;
	}

	cout << "analogous: " + toRGBAString(getAnalogous(color_test, 2)) << endl;

	vector<vec4> double_comp = getDoubleComplementary(color_test, 2);
	cout << "double complementary: " << endl;
	for (const auto &color : double_comp)
	{
		cout << toRGBAString(color) << endl;
	}

	vector<vec4> mono_palette = getMonochromaticPalette(color_test, 5);
	cout << "monochromatic palette: " << endl;
	for (const auto &color : mono_palette)
	{
		cout << toRGBAString(color) << endl;
	}

	vector<vec4> ana_palette = getAnalogousPalette(color_test, 5);
	cout << "analogous palette: " << endl;
	for (const auto &color : ana_palette)
	{
		cout << toRGBAString(color) << endl;
	}

}

string color_manager::toRGBAString(const vec4 &color) const
{
	return "(" + std::to_string(int(255.0f * color.r)) + ", " + std::to_string(int(255.0f * color.g)) + ", " + std::to_string(int(255.0f * color.b)) + ", " + std::to_string(color.a) + ")";
}

vec4 color_manager::generateRandomColorFromSaturation(const matrix_creator &mc, const vec4 &seed) const
{
	float saturation = calcSaturation(seed);
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

	// from http://www.rapidtables.com/convert/color/hsl-to-rgb.htm
	/*float c = (1.0f - abs((2.0f * hsl.L) - 1.0f)) * hsl.S;	
	float x = c * float(1 - abs(int((float(hsl.H) / 60.0f)) % 2 - 1));
	float m = hsl.L - (c / 2.0f);
	vec3 rgb_prime;

	if (hsl.H < 60)
		rgb_prime = vec3(c, x, 0.0f);

	else if (hsl.H < 120)
		rgb_prime = vec3(x, c, 0.0f);

	else if (hsl.H < 180)
		rgb_prime = vec3(0.0f, c, x);

	else if (hsl.H < 240)
		rgb_prime = vec3(0.0f, x, c);

	else if (hsl.H < 300)
		rgb_prime = vec3(x, 0.0f, c);

	else rgb_prime = vec3(c, 0.0f, x);

	return vec4(rgb_prime.r + m, rgb_prime.g + m, rgb_prime.b + m, alpha);*/
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

vec4 color_manager::getComplementary(const vec4 &color) const
{
	HSL hsl = getHSLFromRGBA(color);
	int new_hue = hsl.H + 180 > 360 ? hsl.H - 180 : hsl.H + 180;
	hsl.H = new_hue;
	return getRGBAFromHSL(hsl, color.a);
}

vector<vec4> color_manager::getSplitComplementary(const vec4 &color, int steps) const
{
	vector<vec4> split_comp;

	vec4 complementary = getComplementary(color);
	HSL comp_hsl = getHSLFromRGBA(complementary);

	HSL sc1(getNewHue(comp_hsl.H, steps * 15), comp_hsl.S, comp_hsl.L);
	HSL sc2(getNewHue(comp_hsl.H, steps * -15), comp_hsl.S, comp_hsl.L);

	split_comp.push_back(getRGBAFromHSL(sc1));
	split_comp.push_back(getRGBAFromHSL(sc2));

	return split_comp;
}

vec4 color_manager::getAnalogous(const vec4 &color, int steps) const
{
	HSL hsl = getHSLFromRGBA(color);
	hsl.H = getNewHue(hsl.H, steps * 30);
	return getRGBAFromHSL(hsl);
}

vector<vec4> color_manager::getTriad(const vec4 &color) const
{
	vector<vec4> triad;

	HSL hsl = getHSLFromRGBA(color);

	HSL t1(getNewHue(hsl.H, 120), hsl.S, hsl.L);
	HSL t2(getNewHue(hsl.H, -120), hsl.S, hsl.L);

	triad.push_back(color);
	triad.push_back(getRGBAFromHSL(t1));
	triad.push_back(getRGBAFromHSL(t2));

	return triad;
}

vector<vec4> color_manager::getTetrad(const vec4 &color) const
{
	vector<vec4> tetrad;

	HSL hsl = getHSLFromRGBA(color);

	HSL t1(getNewHue(hsl.H, 60), hsl.S, hsl.L);
	HSL t2(getNewHue(hsl.H, 180), hsl.S, hsl.L);
	HSL t3(getNewHue(hsl.H, 240), hsl.S, hsl.L);

	tetrad.push_back(color);
	tetrad.push_back(getRGBAFromHSL(t1));
	tetrad.push_back(getRGBAFromHSL(t2));
	tetrad.push_back(getRGBAFromHSL(t3));

	return tetrad;
}

vector<vec4> color_manager::getSquare(const vec4 &color) const
{
	vector<vec4> square;

	HSL hsl = getHSLFromRGBA(color);

	HSL t1(getNewHue(hsl.H, 90), hsl.S, hsl.L);
	HSL t2(getNewHue(hsl.H, 180), hsl.S, hsl.L);
	HSL t3(getNewHue(hsl.H, 270), hsl.S, hsl.L);

	square.push_back(color);
	square.push_back(getRGBAFromHSL(t1));
	square.push_back(getRGBAFromHSL(t2));
	square.push_back(getRGBAFromHSL(t3));

	return square;
}

int color_manager::getNewHue(int hue, int dist) const
{
	hue = hue % 360;

	if (dist < 0)
		return hue + dist < 0 ? hue + (360 + dist) : hue + dist;

	else return hue + dist > 360 ? hue - (360 - dist) : hue + dist;
}

vector<vec4> color_manager::getDoubleComplementary(const vec4 &color, int steps) const
{
	vector<vec4> double_comp;

	vec4 analogous = getAnalogous(color, steps);

	double_comp.push_back(color);
	double_comp.push_back(analogous);
	double_comp.push_back(getComplementary(color));
	double_comp.push_back(getComplementary(analogous));

	return double_comp;
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
		colors.push_back(getRGBAFromHSL(new_color));
	}

	return colors;
}

vector<vec4> color_manager::getAnalogousPalette(const vec4 &color, int count) const
{
	vector<vec4> colors;

	for (int i = 0; i < count; i++)
	{
		int current_step = i % 2 == 0 ? i : i * -1;
		colors.push_back(getAnalogous(color, current_step));
	}

	return colors;
}

vector<vec4> color_manager::getTriadPalette(const vec4 &color, int count) const
{
	vector<vec4> triad = getTriad(color);
	vector<vec4> palette;

	if (3 == count)
		return triad;

	else if (count < 3)
	{
		for (int i = 0; i < count; i++)
		{
			palette.push_back(triad.at(i));
		}
	}

	else
	{
		int required_monochrome_steps = (count / 3) + 1;

		vector< vector<vec4> > triad_palettes;
		triad_palettes.push_back(getMonochromaticPalette(triad.at(0), required_monochrome_steps));
		triad_palettes.push_back(getMonochromaticPalette(triad.at(1), required_monochrome_steps));
		triad_palettes.push_back(getMonochromaticPalette(triad.at(2), required_monochrome_steps));

		for (int i = 0; i < count; i++)
		{
			int base_color = i % 3;
			int current_step = i / 3;
			palette.push_back(triad_palettes.at(base_color).at(current_step));
		}
	}

	return palette;
}

vector<vec4> color_manager::getTetradPalette(const vec4 &color, int count) const
{
	vector<vec4> tetrad = getTetrad(color);
	vector<vec4> palette;

	if (4 == count)
		return tetrad;

	else if (count < 4)
	{
		for (int i = 0; i < count; i++)
		{
			palette.push_back(tetrad.at(i));
		}
	}

	else
	{
		int required_monochrome_steps = (count / 4) + 1;

		vector< vector<vec4> > tetrad_palettes;
		tetrad_palettes.push_back(getMonochromaticPalette(tetrad.at(0), required_monochrome_steps));
		tetrad_palettes.push_back(getMonochromaticPalette(tetrad.at(1), required_monochrome_steps));
		tetrad_palettes.push_back(getMonochromaticPalette(tetrad.at(2), required_monochrome_steps));
		tetrad_palettes.push_back(getMonochromaticPalette(tetrad.at(3), required_monochrome_steps));

		for (int i = 0; i < count; i++)
		{
			int base_color = i % 4;
			int current_step = i / 4;
			palette.push_back(tetrad_palettes.at(base_color).at(current_step));
		}
	}

	return palette;
}

vector<vec4> color_manager::getSquarePalette(const vec4 &color, int count) const
{
	vector<vec4> square = getTetrad(color);
	vector<vec4> palette;

	if (4 == count)
		return square;

	else if (count < 4)
	{
		for (int i = 0; i < count; i++)
		{
			palette.push_back(square.at(i));
		}
	}

	else
	{
		int required_monochrome_steps = (count / 4) + 1;

		vector< vector<vec4> > square_palettes;
		square_palettes.push_back(getMonochromaticPalette(square.at(0), required_monochrome_steps));
		square_palettes.push_back(getMonochromaticPalette(square.at(1), required_monochrome_steps));
		square_palettes.push_back(getMonochromaticPalette(square.at(2), required_monochrome_steps));
		square_palettes.push_back(getMonochromaticPalette(square.at(3), required_monochrome_steps));

		for (int i = 0; i < count; i++)
		{
			int base_color = i % 4;
			int current_step = i / 4;
			palette.push_back(square_palettes.at(base_color).at(current_step));
		}
	}

	return palette;
}

vector<vec4> color_manager::getRandomPalette(const matrix_creator &mc, int count) const
{
	vector<vec4> palette;
	for (int i = 0; i < count; i++)
	{
		palette.push_back(mc.getRandomVec4());
	}

	return palette;
}