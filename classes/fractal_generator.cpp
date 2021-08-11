#include "fractal_generator.h"

fractal_generator::fractal_generator(
	const string &randomization_seed,
	const shared_ptr<ogl_context> &con,
	int num_points)
{
	vertex_count = num_points;
	base_seed = randomization_seed;

	context = con;
	rg.seed(base_seed);
	color_man.seed(base_seed);
	sm.randomize(rg);
	generateLights();
	setMatrices();
	initialized = false;

	GLint range[2];
	glEnable(GL_PROGRAM_POINT_SIZE);
	glGetIntegerv(GL_ALIASED_POINT_SIZE_RANGE, range);
	max_point_size = range[1] < 1 ? min(context->getWindowHeight(), context->getWindowWidth()) : min(min(context->getWindowHeight(), context->getWindowWidth()), range[1]);

	glEnable(GL_DEPTH_CLAMP);
	glDepthRange(0.0, 1.0);
}

void fractal_generator::bufferData(const std::vector<float> &vertex_data, const std::vector<unsigned short> &line_indices_to_buffer, const std::vector<unsigned short> &triangle_indices_to_buffer)
{
	vertex_count = (vertex_data.size() / vertex_size);
	sm.enable_triangles = vertex_count >= 3;
	sm.enable_lines = vertex_count >= 2;

	// create/bind Vertex Array Object
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// create/bind Vertex Buffer Object
	glGenBuffers(1, &vertices_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_data.size(), &vertex_data[0], GL_STATIC_DRAW);

	// stride is the total size of each vertex's attribute data (position + color + size)
	// change this to 7 for triangle bug
	int stride = vertex_size * sizeof(float);

	// load position data
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, stride, (void*)0);

	// load color data
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(4 * sizeof(float)));

	// load point size
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));

	glGenBuffers(1, &line_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, line_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, line_indices_to_buffer.size() * sizeof(unsigned short), &line_indices_to_buffer[0], GL_STATIC_DRAW);
	line_index_count = line_indices_to_buffer.size();

	glGenBuffers(1, &triangle_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangle_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangle_indices_to_buffer.size() * sizeof(unsigned short), &triangle_indices_to_buffer[0], GL_STATIC_DRAW);
	triangle_index_count = triangle_indices_to_buffer.size();

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void fractal_generator::bufferPalette(const std::vector<float> &vertex_data)
{
	// create/bind Vertex Array Object
	glGenVertexArrays(1, &palette_vao);
	glBindVertexArray(palette_vao);

	// create/bind Vertex Buffer Object
	glGenBuffers(1, &palette_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, palette_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_data.size(), &vertex_data[0], GL_STATIC_DRAW);

	// 6 floats total -> 2 for position, 4 for color
	int stride = 6 * sizeof(float);

	// load color data
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)0);

	// load position data
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(4 * sizeof(float)));

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void fractal_generator::bufferLightData(const std::vector<float> &vertex_data)
{
	for (int i = 0; i < LIGHT_COUNT; i++)
	{
		if (i < light_indices.size())
		{
			int light_index = light_indices.at(i);
			int data_index = light_index * vertex_size;

			vec4 light_position(vertex_data.at(data_index), vertex_data.at(data_index + 1), vertex_data.at(data_index + 2), 1.0f);
			vec4 light_color(vertex_data.at(data_index + 4), vertex_data.at(data_index + 5), vertex_data.at(data_index + 6), 1.0f);

			light_positions[i] = light_position;
			light_colors[i] = light_color;
		}

		else
		{
			light_positions[i] = vec4(0.0f);
			light_colors[i] = vec4(0.0f);
		}
	}

	context->setUniform4fv("light_positions", LIGHT_COUNT, light_positions[0]);
	context->setUniform4fv("light_colors", LIGHT_COUNT, light_colors[0]);
}

void fractal_generator::drawFractal(shared_ptr<ogl_camera_flying> &camera) const
{
	// bind target VAO
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo);

	if (dof_enabled)
	{
		vec3 camera_vector = glm::normalize(camera->getFocus() - camera->getPosition());
		vec3 camera_right =  glm::normalize(glm::cross(vec3(camera_vector.x, 0.0f, camera_vector.z), vec3(0.0f, 1.0f, 0.0f)));
		vec3 camera_up = -1.0f * glm::normalize(glm::cross(camera_vector, camera_right));

		int geometry_passes = 0;
		geometry_passes = int(sm.enable_triangles && sm.triangle_mode != 0) + int(sm.enable_lines && sm.line_mode != 0) + int(sm.show_points);

		float total_passes = float(dof_passes * geometry_passes);
		bool accum_loaded = false;

		for (int i = 0; i < dof_passes; i++)
		{
			vec3 bokeh = camera_right * cos((float)i * 2.0f * PI / (float)dof_passes) + camera_up * sinf((float)i * 2.0f * PI / (float)dof_passes);
			glm::mat4 modelview = glm::lookAt(camera->getPosition() + dof_aperture * bokeh, camera->getFocus(), camera_up);
			glm::mat4 mvp = camera->getProjectionMatrix() * modelview;

			context->setUniformMatrix4fv("MVP", 1, GL_FALSE, mvp);

			if (sm.show_points)
			{
				drawVertices();
				glAccum(accum_loaded ? GL_ACCUM : GL_LOAD, 1.0f / total_passes);
				accum_loaded = true;
			}

			if (sm.enable_lines && sm.line_mode != 0)
			{
				drawLines();
				glAccum(accum_loaded ? GL_ACCUM : GL_LOAD, 1.0f / total_passes);
				accum_loaded = true;
			}

			if (sm.enable_triangles && sm.triangle_mode != 0)
			{
				drawTriangles();
				glAccum(accum_loaded ? GL_ACCUM : GL_LOAD, 1.0f / total_passes);
				accum_loaded = true;
			}
		}
		if (total_passes > 0.5f)
			glAccum(GL_RETURN, 1.0f / total_passes);
	}

	else
	{
		int geometry_passes = 0;
		geometry_passes = int(sm.enable_triangles && sm.triangle_mode != 0) + int(sm.enable_lines && sm.line_mode != 0) + int(sm.show_points);

		bool accum_loaded = false;

		if (sm.show_points)
		{
			drawVertices();
			glAccum(accum_loaded ? GL_ACCUM : GL_LOAD, 1.0f / float(geometry_passes));
			accum_loaded = true;
		}

		if (sm.enable_lines && sm.line_mode != 0)
		{
			drawLines();
			glAccum(accum_loaded ? GL_ACCUM : GL_LOAD, 1.0f / float(geometry_passes));
			accum_loaded = true;
		}

		if (sm.enable_triangles && sm.triangle_mode != 0)
		{
			drawTriangles();
			glAccum(accum_loaded ? GL_ACCUM : GL_LOAD, 1.0f / float(geometry_passes));
			accum_loaded = true;
		}

		if (geometry_passes != 0)
			glAccum(GL_RETURN, 1.0f / float(geometry_passes));
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (sm.show_palette)
	{
		glBindVertexArray(VAO);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(3);

		glBindBuffer(GL_ARRAY_BUFFER, palette_vbo);
		drawPalette();

		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(3);
	}

	glBindVertexArray(0);
}

void fractal_generator::drawVertices() const
{
	context->setUniform1i("geometry_type", 0);
	glDrawArrays(GL_POINTS, 0, show_growth ? glm::clamp(vertices_to_render, 0, vertex_count) : vertex_count);
}

void fractal_generator::drawLines() const
{
	context->setUniform1i("geometry_type", 1);
	if (sm.line_mode == GL_LINES)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, line_indices);
		glDrawElements(sm.line_mode, show_growth ? glm::clamp(vertices_to_render, 0, line_index_count) : line_index_count, GL_UNSIGNED_SHORT, (void*)0);
	}

	else
	{
		glDrawArrays(sm.line_mode, 0, show_growth ? glm::clamp(vertices_to_render, 0, vertex_count) : vertex_count);
	}
}

void fractal_generator::drawTriangles() const
{
	context->setUniform1i("geometry_type", 2);
	if (sm.triangle_mode == GL_TRIANGLES)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangle_indices);
		glDrawElements(sm.triangle_mode, show_growth ? glm::clamp(vertices_to_render, 0, triangle_index_count) : triangle_index_count, GL_UNSIGNED_SHORT, (void*)0);
	}

	else
	{
		glDrawArrays(sm.triangle_mode, 0, show_growth ? glm::clamp(vertices_to_render, 0, vertex_count) : vertex_count);
	}
}

