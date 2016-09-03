#include "settings_manager.h"

// TODO now that settings have access to fg, remove dependency on mc
// this method is run for each new fractal_generator object that is created. it does NOT run for every generation iteration of the fractal set
void settings_manager::randomize(const random_generator &rg)
{
	generation = 0;

	use_point_sequence = rg.getBool(0.2f);
	two_dimensional = rg.getBool(0.2f);
	refresh_enabled = rg.getBool(0.5f);
	rg.getBool(0.5f); // formerly size_enabled
	show_points = rg.getBool(0.5f);
	enable_triangles = rg.getBool(0.4f);
	enable_lines = rg.getBool(0.4f);
	//smooth_render = !refresh_enabled || mc.getBool(0.8f);
	randomize_lightness = rg.getBool(0.8f);
	randomize_alpha = rg.getBool(0.8f);
	inverted = rg.getBool(0.5f);
	scale_matrices = rg.getBool(0.9f);
	rg.getBool(0.25f); // formerly show_growth
	no_background = rg.getBool(0.2f);

	// MATRIX CREATOR CALL RESERVATIONS	- BOOL
	// calls to mc below are made to preserve the consistency of seeded generation throughout development
	// when another BOOL is added to the settings, relocate one of the calls below to the section above
	rg.getBool(0.5f);
	rg.getBool(0.5f);
	rg.getBool(0.5f);
	rg.getBool(0.5f);
	rg.getBool(0.5f);
	rg.getBool(0.5f);
	rg.getBool(0.5f);
	rg.getBool(0.5f);
	// END OF CALL RESERVATIONS

	refresh_value = rg.getRandomIntInRange(refresh_min, refresh_max);
	num_matrices = rg.getRandomIntInRange(3, 12);
	background_front_index = rg.getRandomIntInRange(0, num_matrices);
	background_back_index = rg.getRandomIntInRange(0, num_matrices);
	translate_weight = rg.getRandomIntInRange(1, 5);
	rotate_weight = rg.getRandomIntInRange(1, 5);
	scale_weight = rg.getRandomIntInRange(1, 5);

	int standard_matrix_weight_proportion = 100;
	int matrix_geometry_weight_pool = rg.getRandomIntInRange(standard_matrix_weight_proportion, standard_matrix_weight_proportion * GEOMETRY_ENUM_COUNT);

	vector<int> indexed_enumerated_geometry_types;
	for (int i = 0; i < GEOMETRY_ENUM_COUNT; i++)
	{
		indexed_enumerated_geometry_types.push_back(i);
	}

	std::random_shuffle(indexed_enumerated_geometry_types.begin(), indexed_enumerated_geometry_types.end());

	for (const int &index : indexed_enumerated_geometry_types)
	{
		matrix_geometry_weights[index] = rg.getRandomIntInRange(0, standard_matrix_weight_proportion > matrix_geometry_weight_pool ? matrix_geometry_weight_pool : standard_matrix_weight_proportion);
		matrix_geometry_weight_pool -= matrix_geometry_weights[index];
	}

	num_lights = rg.getRandomIntInRange(LIGHT_COUNT / 2, LIGHT_COUNT);
	point_sequence_index = rg.getRandomIntInRange(0, GEOMETRY_ENUM_COUNT);

	// MATRIX CREATOR CALL RESERVATIONS	- INTEGER
	// calls to mc below are made to preserve the consistency of seeded generation throughout development
	// when another INTEGER is added to the settings, relocate one of the calls below to the section above
	rg.getRandomIntInRange(0, 10);
	rg.getRandomIntInRange(0, 10);
	rg.getRandomIntInRange(0, 10);
	rg.getRandomIntInRange(0, 10);
	rg.getRandomIntInRange(0, 10);
	rg.getRandomIntInRange(0, 10);
	rg.getRandomIntInRange(0, 10);
	// END OF CALL RESERVATIONS

	matrix_geometry_coefficient = rg.getRandomFloat();
	alpha_min = rg.getRandomFloatInRange(0.0f, 0.5f);
	alpha_max = rg.getRandomFloatInRange(0.5f, 1.0f);
	bias_coefficient = rg.getRandomFloatInRange(0.1f, 0.3f);
	matrix_geometry_uses_solid_geometry_coefficient = rg.getRandomFloat();

	// MATRIX CREATOR CALL RESERVATIONS	- FLOAT
	// calls to mc below are made to preserve the consistency of seeded generation throughout development
	// when another FLOAT is added to the settings, relocate one of the calls below to the section above
	rg.getRandomFloat();
	rg.getRandomFloat();
	rg.getRandomFloat();
	rg.getRandomFloat();
	rg.getRandomFloat();
	rg.getRandomFloat();
	rg.getRandomFloat();
	rg.getRandomFloat();
	rg.getRandomFloat();
	rg.getRandomFloat();
	// END OF CALL RESERVATIONS

	GLfloat width_range[2];
	glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, width_range);
	glLineWidth(GLfloat(line_width) * width_range[1]);

	if (rg.getRandomFloat() < 0.8f)
	{
		color_palette synced_palette = color_palette((int)rg.getRandomFloatInRange(0, (int)DEFAULT_COLOR_PALETTE));
		palette_front = synced_palette;
		palette_back = synced_palette;
	}

	else
	{
		palette_front = color_palette((int)rg.getRandomFloatInRange(0, (int)DEFAULT_COLOR_PALETTE));
		palette_back = color_palette((int)rg.getRandomFloatInRange(0, (int)DEFAULT_COLOR_PALETTE));
	}

	int random_line_mode = int(rg.getRandomFloat() * 3.0f);	
	switch (random_line_mode)
	{
	case 0: line_mode = GL_LINES; break;
	case 1: line_mode = GL_LINE_STRIP; break;
	case 2: line_mode = 0; break;
	default: break;
	}

	int random_triangle_mode = int(rg.getRandomFloat() * 4.0f);
	switch (random_triangle_mode)
	{
	case 0: triangle_mode = GL_TRIANGLES; break;
	case 1: triangle_mode = GL_TRIANGLE_STRIP; break;
	case 2: triangle_mode = GL_TRIANGLE_FAN; break;
	case 3: triangle_mode = 0; break;
	default: break;
	}

	lm = lighting_mode(rg.getRandomIntInRange(0, (int)LIGHTING_MODE_SIZE));

	if (use_point_sequence)
	{
		setPointSequenceGeometry(point_sequence_index, rg);
	}
}

