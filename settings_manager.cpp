#include "settings_manager.h"

void settings_manager::randomize(const matrix_creator &mc)
{
	generation = 0;

	refresh_value = int(mc.getRandomFloatInRange(refresh_min, refresh_max));
	num_matrices = int(mc.getRandomFloatInRange(3, 12));
	background_front_index = int(mc.getRandomFloatInRange(0, num_matrices));
	background_back_index = int(mc.getRandomFloatInRange(0, num_matrices));
	translate_weight = int(mc.getRandomFloatInRange(1, 10));
	rotate_weight = int(mc.getRandomFloatInRange(1, 10));
	scale_weight = int(mc.getRandomFloatInRange(1, 10));

	matrix_geometry_weights[TRIANGLE] = mc.getRandomIntInRange(0, 10);
	matrix_geometry_weights[U_RECTANGLE] = mc.getRandomIntInRange(0, 10);
	matrix_geometry_weights[U_SQUARE] = mc.getRandomIntInRange(0, 10);
	matrix_geometry_weights[U_CUBOID] = mc.getRandomIntInRange(0, 10);
	matrix_geometry_weights[U_CUBE] = mc.getRandomIntInRange(0, 10);
	matrix_geometry_weights[U_TETRAHEDRON] = mc.getRandomIntInRange(0, 10);
	matrix_geometry_weights[U_OCTAHEDRON] = mc.getRandomIntInRange(0, 10);
	matrix_geometry_weights[U_DODECAHEDRON] = mc.getRandomIntInRange(0, 10);
	matrix_geometry_weights[U_ICOSAHEDRON] = mc.getRandomIntInRange(0, 10);

	matrix_geometry_coefficient = mc.getRandomFloat();
	line_width = mc.getRandomFloat();
	alpha_min = mc.getRandomFloatInRange(0.0f, 0.5f);
	alpha_max = mc.getRandomFloatInRange(0.5f, 1.0f);
	bias_coefficient = mc.getRandomFloatInRange(0.1f, 0.3f);

	use_point_sequence = mc.getRandomFloat() < 0.5f;
	two_dimensional = mc.getRandomFloat() < 0.2f;
	refresh_enabled = mc.getRandomFloat() < 0.5f;
	size_enabled = mc.getRandomFloat() < 0.5f;
	show_points = mc.getRandomFloat() < 0.5f;
	enable_triangles = mc.getRandomFloat() < 0.4f;
	enable_lines = mc.getRandomFloat() < 0.4f;
	smooth_render = mc.getRandomFloat() < 0.8f;
	randomize_lightness = mc.getRandomFloat() < 0.8f;
	randomize_alpha = mc.getRandomFloat() < 0.8f;
	inverted = mc.getRandomFloat() < 0.5f;
	scale_matrices = mc.getRandomFloat() < 0.5f;
	show_growth = mc.getRandomFloat() < 0.25f;	
	no_background = mc.getRandomFloat() < 0.2f;

	GLfloat width_range[2];
	glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, width_range);
	glLineWidth(GLfloat(line_width) * width_range[1]);

	if (mc.getRandomFloat() < 0.8f)
	{
		color_palette synced_palette = color_palette((int)mc.getRandomFloatInRange(0, (int)DEFAULT_COLOR_PALETTE));
		palette_front = synced_palette;
		palette_back = synced_palette;
	}

	else
	{
		palette_front = color_palette((int)mc.getRandomFloatInRange(0, (int)DEFAULT_COLOR_PALETTE));
		palette_back = color_palette((int)mc.getRandomFloatInRange(0, (int)DEFAULT_COLOR_PALETTE));
	}

	int random_line_mode = int(mc.getRandomFloat() * 3.0f);	
	switch (random_line_mode)
	{
	case 0: line_mode = GL_LINES; break;
	case 1: line_mode = GL_LINE_STRIP; break;
	case 2: line_mode = 0; break;
	default: break;
	}

	int random_triangle_mode = int(mc.getRandomFloat() * 4.0f);
	switch (random_triangle_mode)
	{
	case 0: triangle_mode = GL_TRIANGLES; break;
	case 1: triangle_mode = GL_TRIANGLE_STRIP; break;
	case 2: triangle_mode = GL_TRIANGLE_FAN; break;
	case 3: triangle_mode = 0; break;
	default: break;
	}

	geo_type = geometry_type((int)mc.getRandomFloatInRange(0, (int)DEFAULT_GEOMETRY_TYPE));
	lm = lighting_mode((int)mc.getRandomFloatInRange(0, (int)LIGHTING_MODE_SIZE));

	if (use_point_sequence)
	{
		float random_width = mc.getRandomFloatInRange(0.2f, 1.0f);
		float random_height = mc.getRandomFloatInRange(0.2f, 1.0f);
		float random_depth = mc.getRandomFloatInRange(0.2f, 1.0f);

		switch (geo_type)
		{
		case TRIANGLE: point_sequence = gm.getTriangle(random_width); break;
		case RECTANGLE: point_sequence = gm.getRectangle(random_width, random_height); break;
		case SQUARE: point_sequence = gm.getSquare(random_width); break;
		case CUBOID: point_sequence = gm.getCuboid(random_width, random_height, random_depth); break;
		case CUBE: point_sequence = gm.getCube(random_width); break;
		case TETRAHEDRON: point_sequence = gm.getTetrahedron(random_width); break;
		case OCTAHEDRON: point_sequence = gm.getOctahedron(random_width); break;
		case DODECAHEDRON: point_sequence = gm.getDodecahedron(random_width); break;
		case ICOSAHEDRON: point_sequence = gm.getIcosahedron(random_width); break;
		case U_RECTANGLE: point_sequence = gm.getUnorderedRectangle(random_width, random_height); break;
		case U_SQUARE: point_sequence = gm.getUnorderedSquare(random_width); break;
		case U_CUBOID: point_sequence = gm.getUnorderedCuboid(random_width, random_height, random_depth); break;
		case U_CUBE: point_sequence = gm.getUnorderedCube(random_width); break;
		case U_TETRAHEDRON: point_sequence = gm.getUnorderedTetrahedron(random_width); break;
		case U_OCTAHEDRON: point_sequence = gm.getUnorderedOctahedron(random_width); break;
		case U_DODECAHEDRON: point_sequence = gm.getUnorderedDodecahedron(random_width); break;
		case U_ICOSAHEDRON: point_sequence = gm.getUnorderedIcosahedron(random_width); break;
		case LOADED_SEQUENCE: geo_type = DEFAULT_GEOMETRY_TYPE;
		case DEFAULT_GEOMETRY_TYPE:
		default: use_point_sequence = false;
		}
	}
}