void fractal_generator::drawPalette() const
{
	glBindBuffer(GL_ARRAY_BUFFER, palette_vbo);
	glDrawArrays(GL_TRIANGLES, 0, palette_vertex_count);
}

std::vector<std::pair<string, mat4>> fractal_generator::generateMatrixVector(const int &count, geometry_type &geo_type)
{
    std::vector<std::pair<string, mat4>> matrix_vector;

	if (rg.getRandomFloat() < sm.matrix_geometry_coefficient)
	{
		std::vector<vec4> point_sequence;
		int matrix_geometry_index;
		// TODO create mc exception class
		/*if (loaded_sequences.size() > 0)
			sm.matrix_geometry_weights[LOADED_SEQUENCE] = mc.getRandomIntInRange(0, loaded_sequences.size() * 10);*/

		if (!rg.catRoll<int>(sm.matrix_geometry_weights, matrix_geometry_index))
			throw;

		float random_width = rg.getRandomFloatInRange(0.2f, 1.0f);
		float random_height = rg.getRandomFloatInRange(0.2f, 1.0f);
		float random_depth = rg.getRandomFloatInRange(0.2f, 1.0f);

		if (matrix_geometry_index < GEOMETRY_TYPE_SIZE)
		{
			geometry_type gt = geometry_type(matrix_geometry_index);

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
			default: throw;
			}
		}

		else
		{
			matrix_geometry_index -= (int)GEOMETRY_TYPE_SIZE;
			ngon_type nt = ngon_type(matrix_geometry_index);
			int side_count = (int)nt + 3;
			point_sequence = gm.getNgonVertices(rg.getRandomFloatInRange(0.2f, 1.0f), side_count);
		}

		for (int i = 0; i < count; i++)
		{
			vec3 vertex(point_sequence.at(i % point_sequence.size()));
			matrix_vector.push_back(std::pair<string, mat4>("translate (" + std::to_string(matrix_geometry_index) + ")", glm::translate(mat4(1.0f), vertex)));
		}
	}

	else
	{
		geo_type = GEOMETRY_TYPE_SIZE;
		int matrix_type;

		for (int i = 0; i < count; i++)
		{
			short matrix_type;
			std::map<short, unsigned int> matrix_map;
			matrix_map[0] = sm.translate_weight;
			matrix_map[1] = sm.rotate_weight;
			matrix_map[2] = sm.scale_matrices ? sm.scale_weight : 0;

			// TODO create mc exception class
			if (!rg.catRoll<short>(matrix_map, matrix_type))
				throw;

			string matrix_category;
			mat4 matrix_to_add;

			switch (matrix_type)
			{
			case 0:
				matrix_to_add = sm.two_dimensional ? rg.getRandomTranslation2D() : rg.getRandomTranslation();
				matrix_category = "translate";
				break;
			case 1:
				matrix_to_add = sm.two_dimensional ? rg.getRandomRotation2D() : rg.getRandomRotation();
				matrix_category = "rotate";
				break;
			case 2:
				matrix_to_add = sm.two_dimensional ? rg.getRandomScale2D() : rg.getRandomScale();
				matrix_category = "scale";
				break;
			default: break;
			}

			matrix_vector.push_back(std::pair<string, mat4>(matrix_category, matrix_to_add));
		}
	}

	return matrix_vector;
}

std::vector<vec4> fractal_generator::generateColorVector(const vec4 &seed, color_palette palette, const int &count, color_palette &random_selection) const
{
	std::vector<vec4> color_set;

	color_set = color_man.generatePaletteFromSeed(seed, palette, count, random_selection);

	if (sm.randomize_alpha)
		color_man.randomizeAlpha(color_set, sm.alpha_min, sm.alpha_max);

	if (sm.randomize_lightness)
		color_man.modifyLightness(color_set, rg.getRandomFloatInRange(0.3, 1.2f));

	return color_set;
}

std::vector<float> fractal_generator::generateSizeVector(const int &count) const
{
	std::vector<float> size_vector;

	for (int i = 0; i < count; i++)
	{
		size_vector.push_back(rg.getRandomFloatInRange(POINT_SCALE_MIN, POINT_SCALE_MAX));
	}

	return size_vector;
}

// this method is run once and only once per fractal gen object
void fractal_generator::setMatrices()
{
	generation_seed = base_seed + "_" + std::to_string(sm.generation);
	rg.seed(generation_seed);
	color_man.seed(generation_seed);

	for (int i = 0; i < vertex_count; i++)
	{
		matrix_sequence_front.push_back(int(rg.getRandomFloatInRange(0.0f, float(sm.num_matrices))));
		matrix_sequence_back.push_back(int(rg.getRandomFloatInRange(0.0f, float(sm.num_matrices))));
	}

	int random_palette_index = int(rg.getRandomFloatInRange(0.0f, float(DEFAULT_COLOR_PALETTE)));
	sm.palette_front = color_palette(random_palette_index);
	sm.palette_back = color_palette(random_palette_index);

	//TODO add .reserve() for each std::vector
	matrices_front.clear();
	colors_front.clear();
	sizes_front.clear();

	matrices_back.clear();
	colors_back.clear();
	sizes_back.clear();

	//front data set
	matrices_front = generateMatrixVector(sm.num_matrices, geo_type_front);
	seed_color_front = rg.getRandomVec4FromColorRanges(
		0.0f, 1.0f,		// red range
		0.0f, 1.0f,		// green range
		0.0f, 1.0f,		// blue range
		sm.alpha_min, sm.alpha_max		// alpha range
		);
	colors_front = generateColorVector(seed_color_front, sm.palette_front, sm.num_matrices, sm.random_palette_front);
	sizes_front = generateSizeVector(sm.num_matrices);
	
	//back data set
	sm.generation++;
	generation_seed = base_seed + "_" + std::to_string(sm.generation);
	rg.seed(generation_seed);
	color_man.seed(generation_seed);
	matrices_back = generateMatrixVector(sm.num_matrices, geo_type_back);
	//std::random_shuffle(matrices_front.begin(), matrices_front.end());
	seed_color_back = rg.getRandomVec4FromColorRanges(
		0.0f, 1.0f,		// red range
		0.0f, 1.0f,		// green range
		0.0f, 1.0f,		// blue range
		sm.alpha_min, sm.alpha_max		// alpha range
		);
	colors_back = generateColorVector(seed_color_back, sm.palette_back, sm.num_matrices, sm.random_palette_back);
	sizes_back = generateSizeVector(sm.num_matrices);
}

void fractal_generator::swapMatrices() 
{
	if (sm.reverse)
		sm.generation--;

	else sm.generation++;

	generation_seed = base_seed + "_" + std::to_string(sm.generation);
	rg.seed(generation_seed);

	//TODO use pointers instead of loaded if statements
	if (!sm.reverse)
	{
		matrices_back = matrices_front;
		std::vector<unsigned int> matrix_sequence_temp(matrix_sequence_back);
		matrix_sequence_back = matrix_sequence_front;
		matrix_sequence_front = matrix_sequence_temp;
		colors_back = colors_front;
		sizes_back = sizes_front;
		seed_color_back = seed_color_front;
		sm.background_back_index = sm.background_front_index;
		sm.background_front_index = rg.getRandomIntInRange(0, colors_front.size());

		seed_color_front = rg.getRandomVec4FromColorRanges(
			0.0f, 1.0f,		// red range
			0.0f, 1.0f,		// green range
			0.0f, 1.0f,		// blue range
			sm.alpha_min, sm.alpha_max		// alpha range
			);

		matrices_front = generateMatrixVector(matrices_back.size(), geo_type_front);
		colors_front = generateColorVector(seed_color_front, sm.palette_front, matrices_back.size(), sm.random_palette_front);
		sizes_front = generateSizeVector(matrices_back.size());
	}

	else
	{
		matrices_front = matrices_back;
		std::vector<unsigned int> matrix_sequence_temp(matrix_sequence_front);
		matrix_sequence_front = matrix_sequence_back;
		matrix_sequence_back = matrix_sequence_temp;
		colors_front = colors_back;
		sizes_front = sizes_back;
		seed_color_front = seed_color_back;
		sm.background_front_index = sm.background_back_index;
		sm.background_back_index = rg.getRandomIntInRange(0, colors_back.size());

		seed_color_back = rg.getRandomVec4FromColorRanges(
			0.0f, 1.0f,		// red range
			0.0f, 1.0f,		// green range
			0.0f, 1.0f,		// blue range
			sm.alpha_min, sm.alpha_max		// alpha range
			);

		matrices_back = generateMatrixVector(matrices_front.size(), geo_type_back);
		colors_back = generateColorVector(seed_color_back, sm.palette_back, matrices_front.size(), sm.random_palette_back);
		sizes_back = generateSizeVector(matrices_front.size());
	}

	if (sm.print_context_on_swap)
		printContext();
}