void settings_manager::setPointSequenceGeometry(int index, const random_generator &rg)
{
	if (index < GEOMETRY_TYPE_SIZE)
	{
		geometry_type gt = geometry_type(index);

		float random_width = rg.getRandomFloatInRange(0.2f, 1.0f);
		float random_height = rg.getRandomFloatInRange(0.2f, 1.0f);
		float random_depth = rg.getRandomFloatInRange(0.2f, 1.0f);

		switch (gt)
		{
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

		line_indices = gm.getSolidGeometryIndices(gt, LINE_INDICES);
		triangle_indices = gm.getSolidGeometryIndices(gt, TRIANGLE_INDICES);
	}

	else
	{
		index -= (int)GEOMETRY_TYPE_SIZE;
		ngon_type nt = ngon_type(index);
		int side_count = (int)nt + 3;
		point_sequence = gm.getNgonVertices(rg.getRandomFloatInRange(0.2f, 1.0f), side_count);

		line_indices = gm.getNgonIndices(nt, LINE_INDICES);
		triangle_indices = gm.getNgonIndices(nt, TRIANGLE_INDICES);
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

	encoded_string += parseFloat(matrix_geometry_coefficient) + "_";
	encoded_string += parseFloat(line_width) + "_";
	encoded_string += parseFloat(alpha_min) + "_";
	encoded_string += parseFloat(alpha_max) + "_";
	encoded_string += parseFloat(bias_coefficient) + "_";
	encoded_string += parseFloat(illumination_distance) + "_";

	encoded_string += std::to_string(use_point_sequence) + "_";
	encoded_string += std::to_string(two_dimensional) + "_";
	encoded_string += std::to_string(refresh_enabled) + "_";
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
