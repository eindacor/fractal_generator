#include "settings_manager.h"

void settings_manager::randomize(const matrix_creator &mc)
{
	background_front_index = 0;
	background_back_index = 0;
	generation = 0;

	refresh_value = int(mc.getRandomFloatInRange(refresh_min, refresh_max));
	num_matrices = int(mc.getRandomFloatInRange(3, 12));
	translate_weight = int(mc.getRandomFloatInRange(1, 10));
	rotate_weight = int(mc.getRandomFloatInRange(1, 10));
	scale_weight = int(mc.getRandomFloatInRange(1, 10));
	matrix_geometry_coefficient = mc.getRandomFloat();

	use_point_sequence = mc.getRandomFloat() < 0.5f;
	two_dimensional = mc.getRandomFloat() < 0.2f;
	refresh_enabled = mc.getRandomFloat() < 0.5f;
	size_enabled = mc.getRandomFloat() < 0.5f;
	show_points = true;
	enable_triangles = mc.getRandomFloat() < 0.4f;
	enable_lines = mc.getRandomFloat() < 0.4f;
	smooth_render = mc.getRandomFloat() < 0.8f;
	randomize_lightness = mc.getRandomFloat() < 0.8f;
	randomize_alpha = mc.getRandomFloat() < 0.8f;
	lighting_enabled = mc.getRandomFloat() < 0.5f;
	inverted = mc.getRandomFloat() < 0.5f;
	scale_matrices = mc.getRandomFloat() < 0.5f;
	line_width = mc.getRandomFloat();
	show_growth = mc.getRandomFloat() < 0.25f;

	alpha_min = mc.getRandomFloatInRange(0.0f, 0.5f);
	alpha_max = mc.getRandomFloatInRange(0.5f, 1.0f);

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