void fractal_generator::cycleColorPalette()
{
	// palettes separated in case these change independently at some point
	if (sm.palette_front == DEFAULT_COLOR_PALETTE)
		sm.palette_front = color_palette(0);

	else sm.palette_front = color_palette(int(sm.palette_front) + 1);

	if (sm.palette_back == DEFAULT_COLOR_PALETTE)
		sm.palette_back = color_palette(0);

	else sm.palette_back = color_palette(int(sm.palette_back) + 1);
}

void fractal_generator::printMatrices() const
{
	std::cout << "-----matrices_front-----" << std::endl;
	for (const auto &matrix_pair : matrices_front)
	{
		std::cout << matrix_pair.first << std::endl;
		std::cout << glm::to_string(matrix_pair.second) << std::endl;
		std::cout << "----------" << std::endl;
	}

	std::cout << "-----matrices_back-----" << std::endl;
	for (const auto &matrix_pair : matrices_back)
	{
		std::cout << matrix_pair.first << std::endl;
		std::cout << glm::to_string(matrix_pair.second) << std::endl;
		std::cout << "----------" << std::endl;
	}

	std::cout << "------------------" << std::endl;
}

void fractal_generator::generateFractalFromPointSequence()
{
	std::vector<float> points;
	std::vector<unsigned short> line_indices_to_buffer;
	std::vector<unsigned short> triangle_indices_to_buffer;
	points.reserve(vertex_count * vertex_size);

	int num_matrices = matrices_front.size();

	vec4 point_color = sm.inverted ? vec4(0.0f, 0.0f, 0.0f, 1.0f) : vec4(1.0f);
	float starting_size = POINT_SCALE_MAX;

	mat4 origin_matrix = glm::scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));

	int current_sequence_index_lines = 0;
	int current_sequence_index_triangles = 0;

	for (int i = 0; i < vertex_count / sm.point_sequence.size(); i++)
	{
		int matrix_index_front = sm.smooth_render ? matrix_sequence_front.at(i) : int(rg.getRandomFloatInRange(0.0f, float(matrices_front.size())));
		int matrix_index_back = sm.smooth_render ? matrix_sequence_back.at(i) : int(rg.getRandomFloatInRange(0.0f, float(matrices_front.size())));
		vec4 transformation_color = influenceElement<vec4>(colors_back.at(matrix_index_back), colors_front.at(matrix_index_front), sm.interpolation_state);
		float transformation_size = influenceElement<float>(sizes_back.at(matrix_index_back), sizes_front.at(matrix_index_front), sm.interpolation_state);

		addPointSequenceAndIterate(origin_matrix, point_color, starting_size, matrix_index_front, matrix_index_back, points, line_indices_to_buffer, triangle_indices_to_buffer, current_sequence_index_lines, current_sequence_index_triangles);
	}

	addPalettePointsAndBufferData(points, line_indices_to_buffer, triangle_indices_to_buffer);
}

void fractal_generator::generateFractal()
{
	std::vector<float> points;
	std::vector<unsigned short> line_indices_to_buffer;
	std::vector<unsigned short> triangle_indices_to_buffer;
	points.reserve(vertex_count * vertex_size);

	int num_matrices = matrices_front.size();

	vec4 starting_point = origin;
	vec4 point_color = sm.inverted ? vec4(0.0f, 0.0f, 0.0f, 1.0f) : vec4(1.0f);
	float starting_size = POINT_SCALE_MAX;

	for (int i = 0; i < vertex_count && num_matrices > 0; i++)
	{
		int matrix_index_front = sm.smooth_render ? matrix_sequence_front.at(i) : int(rg.getRandomFloatInRange(0.0f, float(matrices_front.size())));
		int matrix_index_back = sm.smooth_render ? matrix_sequence_back.at(i) : int(rg.getRandomFloatInRange(0.0f, float(matrices_front.size())));

		addNewPointAndIterate(starting_point, point_color, starting_size, matrix_index_front, matrix_index_back, points);
		line_indices_to_buffer.push_back(line_indices_to_buffer.size());
		triangle_indices_to_buffer.push_back(triangle_indices_to_buffer.size());
	}

	addPalettePointsAndBufferData(points, line_indices_to_buffer, triangle_indices_to_buffer);
}

void fractal_generator::generateFractalWithRefresh()
{
	std::vector<float> points;
	std::vector<unsigned short> line_indices_to_buffer;
	std::vector<unsigned short> triangle_indices_to_buffer;
	points.reserve(vertex_count * vertex_size);

	int num_matrices = matrices_front.size();
	signed int actual_refresh = sm.refresh_value == -1 ? int(rg.getRandomFloatInRange(sm.refresh_min, sm.refresh_max)) : sm.refresh_value;

	for (int i = 0; i < vertex_count && num_matrices > 0; i++)
	{
		vec4 point_color = sm.inverted ? vec4(0.0f, 0.0f, 0.0f, 1.0f) : vec4(1.0f);
		vec4 new_point = origin;
		float new_size = POINT_SCALE_MAX;

		for (int n = 0; n < actual_refresh; n++)
		{
			int matrix_index_front = sm.smooth_render ? matrix_sequence_front.at((i + n) % matrix_sequence_front.size()) : int(rg.getRandomFloatInRange(0.0f, float(matrices_front.size())));
			int matrix_index_back = sm.smooth_render ? matrix_sequence_back.at((i + n) % matrix_sequence_back.size()) : int(rg.getRandomFloatInRange(0.0f, float(matrices_back.size())));

			mat4 matrix_front = matrices_front.at(matrix_index_front).second;
			mat4 matrix_back = matrices_back.at(matrix_index_back).second;
			vec4 point_front = matrix_front * new_point;
			vec4 point_back = matrix_back * new_point;

			sm.interpolation_state = glm::clamp(sm.interpolation_state, 0.0f, 1.0f);

			vec4 transformation_color = influenceElement<vec4>(colors_back.at(matrix_index_back), colors_front.at(matrix_index_front), sm.interpolation_state);
			float transformation_size = influenceElement<float>(sizes_back.at(matrix_index_back), sizes_front.at(matrix_index_front), sm.interpolation_state);
			new_point = influenceElement<vec4>(point_back, point_front, sm.interpolation_state);

			point_color += transformation_color;
    			new_size += transformation_size;
		}

		point_color /= ((float)actual_refresh + 1.0f);
		new_size /= ((float)actual_refresh + 1.0f);

		addNewPoint(new_point, point_color, new_size, points);
		line_indices_to_buffer.push_back(line_indices_to_buffer.size());
		triangle_indices_to_buffer.push_back(triangle_indices_to_buffer.size());
	}

	addPalettePointsAndBufferData(points, line_indices_to_buffer, triangle_indices_to_buffer);
}