string settings_manager::toString() const
{
	string encoded_string;

	encoded_string += std::to_string(refresh_value) + "_";
	encoded_string += std::to_string(num_matrices) + "_";
	encoded_string += std::to_string(background_front_index) + "_";
	encoded_string += std::to_string(background_back_index) + "_";
	encoded_string += std::to_string(num_matrices) + "_";
	encoded_string += std::to_string(translate_weight) + "_";
	encoded_string += std::to_string(rotate_weight) + "_";
	encoded_string += std::to_string(scale_weight) + "_";

	for (const auto &geo_pair : matrix_geometry_weights)
	{
		encoded_string += std::to_string(geo_pair.second) + "_";
	}

	encoded_string += parseFloat(matrix_geometry_coefficient) + "_";
	encoded_string += parseFloat(line_width) + "_";
	encoded_string += parseFloat(alpha_min) + "_";
	encoded_string += parseFloat(alpha_max) + "_";
	encoded_string += parseFloat(bias_coefficient) + "_";
	encoded_string += parseFloat(illumination_distance) + "_";

	encoded_string += std::to_string(use_point_sequence) + "_";
	encoded_string += std::to_string(two_dimensional) + "_";
	encoded_string += std::to_string(refresh_enabled) + "_";
	encoded_string += std::to_string(size_enabled) + "_";
	encoded_string += std::to_string(show_points) + "_";
	encoded_string += std::to_string(enable_triangles) + "_";
	encoded_string += std::to_string(enable_lines) + "_";
	encoded_string += std::to_string(smooth_render) + "_";
	encoded_string += std::to_string(randomize_lightness) + "_";
	encoded_string += std::to_string(randomize_alpha) + "_";
	encoded_string += std::to_string(inverted) + "_";
	encoded_string += std::to_string(scale_matrices) + "_";
	encoded_string += std::to_string(show_growth) + "_";

	encoded_string += std::to_string(palette_front) + "_";
	encoded_string += std::to_string(palette_back) + "_";
	encoded_string += std::to_string(line_mode) + "_";
	encoded_string += std::to_string(triangle_mode) + "_";
	encoded_string += std::to_string(geo_type) + "_";
	encoded_string += std::to_string(lm) + "_";
	encoded_string += std::to_string(use_point_sequence) + "_";
	encoded_string += std::to_string(no_background);

	return encoded_string;
}

void settings_manager::setWithString(string settings)
{

}

string settings_manager::parseFloat(float f) const
{
	string parsed = std::to_string(f);
	if (parsed.size() > 5)
		parsed.erase(5);

	return parsed;
}
