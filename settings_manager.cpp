#include "settings_manager.h"

// TODO now that settings have access to fg, remove dependency on mc
void settings_manager::randomize(const random_generator &mc)
{
	generation = 0;

	refresh_value = mc.getRandomIntInRange(refresh_min, refresh_max);
	num_matrices = mc.getRandomIntInRange(3, 12);
	background_front_index = mc.getRandomIntInRange(0, num_matrices);
	background_back_index = mc.getRandomIntInRange(0, num_matrices);
	translate_weight = mc.getRandomIntInRange(1, 5);
	rotate_weight = mc.getRandomIntInRange(1, 5);
	scale_weight = mc.getRandomIntInRange(1, 5);

	matrix_geometry_weights[TRIANGLE] = mc.getRandomIntInRange(0, 10);
	matrix_geometry_weights[RECTANGLE] = mc.getRandomIntInRange(0, 10);
	matrix_geometry_weights[SQUARE] = mc.getRandomIntInRange(0, 10);
	matrix_geometry_weights[CUBOID] = mc.getRandomIntInRange(0, 10);
	matrix_geometry_weights[CUBE] = mc.getRandomIntInRange(0, 10);
	matrix_geometry_weights[TETRAHEDRON] = mc.getRandomIntInRange(0, 10);
	matrix_geometry_weights[OCTAHEDRON] = mc.getRandomIntInRange(0, 10);
	matrix_geometry_weights[DODECAHEDRON] = mc.getRandomIntInRange(0, 10);
	matrix_geometry_weights[ICOSAHEDRON] = mc.getRandomIntInRange(0, 10);

	num_lights = mc.getRandomIntInRange(LIGHT_COUNT / 2, LIGHT_COUNT);

	// MATRIX CREATOR CALL RESERVATIONS	- INTEGER
	// calls to mc below are made to preserve the consistency of seeded generation throughout development
	// when another INTEGER is added to the settings, relocate one of the calls below to the section above
	//mc.getRandomIntInRange(0, 10); -> num_lights
	mc.getRandomIntInRange(0, 10);
	mc.getRandomIntInRange(0, 10);
	mc.getRandomIntInRange(0, 10);
	mc.getRandomIntInRange(0, 10);
	mc.getRandomIntInRange(0, 10);
	mc.getRandomIntInRange(0, 10);
	mc.getRandomIntInRange(0, 10);
	mc.getRandomIntInRange(0, 10);
	mc.getRandomIntInRange(0, 10);
	// END OF CALL RESERVATIONS

	matrix_geometry_coefficient = mc.getRandomFloat();
	line_width = mc.getRandomFloat();
	alpha_min = mc.getRandomFloatInRange(0.0f, 0.5f);
	alpha_max = mc.getRandomFloatInRange(0.5f, 1.0f);
	bias_coefficient = mc.getRandomFloatInRange(0.1f, 0.3f);

	// MATRIX CREATOR CALL RESERVATIONS	- FLOAT
	// calls to mc below are made to preserve the consistency of seeded generation throughout development
	// when another FLOAT is added to the settings, relocate one of the calls below to the section above
	mc.getRandomFloat();
	mc.getRandomFloat();
	mc.getRandomFloat();
	mc.getRandomFloat();
	mc.getRandomFloat();
	mc.getRandomFloat();
	mc.getRandomFloat();
	mc.getRandomFloat();
	mc.getRandomFloat();
	mc.getRandomFloat();
	// END OF CALL RESERVATIONS

	use_point_sequence = mc.getBool(0.2f);
	two_dimensional = mc.getBool(0.2f);
	refresh_enabled = mc.getBool(0.5f);
	size_enabled = mc.getBool(0.5f);
	show_points = mc.getBool(0.5f);
	enable_triangles = mc.getBool(0.4f);
	enable_lines = mc.getBool(0.4f);
	//smooth_render = !refresh_enabled || mc.getBool(0.8f);
	randomize_lightness = mc.getBool(0.8f);
	randomize_alpha = mc.getBool(0.8f);
	inverted = mc.getBool(0.5f);
	scale_matrices = mc.getBool(0.9f);
	mc.getBool(0.25f); // formerly show_growth
	no_background = mc.getBool(0.2f);
	//light_effects_transparency = mc.getRandomFloat() < 0.5f;

	// MATRIX CREATOR CALL RESERVATIONS	- BOOL
	// calls to mc below are made to preserve the consistency of seeded generation throughout development
	// when another BOOL is added to the settings, relocate one of the calls below to the section above
	mc.getBool(0.5f);
	mc.getBool(0.5f);
	mc.getBool(0.5f);
	mc.getBool(0.5f);
	mc.getBool(0.5f);
	mc.getBool(0.5f);
	mc.getBool(0.5f);
	mc.getBool(0.5f);
	mc.getBool(0.5f);
	mc.getBool(0.5f);
	// END OF CALL RESERVATIONS

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

	lm = lighting_mode(mc.getRandomIntInRange(0, (int)LIGHTING_MODE_SIZE));

	geo_type = geometry_type(mc.getRandomIntInRange(0, (int)GEOMETRY_TYPE_SIZE));
	// TODO refactor load sequence system
	/*if (geo_type == LOADED_SEQUENCE)
		geo_type = GEOMETRY_TYPE_SIZE;*/

	if (use_point_sequence)
	{
		float random_width = mc.getRandomFloatInRange(0.2f, 1.0f);
		float random_height = mc.getRandomFloatInRange(0.2f, 1.0f);
		float random_depth = mc.getRandomFloatInRange(0.2f, 1.0f);

		switch (geo_type)
		{
		case TRIANGLE: point_sequence = gm.getTriangleVertices(random_width); break;
		case RECTANGLE: point_sequence = gm.getRectangleVertices(random_width, random_height); break;
		case SQUARE: point_sequence = gm.getSquareVertices(random_width); break;
		case CUBOID: point_sequence = gm.getCuboidVertices(random_width, random_height, random_depth); break;
		case CUBE: point_sequence = gm.getCubeVertices(random_width); break;
		case TETRAHEDRON: point_sequence = gm.getTetrahedronVertices(random_width); break;
		case OCTAHEDRON: point_sequence = gm.getOctahedronVertices(random_width); break;
		case DODECAHEDRON: point_sequence = gm.getDodecahedronVertices(random_width); break;
		case ICOSAHEDRON: point_sequence = gm.getIcosahedronVertices(random_width); break;
		//case LOADED_SEQUENCE: geo_type = DEFAULT_GEOMETRY_TYPE;
		case GEOMETRY_TYPE_SIZE:
		default: use_point_sequence = false;
		}

		line_indices = gm.getIndices(geo_type, LINE_INDICES);
		triangle_indices = gm.getIndices(geo_type, TRIANGLE_INDICES);
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

	encoded_string += std::to_string(palette_front) + "_";
	encoded_string += std::to_string(palette_back) + "_";
	encoded_string += std::to_string(line_mode) + "_";
	encoded_string += std::to_string(triangle_mode) + "_";
	encoded_string += std::to_string(geo_type) + "_";
	encoded_string += std::to_string(lm) + "_";
	encoded_string += std::to_string(use_point_sequence) + "_";
	encoded_string += std::to_string(no_background) + "_";
	encoded_string += std::to_string(light_effects_transparency);

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