void fractal_generator::generateFractalFromPointSequenceWithRefresh()
{
	std::vector<float> points;
	std::vector<unsigned short> line_indices_to_buffer;
	std::vector<unsigned short> triangle_indices_to_buffer;
	points.reserve(vertex_count * vertex_size);

	int num_matrices = matrices_front.size();

	signed int actual_refresh = sm.refresh_value == -1 ? int(rg.getRandomFloatInRange(sm.refresh_min, sm.refresh_max)) : sm.refresh_value;

	int current_sequence_index_lines = 0;
	int current_sequence_index_triangles = 0;

	for (int i = 0; i < vertex_count / sm.point_sequence.size() && num_matrices > 0; i++)
	{
		vec4 final_color = sm.inverted ? vec4(0.0f, 0.0f, 0.0f, 1.0f) : vec4(1.0f);
		float final_size = POINT_SCALE_MAX;
		mat4 final_matrix = glm::scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));

		for (int n = 0; n < actual_refresh; n++)
		{
			int matrix_index_front = sm.smooth_render ? matrix_sequence_front.at((i + n) % matrix_sequence_front.size()) : int(rg.getRandomFloatInRange(0.0f, float(matrices_front.size())));
			int matrix_index_back = sm.smooth_render ? matrix_sequence_back.at((i + n) % matrix_sequence_back.size()) : int(rg.getRandomFloatInRange(0.0f, float(matrices_back.size())));

			mat4 matrix_front = matrices_front.at(matrix_index_front).second;
			mat4 matrix_back = matrices_back.at(matrix_index_back).second;
			mat4 interpolated_matrix = influenceElement<mat4>(matrix_back, matrix_front, sm.interpolation_state);
			vec4 transformation_color = influenceElement<vec4>(colors_back.at(matrix_index_back), colors_front.at(matrix_index_front), sm.interpolation_state);
			float transformation_size = influenceElement<float>(sizes_back.at(matrix_index_back), sizes_front.at(matrix_index_front), sm.interpolation_state);

			final_matrix = interpolated_matrix * final_matrix;
			final_color += transformation_color;
			final_size += transformation_size;
		}

		final_color /= float(actual_refresh + 1);
		final_size /= float(actual_refresh + 1);

		int index_sequences_added = points.size() / (sm.point_sequence.size() * vertex_size);

		// determine where values should begin based on the used sequence and number of indices already added

		std::vector<int>::iterator max_local_value_lines = std::max_element(sm.line_indices.begin(), sm.line_indices.end());
		int starting_index_lines = index_sequences_added * (*max_local_value_lines + 1);
		for (const unsigned short index : sm.line_indices)
		{
			line_indices_to_buffer.push_back(starting_index_lines + index);
		}

		std::vector<int>::iterator max_local_value_triangles = std::max_element(sm.triangle_indices.begin(), sm.triangle_indices.end());
		int starting_index_triangles = index_sequences_added * (*max_local_value_triangles + 1);
		for (const unsigned short index : sm.triangle_indices)
		{
			triangle_indices_to_buffer.push_back(starting_index_triangles + index);
		}

		for (int n = 0; n < sm.point_sequence.size(); n++)
		{
			addNewPoint(final_matrix * sm.point_sequence.at(n), final_color, final_size, points);
		}
	}

	addPalettePointsAndBufferData(points, line_indices_to_buffer, triangle_indices_to_buffer);
}


void fractal_generator::renderFractal(const int &image_width, const int &image_height, const int &matrix_sequence_count)
{
	char cFileName[64];
	FILE *fScreenshot;

	int nSize = image_width * image_height * 3;
	GLubyte *pixels = new GLubyte[nSize];
	if (pixels == NULL) return;

	int nShot = 0;

	while (nShot < 64)
	{
		sprintf(cFileName, "screenshot_%d.tga", nShot);
		fScreenshot = fopen(cFileName, "rb");
		if (fScreenshot == NULL) break;
		else fclose(fScreenshot);

		++nShot;

		if (nShot > 63)
		{
			std::cout << "Screenshot limit of 64 reached. Remove some shots if you want to take more." << std::endl;
			return;
		}
	}

	fScreenshot = fopen(cFileName, "wb");

	std::vector<mat4> matrix_sequence = generateMatrixSequence(10);
	std::map<int, int> calc_map;
	vec4 white(1.0f, 1.0f, 1.0f, 1.0f);
	mat4 scale_matrix = glm::scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));

	//convert to BGR format    
	unsigned char temp;
	for (int i = 0; i < nSize; i += 3)
	{
		unsigned int calc_counter = 0;
		int x_coord = (i/3) % image_width;
		int y_coord = (i/3) / image_width;
		float x_pos = ((float)x_coord / (float)image_width);
		float y_pos = ((float)y_coord / (float)image_height);
		vec4 uv_point((x_pos * 2.0f) - 1.0f, (y_pos * 2.0f) - 1.0f, 0.0f, 1.0f);

		float x = 0.0f;
		float y = 0.0f;

		while (uv_point.x < 1.1f && uv_point.x > -1.1f  && uv_point.y < 1.1f  && uv_point.y > -1.1f && calc_counter < 1000)
		{
			mat4 current_matrix = scale_matrix * matrix_sequence.at(calc_counter % matrix_sequence.size());
			uv_point = current_matrix * uv_point;

			calc_counter++;
		}

		if (calc_map.find(calc_counter) == calc_map.end())
			calc_map[calc_counter] = 1;

		else calc_map[calc_counter] += 1;

		//dependend on fractal seed, replace
		int color_index = calc_counter / 100;
		float color_value = (float)color_index / 10.0f;

		pixels[i] = GLubyte(color_value * 255.0f);
		pixels[i+1] = GLubyte(color_value * 255.0f);
		pixels[i+2] = GLubyte((1.0f - color_value) * 255.0f);

		calc_counter = 0;
	}

	for (const auto &calc_pair : calc_map)
	{
		std::cout << calc_pair.first << " calcs: " << calc_pair.second << std::endl;
	}

	unsigned char TGAheader[12] = { 0,0,2,0,0,0,0,0,0,0,0,0 };
	unsigned char header[6] = {
	        (unsigned char)(image_width % 256),
	        (unsigned char)(image_width / 256),
	        (unsigned char)(image_height % 256),
	        (unsigned char)(image_height / 256),
	        24,
	        0 };

	fwrite(TGAheader, sizeof(unsigned char), 12, fScreenshot);
	fwrite(header, sizeof(unsigned char), 6, fScreenshot);
	fwrite(pixels, sizeof(GLubyte), nSize, fScreenshot);
	fclose(fScreenshot);

	delete[] pixels;

	return;
}

std::vector<mat4> fractal_generator::generateMatrixSequence(const int &sequence_size) const
{
	std::vector<mat4> matrix_sequence;

	for (int i = 0; i < sequence_size; i++)
	{
		int random_index = rg.getRandomUniform() * (float)matrices_front.size();
		matrix_sequence.push_back(matrices_front.at(random_index).second);
	}

	//return matrix_sequence;

	std::vector<mat4> dummy_sequence = {
		glm::translate(mat4(1.0f), vec3(0.01f, 0.01f, 0.0f)),
		glm::rotate(mat4(1.0f), 0.05f, vec3(0.0f, 0.0f, 1.0f)),
		glm::translate(mat4(1.0f), vec3(0.02f, -0.01f, 0.0f)),
		glm::translate(mat4(1.0f), vec3(-0.01f, 0.02f, 0.0f)),
		glm::rotate(mat4(1.0f), 0.02f, vec3(0.0f, 0.0f, 1.0f))
	};

	return dummy_sequence;
}

void fractal_generator::addNewPointAndIterate(
	vec4 &starting_point,
	vec4 &starting_color,
	float &starting_size,
	int matrix_index_front,
	int matrix_index_back,
	std::vector<float> &points)
{
	mat4 matrix_front = matrices_front.at(matrix_index_front).second;
	mat4 matrix_back = matrices_back.at(matrix_index_back).second;
	vec4 point_front = matrix_front * starting_point;
	vec4 point_back = matrix_back * starting_point;

	vec4 matrix_color_front = influenceElement<vec4>(starting_color, colors_front.at(matrix_index_front), sm.bias_coefficient);
	vec4 matrix_color_back = influenceElement<vec4>(starting_color, colors_back.at(matrix_index_back), sm.bias_coefficient);

	float point_size_front = influenceElement<float>(starting_size, sizes_front.at(matrix_index_front), sm.bias_coefficient);
	float point_size_back = influenceElement<float>(starting_size, sizes_back.at(matrix_index_back), sm.bias_coefficient);

	starting_point = influenceElement<vec4>(point_back, point_front, sm.interpolation_state);
	starting_color = influenceElement<vec4>(matrix_color_back, matrix_color_front, sm.interpolation_state);
	starting_size = influenceElement<float>(point_size_back, point_size_front, sm.interpolation_state);

	vec4 point_to_add = starting_point;

	if (points.size() == 0)
	{
		focal_point = vec3(0.0f);
		average_delta = 0.0f;
		max_x = 0.0f;
		max_y = 0.0f;
		max_z = 0.0f;
	}

	float current_point_count = points.size();

	focal_point = ((current_point_count * focal_point) + vec3(point_to_add)) / (current_point_count + 1.0f);
	float delta = glm::length(vec3(point_to_add) - focal_point);
	average_delta = ((current_point_count * average_delta) + delta) / (current_point_count + 1.0f);

	if (point_to_add.x > max_x)
		max_x = point_to_add.x;

	if (point_to_add.y > max_y)
		max_y = point_to_add.y;

	if (point_to_add.z > max_z)
		max_z = point_to_add.z;

	points.push_back((float)(point_to_add.x));
	points.push_back((float)(point_to_add.y));
	points.push_back((float)(point_to_add.z));
	points.push_back((float)(starting_point.w));

	points.push_back((float)(starting_color.r));
	points.push_back((float)(starting_color.g));
	points.push_back((float)(starting_color.b));
	points.push_back((float)(starting_color.a));
	points.push_back(starting_size);
}

void fractal_generator::addPointSequenceAndIterate(
	mat4 &origin_matrix,
	vec4 &starting_color,
	float &starting_size,
	int matrix_index_front,
	int matrix_index_back,
	std::vector<float> &points,
	std::vector<unsigned short> &line_indices,
	std::vector<unsigned short> &triangle_indices,
	int &current_sequence_index_lines,
	int &current_sequence_index_triangles)
{
	mat4 matrix_front = matrices_front.at(matrix_index_front).second;
	mat4 matrix_back = matrices_back.at(matrix_index_back).second;
	mat4 interpolated_matrix = influenceElement<mat4>(matrix_back, matrix_front, sm.interpolation_state);
	mat4 final_matrix = interpolated_matrix * origin_matrix;

	vec4 matrix_color_front = influenceElement<vec4>(starting_color, colors_front.at(matrix_index_front), sm.bias_coefficient);
	vec4 matrix_color_back = influenceElement<vec4>(starting_color, colors_back.at(matrix_index_back), sm.bias_coefficient);

	float point_size_front = influenceElement<float>(starting_size, sizes_front.at(matrix_index_front), sm.bias_coefficient);
	float point_size_back = influenceElement<float>(starting_size, sizes_back.at(matrix_index_back), sm.bias_coefficient);

	starting_color = influenceElement<vec4>(matrix_color_back, matrix_color_front, sm.interpolation_state);
	starting_size = influenceElement<float>(point_size_back, point_size_front, sm.interpolation_state);

	for (const vec4 &point : sm.point_sequence)
	{
		vec4 point_to_add = final_matrix * point;

		if (points.size() == 0)
		{
			focal_point = vec3(0.0f);
			average_delta = 0.0f;
			max_x = 0.0f;
			max_y = 0.0f;
			max_z = 0.0f;
		}

		float current_point_count = points.size();

		focal_point = ((current_point_count * focal_point) + vec3(point_to_add)) / (current_point_count + 1.0f);
		float delta = glm::length(vec3(point_to_add) - focal_point);
		average_delta = ((current_point_count * average_delta) + delta) / (current_point_count + 1.0f);

		if (point_to_add.x > max_x)
			max_x = point_to_add.x;

		if (point_to_add.y > max_y)
			max_y = point_to_add.y;

		if (point_to_add.z > max_z)
			max_z = point_to_add.z;

		points.push_back((float)(point_to_add.x));
		points.push_back((float)(point_to_add.y));
		points.push_back((float)(point_to_add.z));
		points.push_back((float)(1.0f));

		points.push_back((float)(starting_color.r));
		points.push_back((float)(starting_color.g));
		points.push_back((float)(starting_color.b));
		points.push_back((float)(starting_color.a));
		points.push_back(starting_size);
	}	

	int local_points_size = sm.point_sequence.size();
	int index_sequences_added = points.size() / (sm.point_sequence.size() * vertex_size);

	// determine where values should begin based on the used sequence and number of indices already added
	std::vector<int>::iterator max_local_value_lines = std::max_element(sm.line_indices.begin(), sm.line_indices.end());
	int starting_index_lines = current_sequence_index_lines;
	for (const unsigned short index : sm.line_indices)
	{
		line_indices.push_back(starting_index_lines + index);
	}

	current_sequence_index_lines += *max_local_value_lines;

	std::vector<int>::iterator max_local_value_triangles = std::max_element(sm.triangle_indices.begin(), sm.triangle_indices.end());
	int starting_index_triangles = current_sequence_index_triangles;
	for (const unsigned short index : sm.triangle_indices)
	{
		triangle_indices.push_back(starting_index_triangles + index);
	}

	current_sequence_index_triangles += *max_local_value_triangles;

	origin_matrix = final_matrix;
}

void fractal_generator::addNewPoint(
	const vec4 &point,
	const vec4 &color,
	const float &size,
	std::vector<float> &points)
{
	vec4 point_to_add = point;

	if (points.size() == 0)
	{
		focal_point = vec3(0.0f);
		average_delta = 0.0f;
		max_x = 0.0f;
		max_y = 0.0f;
		max_z = 0.0f;
	}

	float current_point_count = points.size();

	focal_point = ((current_point_count * focal_point) + vec3(point_to_add)) / (current_point_count + 1.0f);
	float delta = glm::length(vec3(point_to_add) - focal_point);
	average_delta = ((current_point_count * average_delta) + delta) / (current_point_count + 1.0f);

	if (point_to_add.x > max_x)
		max_x = point_to_add.x;

	if (point_to_add.y > max_y)
		max_y = point_to_add.y;

	if (point_to_add.z > max_z)
		max_z = point_to_add.z;

	points.push_back((float)point_to_add.x);
	points.push_back((float)point_to_add.y);
	points.push_back((float)point_to_add.z);
	points.push_back((float)point.w);

	points.push_back((float)color.r);
	points.push_back((float)color.g);
	points.push_back((float)color.b);
	points.push_back((float)color.a);
	points.push_back(size);
}

vec4 fractal_generator::getSampleColor(const int &samples, const std::vector<vec4> &color_pool) const
{
	if (samples > color_pool.size())
	{
		std::cout << "color samples requested are greater than the number of colors in the targeted generator" << std::endl;
		return vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	vec4 out_color(0.0f, 0.0f, 0.0f, 0.0f);

	if (samples <= 0)
		return out_color;

	for (int i = 0; i < samples; i++)
	{
		int random_index = (int)(rg.getRandomUniform() * color_pool.size());
		out_color += color_pool.at(random_index);
	}

	out_color /= (float)samples;
	return out_color;
}

void fractal_generator::checkKeys(const shared_ptr<key_handler> &keys)
{
	if (keys->checkPress(GLFW_KEY_O, false))
	{
		//available
	}

	if (keys->checkPress(GLFW_KEY_J, false))
		show_growth = !show_growth;

	if (keys->checkPress(GLFW_KEY_G, false))
	{
		frame_increment = glm::clamp(int(frame_increment) * 2, 1, 100);
	}

	if (keys->checkPress(GLFW_KEY_F, false))
	{
		frame_increment = glm::clamp(int(frame_increment) / 2, 1, 100);
	}

	if (keys->checkPress(GLFW_KEY_H, false))
		reverse_growth = !reverse_growth;

	if (keys->checkPress(GLFW_KEY_HOME, false) && keys->checkShiftHold())
	{
		dof_passes = glm::clamp(dof_passes + 1, 1, 50);
		std::cout << "dof_passes: " << dof_passes << std::endl;
	}

	if (keys->checkPress(GLFW_KEY_END, false) && keys->checkShiftHold())
	{
		dof_passes = glm::clamp(dof_passes - 1, 1, 50);
		std::cout << "dof_passes: " << dof_passes << std::endl;
	}

	if (keys->checkPress(GLFW_KEY_PAGE_UP, true))
	{
		dof_aperture = glm::clamp(dof_aperture + 0.001f, 0.001f, 0.5f);
		std::cout << "dof_aperture: " << dof_passes << std::endl;
	}

	if (keys->checkPress(GLFW_KEY_PAGE_DOWN, true))
	{
		dof_aperture = glm::clamp(dof_aperture - 0.001f, 0.001f, 0.5f);
		std::cout << "dof_aperture: " << dof_aperture << std::endl;
	}

	if (keys->checkPress(GLFW_KEY_L, false) && sm.enable_lines)
	{
		switch (sm.line_mode)
		{
		case 0: sm.line_mode = GL_LINES; break;
		case GL_LINES: sm.line_mode = GL_LINE_STRIP; break;
		case GL_LINE_STRIP: sm.line_mode = 0; break;
		default: break;
		}
	}

	if (keys->checkPress(GLFW_KEY_K, false) && sm.enable_triangles)
	{
		switch (sm.triangle_mode)
		{
		case 0: sm.triangle_mode = GL_TRIANGLES; break;
		case GL_TRIANGLES: sm.triangle_mode = GL_TRIANGLE_STRIP; break;
		case GL_TRIANGLE_STRIP: sm.triangle_mode = GL_TRIANGLE_FAN; break;
		case GL_TRIANGLE_FAN: sm.triangle_mode = 0; break;
		default: break;
		}
	}

	if (keys->checkPress(GLFW_KEY_M, false))
	{
		if (keys->checkShiftHold() && keys->checkCtrlHold())
			cycleLightColorOverride();

		else if (keys->checkShiftHold())
			cycleLineColorOverride();

		else if (keys->checkCtrlHold())
			cycleTriangleColorOverride();

		else if (keys->checkAltHold())
			cyclePointColorOverride();

		else {
			newColors();
			updatePointColorOverride();
			updateLineColorOverride();
			updateTriangleColorOverride();
		}
	}

	if (keys->checkPress(GLFW_KEY_P, false))
		sm.show_points = !sm.show_points;

	if (keys->checkPress(GLFW_KEY_1, false))
		cycleBackgroundColorIndex();

	if (keys->checkPress(GLFW_KEY_2, false))
	{
		sm.no_background = !sm.no_background;
		updateBackground();
	}

	if (keys->checkPress(GLFW_KEY_3, false))
		cycleGeometryType();

	if (keys->checkPress(GLFW_KEY_4, false))
	{
		sm.light_effects_transparency = !sm.light_effects_transparency;
		context->setUniform1i("light_effects_transparency", sm.light_effects_transparency);
	}

	if (keys->checkPress(GLFW_KEY_Q, false)) 
	{
		sm.randomize_lightness = !sm.randomize_lightness;
		if (sm.randomize_lightness)
			std::cout << "randomize lightness enabled" << std::endl;

		else std::cout << "randomize lightness disabled" << std::endl;
	}

	if (keys->checkPress(GLFW_KEY_U, false))
	{
		sm.randomize_alpha = !sm.randomize_alpha;
		if (sm.randomize_alpha)
			std::cout << "randomize alpha enabled" << std::endl;

		else std::cout << "randomize alpha disabled" << std::endl;
	}

	if (keys->checkPress(GLFW_KEY_PERIOD, false))
	{
		if (keys->checkShiftHold())
		{
			point_size_modifier = glm::clamp(point_size_modifier + 0.1f, 0.0f, 2.0f);
			context->setUniform1f("point_size_modifier", point_size_modifier);
		}

		else
		{
			sm.line_width = glm::clamp(sm.line_width + 0.1f, 0.1f, 1.0f);
			GLfloat width_range[2];
			glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, width_range);
			glLineWidth(GLfloat(sm.line_width) * width_range[1]);
		}
	}

	if (keys->checkPress(GLFW_KEY_COMMA, false))
	{
		if (keys->checkShiftHold())
		{
			point_size_modifier = glm::clamp(point_size_modifier - 0.1f, 0.0f, 2.0f);
			context->setUniform1f("point_size_modifier", point_size_modifier);
		}

		else
		{
			sm.line_width = glm::clamp(sm.line_width - 0.1f, 0.1f, 1.0f);
			GLfloat width_range[2];
			glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, width_range);
			glLineWidth(GLfloat(sm.line_width) * width_range[1]);
		}
	}

	if (keys->checkPress(GLFW_KEY_B))
	{
		sm.fractal_scale *= 1.1f;
		fractal_scale_matrix = glm::scale(mat4(1.0f), vec3(sm.fractal_scale, sm.fractal_scale, sm.fractal_scale));
		context->setUniformMatrix4fv("fractal_scale", 1, GL_FALSE, fractal_scale_matrix);
		std::cout << "scale: " << sm.fractal_scale << std::endl;
	}

	if (keys->checkPress(GLFW_KEY_V))
	{
		sm.fractal_scale /= 1.1f;
		fractal_scale_matrix = glm::scale(mat4(1.0f), vec3(sm.fractal_scale, sm.fractal_scale, sm.fractal_scale));
		context->setUniformMatrix4fv("fractal_scale", 1, GL_FALSE, fractal_scale_matrix);
		std::cout << "scale: " << sm.fractal_scale << std::endl;
	}

	if (keys->checkPress(GLFW_KEY_I, false))
		invertColors();

	if (keys->checkPress(GLFW_KEY_N, false))
		regenerateFractal();

	if (keys->checkPress(GLFW_KEY_Z, false))
		sm.smooth_render = !sm.smooth_render;

	if (keys->checkPress(GLFW_KEY_Y, false))
	{
		cycleColorPalette();
		std::cout << "front palette: " + color_man.getPaletteName(sm.palette_front) << std::endl;
		std::cout << "back palette: " + color_man.getPaletteName(sm.palette_back) << std::endl;
	}

	if (keys->checkPress(GLFW_KEY_SEMICOLON, false))
		printContext();

	if (keys->checkPress(GLFW_KEY_APOSTROPHE, false))
		sm.reverse = !sm.reverse;

	if (keys->checkPress(GLFW_KEY_BACKSLASH, false))
		sm.print_context_on_swap = !sm.print_context_on_swap;

	if (keys->checkPress(GLFW_KEY_E, false))
	{
		sm.show_palette = !sm.show_palette;
		context->setUniform1i("render_palette", sm.show_palette ? 1 : 0);
	}

	if (keys->checkPress(GLFW_KEY_RIGHT_BRACKET, true) || keys->checkPress(GLFW_KEY_LEFT_BRACKET, true)) 
	{
		if (keys->checkShiftHold())
		{
			if (keys->checkPress(GLFW_KEY_RIGHT_BRACKET, true))
				sm.illumination_distance = glm::clamp(sm.illumination_distance + 0.01f, 0.01f, 10.0f);

			else sm.illumination_distance = glm::clamp(sm.illumination_distance - 0.01f, 0.01f, 10.0f);

			context->setUniform1f("illumination_distance", sm.lm == CAMERA ? sm.illumination_distance * 10.0f : sm.illumination_distance);
		}

		else
		{

			float increment_min = .01f;
			float increment_max = .5f;
			float step_amount = .002f;

			if (keys->checkPress(GLFW_KEY_RIGHT_BRACKET, true))
				sm.interpolation_increment += step_amount;

			else sm.interpolation_increment -= step_amount;

			sm.interpolation_increment = glm::clamp(sm.interpolation_increment, increment_min, increment_max);
			std::cout << "transition speed: " << sm.interpolation_increment / increment_max << std::endl;
		}
	}

	//TODO make this cycle through enumerated lighting modes, implement lighting modes
	if (keys->checkPress(GLFW_KEY_8, false))
	{
		cycleEnum<lighting_mode>(lighting_mode(0), LIGHTING_MODE_SIZE, sm.lm);
		if (sm.lm == LIGHTING_MODE_SIZE)
			sm.lm = lighting_mode(0);

		std::cout << "lighting mode: " << getStringFromLightingMode(sm.lm) << std::endl;

		context->setUniform1i("lighting_mode", int(sm.lm));
		context->setUniform1i("illumination_distance", sm.lm == CAMERA ? sm.illumination_distance * 10.0f : sm.illumination_distance);
	}

	if (sm.refresh_value != -1 && keys->checkPress(GLFW_KEY_6, false))
	{
		sm.refresh_value == sm.refresh_min ? sm.refresh_value = -1 : sm.refresh_value--;
		std::cout << "refresh value: " << sm.refresh_value << std::endl;
	}

	if (sm.refresh_value != sm.refresh_max && keys->checkPress(GLFW_KEY_7, false))
	{
		sm.refresh_value == -1 ? sm.refresh_value = sm.refresh_min : sm.refresh_value++;
		std::cout << "refresh value: " << sm.refresh_value << std::endl;
	}

	if (keys->checkPress(GLFW_KEY_MINUS, false))
	{
		sm.scale_matrices = !sm.scale_matrices;
		sm.scale_matrices ? std::cout << "scale matrices enabled" << std::endl : std::cout << "scale matrices disabled" << std::endl;
	}

	if (keys->checkPress(GLFW_KEY_EQUAL, false))
		sm.refresh_enabled = !sm.refresh_enabled;

	if (keys->checkPress(GLFW_KEY_F12, false))
		dof_enabled = !dof_enabled;
}

void fractal_generator::tickAnimation() {
	float increment_coefficient = 1.0f - (abs(0.5f - sm.interpolation_state) * 2.0f);

	float actual_increment = glm::clamp(sm.interpolation_increment * increment_coefficient * increment_coefficient, sm.interpolation_increment * 0.05f, sm.interpolation_increment);

	sm.reverse ? sm.interpolation_state -= actual_increment : sm.interpolation_state += actual_increment;

	if (sm.interpolation_state <= 0.0f)
	{
		sm.interpolation_state = 1.0f;
		swapMatrices();
	}

	else if (sm.interpolation_state >= 1.0f)
	{
		sm.interpolation_state = 0.0f;
		swapMatrices();
	}

	regenerateFractal();
	updateBackground();

	if (show_growth)
	{
		if (reverse_growth)
			current_frame <= frame_increment ? current_frame = 0 : current_frame -= frame_increment;

		else current_frame = glm::clamp(int(current_frame + frame_increment), 0, INT_MAX);

		vertices_to_render = current_frame;
	}
}

void fractal_generator::updateBackground()
{
	vec4 actual_background;
	if (sm.no_background)
	{
		actual_background = sm.inverted ? vec4(1.0f, 1.0f, 1.0f, 1.0f) : vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	else
	{
		vec4 new_background = influenceElement<vec4>(colors_back.at(sm.background_back_index), colors_front.at(sm.background_front_index), sm.interpolation_state);
		color_man.adjustLightness(new_background, 0.1f);

		if (sm.inverted)
			new_background = vec4(1.0f) - new_background;

		actual_background = new_background;
	}

	context->setBackgroundColor(actual_background);
	context->setUniform4fv("background_color", 1, actual_background);
}

void fractal_generator::invertColors()
{
	sm.inverted = !sm.inverted;
	context->setUniform1i("invert_colors", sm.inverted ? 1 : 0);
	updateBackground();
}

void fractal_generator::newColors()
{
	seed_color_front = rg.getRandomVec4FromColorRanges(
		0.0f, 1.0f,		// red range
		0.0f, 1.0f,		// green range
		0.0f, 1.0f,		// blue range
		sm.alpha_min, sm.alpha_max		// alpha range
		);

	seed_color_back = rg.getRandomVec4FromColorRanges(
		0.0f, 1.0f,		// red range
		0.0f, 1.0f,		// green range
		0.0f, 1.0f,		// blue range
		sm.alpha_min, sm.alpha_max		// alpha range
		);

	colors_front = generateColorVector(seed_color_front, sm.palette_front, matrices_front.size(), sm.random_palette_front);
	colors_back = generateColorVector(seed_color_back, sm.palette_back, matrices_front.size(), sm.random_palette_back);

	updateBackground();
}

void fractal_generator::regenerateFractal()
{
	if (sm.refresh_enabled)
	{
		if (sm.use_point_sequence)
			generateFractalFromPointSequenceWithRefresh();

		else generateFractalWithRefresh();
	}

	else
	{
		if (sm.use_point_sequence)
			generateFractalFromPointSequence();

		else generateFractal();
	}

	context->setUniform1i("invert_colors", sm.inverted ? 1 : 0);
	context->setUniform1i("lighting_mode", sm.lm);
	context->setUniform3fv("centerpoint", 1, focal_point);
	context->setUniform1f("illumination_distance", sm.lm == CAMERA ? sm.illumination_distance * 10.0f : sm.illumination_distance);
	context->setUniform1f("point_size_modifier", point_size_modifier);

	updateLineColorOverride();
}

void fractal_generator::updateLightColorOverride()
{
	context->setUniform1i("override_light_color_enabled", light_color_override_index != -3);

	vec4 light_color;
	switch (light_color_override_index)
	{
	case -3: break;
	case -2: light_color = vec4(0.0f, 0.0f, 0.0f, 1.0f); break;
	case -1: light_color = vec4(1.0f, 1.0f, 1.0f, 1.0f); break;
	default: light_color = influenceElement<vec4>(colors_back.at(light_color_override_index), colors_front.at(light_color_override_index), sm.interpolation_state); break;
	}

	if (light_color_override_index != -3)
		context->setUniform4fv("light_override_color", 1, light_color);
}

void fractal_generator::updateLineColorOverride()
{
	context->setUniform1i("override_line_color_enabled", line_color_override_index != -3);

	vec4 line_color;
	switch (line_color_override_index)
	{
	case -3: break;
	case -2: line_color = vec4(0.0f, 0.0f, 0.0f, 1.0f); break;
	case -1: line_color = vec4(1.0f, 1.0f, 1.0f, 1.0f); break;
	default: line_color = influenceElement<vec4>(colors_back.at(line_color_override_index), colors_front.at(line_color_override_index), sm.interpolation_state); break;
	}

	if (line_color_override_index != -3)
		context->setUniform4fv("line_override_color", 1, line_color);
}

void fractal_generator::updateTriangleColorOverride()
{
	context->setUniform1i("override_triangle_color_enabled", triangle_color_override_index != -3);

	vec4 triangle_color;
	switch (triangle_color_override_index)
	{
	case -3: break;
	case -2: triangle_color = vec4(0.0f, 0.0f, 0.0f, 1.0f); break;
	case -1: triangle_color = vec4(1.0f, 1.0f, 1.0f, 1.0f); break;
	default: triangle_color = influenceElement<vec4>(colors_back.at(triangle_color_override_index), colors_front.at(triangle_color_override_index), sm.interpolation_state); break;
	}

	if (triangle_color_override_index != -3)
		context->setUniform4fv("triangle_override_color", 1, triangle_color);
}

void fractal_generator::updatePointColorOverride()
{
	context->setUniform1i("override_point_color_enabled", point_color_override_index != -3);

	vec4 point_color;
	switch (point_color_override_index)
	{
	case -3: break;
	case -2: point_color = vec4(0.0f, 0.0f, 0.0f, 1.0f); break;
	case -1: point_color = vec4(1.0f, 1.0f, 1.0f, 1.0f); break;
	default: point_color = influenceElement<vec4>(colors_back.at(point_color_override_index), colors_front.at(point_color_override_index), sm.interpolation_state); break;
	}

	if (point_color_override_index != -3)
		context->setUniform4fv("point_override_color", 1, point_color);
}

void fractal_generator::applyBackground(const int &num_samples)
{
	background_color = sm.inverted ? vec4(1.0f) - getSampleColor(num_samples, colors_front) : getSampleColor(num_samples, colors_front);

	color_man.adjustLightness(background_color, jep::floatRoll(0.0f, 1.0f, 2));
	context->setBackgroundColor(background_color);
}

void fractal_generator::adjustBackgroundBrightness(float adjustment)
{
	float current_lightness = color_man.getHSLFromRGBA(background_color).L;
	color_man.adjustLightness(background_color, current_lightness + adjustment);
	context->setBackgroundColor(background_color);
}

void fractal_generator::loadPointSequence(string name, const std::vector<vec4> &sequence)
{
	if (sequence.size() == 0)
		return;

	loaded_sequences.push_back(std::pair<string, std::vector<vec4> >(name, sequence));
}

void fractal_generator::printContext()
{
	std::cout << "------------------------------------------------" << std::endl;
	std::cout << "base seed: " << base_seed << std::endl;
	std::cout << "generation seed: " << generation_seed << std::endl;
	std::cout << "current generation: " << sm.generation;
	sm.reverse ? std::cout << " <-" << std::endl : std::cout << " ->" << std::endl;

	printMatrices();

	std::cout << "point count: " << vertex_count << std::endl;
	sm.refresh_enabled ? std::cout << "refresh enabled (" << sm.refresh_value << ")" << std::endl : std::cout << "refresh disabled" << std::endl;
	std::cout << "focal point: " + glm::to_string(focal_point) << std::endl;
	std::cout << "max x: " << max_x << std::endl;
	std::cout << "max y: " << max_y << std::endl;
	std::cout << "max z: " << max_z << std::endl;

	std::cout << "front palette: " + color_man.getPaletteName(sm.palette_front) << std::endl;
	if (sm.palette_front == RANDOM_PALETTE)
		std::cout << "current front palette: " + color_man.getPaletteName(sm.random_palette_front) << std::endl;

	std::cout << "front color set, seed = " + color_man.toRGBAString(seed_color_front) + ":" << std::endl;
	color_man.printColorSet(colors_front);
	std::cout << "front background index: " << sm.background_front_index << std::endl;
	std::cout << std::endl;

	std::cout << "back palette: " + color_man.getPaletteName(sm.palette_back) << std::endl;
	if (sm.palette_back == RANDOM_PALETTE)
		std::cout << "current back palette: " + color_man.getPaletteName(sm.random_palette_back) << std::endl;

	std::cout << "back color set, seed = " + color_man.toRGBAString(seed_color_back) + ":" << std::endl;
	color_man.printColorSet(colors_back);
	std::cout << "back background index: " << sm.background_back_index << std::endl;
	std::cout << std::endl;

	std::cout << "line width: " << sm.line_width << std::endl;
	std::cout << "interpolation state: " << sm.interpolation_state << std::endl;
	std::cout << "current scale: " << sm.fractal_scale << std::endl;
	std::cout << "bias coefficient: " << sm.bias_coefficient << std::endl;
	sm.smooth_render ? std::cout << "smooth rendering enabled" << std::endl : std::cout << "smooth rendering disabled" << std::endl;
	sm.randomize_lightness ? std::cout << "lightness randomization enabled" << std::endl : std::cout << "lightness randomization disabled" << std::endl;
	sm.randomize_alpha ? std::cout << "alpha randomization enabled (" << sm.alpha_min << ", " << sm.alpha_max << ")" << std::endl : std::cout << "alpha randomization disabled" << std::endl;
	sm.refresh_enabled ? std::cout << "refresh mode enabled (" << sm.refresh_value << ")" << std::endl : std::cout << "refresh mode disabled" << std::endl;
	sm.two_dimensional ? std::cout << "2D mode enabled" << std::endl : std::cout << "2D mode disabled" << std::endl;
	sm.scale_matrices ? std::cout << "scale matrices enabled" << std::endl : std::cout << "scale matrices disabled" << std::endl;
	if (sm.inverted)
		std::cout << "inverted colors" << std::endl;
	//std::cout << "geometry draw type: " << getStringFromGeometryType(sm.geo_type) << std::endl;
	std::cout << "lighting mode: " << getStringFromLightingMode(sm.lm) << std::endl;
	std::cout << "front geometry matrix type: " << getStringFromGeometryType(geo_type_front) << std::endl;
	std::cout << "back geometry matrix type: " << getStringFromGeometryType(geo_type_back) << std::endl;
	std::cout << "matrix geometry coefficient: " << sm.matrix_geometry_coefficient << std::endl;
	std::cout << "matrix geometry map: " << std::endl;
	for (const auto &geo_pair : sm.matrix_geometry_weights)
	{
		std::cout << geo_pair.first << ": " << geo_pair.second << std::endl;
	}

	std::cout << "-----------------------------------------------" << std::endl;
	std::cout << sm.toString() << std::endl;
	std::cout << "-----------------------------------------------" << std::endl;
}

std::vector<float> fractal_generator::getPalettePoints()
{
	float swatch_height = 2.0f / colors_front.size();

	std::vector<float> point_data;

	for (int i = 0; i < colors_front.size(); i++)
	{
		vec4 current_color_front = colors_front.at(i);
		vec4 current_color_back = colors_back.at(i);
		vec4 current_color_interpolated = (current_color_front * sm.interpolation_state) + (current_color_back * (1.0f - sm.interpolation_state));

		float top_height = 1.0f - (float(i) * swatch_height);
		float bottom_height = 1.0f - ((i * swatch_height) + swatch_height);
		float swatch_width = 0.05f;

		std::vector<vec2> front_points;
		float front_left = 1.0f - (swatch_width * 3.0f);
		float front_right = front_left + swatch_width;
		front_points.push_back(vec2(front_left, top_height)); //front top left
		front_points.push_back(vec2(front_right, top_height)); //front top right
		front_points.push_back(vec2(front_right, bottom_height)); // front bottom right

		front_points.push_back(vec2(front_right, bottom_height)); // front bottom right
		front_points.push_back(vec2(front_left, bottom_height)); // front bottom left
		front_points.push_back(vec2(front_left, top_height)); //front top left

		for (const vec2 point : front_points)
		{
			addDataToPalettePoints(point, current_color_front, point_data);
		}

		std::vector<vec2> interpolated_points;
		float interpolated_left = front_right;
		float interpolated_right = interpolated_left + swatch_width;
		interpolated_points.push_back(vec2(interpolated_left, top_height)); //interpolated top left
		interpolated_points.push_back(vec2(interpolated_right, top_height)); //interpolated top right
		interpolated_points.push_back(vec2(interpolated_right, bottom_height)); // interpolated bottom right

		interpolated_points.push_back(vec2(interpolated_right, bottom_height)); // interpolated bottom right
		interpolated_points.push_back(vec2(interpolated_left, bottom_height)); // interpolated bottom left
		interpolated_points.push_back(vec2(interpolated_left, top_height)); //interpolated top left

		for (const vec2 point : interpolated_points)
		{
			addDataToPalettePoints(point, current_color_interpolated, point_data);
		}

		std::vector<vec2> back_points;
		float back_left = interpolated_right;
		float back_right = back_left + swatch_width;
		back_points.push_back(vec2(back_left, top_height)); //back top left
		back_points.push_back(vec2(back_right, top_height)); //back top right
		back_points.push_back(vec2(back_right, bottom_height)); // back bottom right

		back_points.push_back(vec2(back_right, bottom_height)); // back bottom right
		back_points.push_back(vec2(back_left, bottom_height)); // back bottom left
		back_points.push_back(vec2(back_left, top_height)); //back top left

		for (const vec2 point : back_points)
		{
			addDataToPalettePoints(point, current_color_back, point_data);
		}
	}

	// 6 floats per palette vertex -> 4 for color, 2 for positoin
	palette_vertex_count = point_data.size() / 6;
	return point_data;
}

void fractal_generator::addDataToPalettePoints(const vec2 &point, const vec4 &color, std::vector<float> &points) const
{
	points.push_back(color.r);
	points.push_back(color.g);
	points.push_back(color.b);
	points.push_back(color.a);
	points.push_back(point.x);
	points.push_back(point.y);
}

void fractal_generator::addPalettePointsAndBufferData(const std::vector<float> &vertex_data,  const std::vector<unsigned short> &line_indices_to_buffer, const std::vector<unsigned short> &triangle_indices_to_buffer)
{
	if (initialized)
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &vertices_vbo);
		glDeleteBuffers(1, &line_indices);
		glDeleteBuffers(1, &triangle_indices);
		glDeleteVertexArrays(1, &palette_vao);
		glDeleteBuffers(1, &palette_vbo);
	}

	bufferLightData(vertex_data);
	bufferPalette(getPalettePoints());
	bufferData(vertex_data, line_indices_to_buffer, triangle_indices_to_buffer);

	initialized = true;
}

void fractal_generator::cycleGeometryType()
{
	if (sm.point_sequence_index == GEOMETRY_ENUM_COUNT)
		sm.point_sequence_index = 0;

	else sm.point_sequence_index++;

	sm.use_point_sequence = GEOMETRY_ENUM_COUNT != sm.point_sequence_index;

	if (sm.use_point_sequence)
	{
		sm.setPointSequenceGeometry(sm.point_sequence_index, rg);
	}

	std::cout << "point sequence index: " << sm.point_sequence_index << std::endl;
}

void fractal_generator::cycleBackgroundColorIndex()
{
	sm.background_front_index + 1 == colors_front.size() ? sm.background_front_index = 0 : sm.background_front_index++;
	sm.background_back_index = sm.background_front_index;
	updateBackground();
}

void fractal_generator::cycleLightColorOverride()
{
	if (light_color_override_index == colors_front.size() - 1)
		light_color_override_index = -3;

	else light_color_override_index++;

	std::cout << "light color override index: " << light_color_override_index << std::endl;

	updateLightColorOverride();
}

void fractal_generator::cycleLineColorOverride()
{
	if (line_color_override_index == colors_front.size() - 1)
		line_color_override_index = -3;

	else line_color_override_index++;

	std::cout << "line color override index: " << line_color_override_index << std::endl;

	updateLineColorOverride();
}

void fractal_generator::cycleTriangleColorOverride()
{
	if (triangle_color_override_index == colors_front.size() - 1)
		triangle_color_override_index = -3;

	else triangle_color_override_index++;

	std::cout << "triangle color override index: " << triangle_color_override_index << std::endl;

	updateTriangleColorOverride();
}

void fractal_generator::cyclePointColorOverride()
{
	if (point_color_override_index == colors_front.size() - 1)
		point_color_override_index = -3;

	else point_color_override_index++;

	std::cout << "point color override index: " << point_color_override_index << std::endl;

	updatePointColorOverride();
}

void fractal_generator::setBackgroundColorIndex(int index)
{
	if (index < colors_front.size())
	{
		sm.background_front_index = index;
		sm.background_back_index = index;
		updateBackground();
	}
}

string fractal_generator::getStringFromGeometryType(geometry_type gt) const
{
	switch (gt)
	{
	case CUBOID: return "cuboid";
	case CUBE: return "cube";
	case TETRAHEDRON: return "tetrahedron";
	case OCTAHEDRON: return "octahedron";
	case DODECAHEDRON: return "dodecahedron";
	case ICOSAHEDRON: return "icosahedron";
	//case LOADED_SEQUENCE: return loaded_sequences.at(current_sequence).first;
	case GEOMETRY_TYPE_SIZE: return "points";
	default: return "unknown type";
	}
}

void fractal_generator::generateLights()
{
	light_indices.clear();
	int light_index_spacing = sm.num_points / sm.num_lights;

	for (int i = 0; i < sm.num_lights; i++)
	{
		light_indices.push_back(i * light_index_spacing);
	}
}