#include "fractal_generator.h"

fractal_generator::fractal_generator(
	const shared_ptr<ogl_context> &con,
	int num_points, 
	bool two_dimensional)
{
	vertex_count = num_points;
	base_seed = mc.generateAlphanumericString(32);

	context = con;
	is_2D = two_dimensional;
	setMatrices();
	initialized = false;

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_CLAMP);
	glDepthRange(0.0, 1.0);
}

fractal_generator::fractal_generator(
	const string &randomization_seed,
	const shared_ptr<ogl_context> &con,
	int num_points, 
	bool two_dimensional)
{
	vertex_count = num_points;
	base_seed = randomization_seed;

	context = con;
	is_2D = two_dimensional;
	setMatrices();
	initialized = false;

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_CLAMP);
	glDepthRange(0.0, 1.0);
}

void fractal_generator::bufferData(const vector<float> &vertex_data)
{
	vertex_count = (vertex_data.size() / vertex_size) - palette_vertex_count;
	enable_triangles = vertex_count >= 3;
	enable_lines = vertex_count >= 2;

	if (initialized)
	{
		glDeleteVertexArrays(1, &pg_VAO); 
		glDeleteBuffers(1, &pg_VBO);
	}

	// create/bind Vertex Array Object
	glGenVertexArrays(1, &pg_VAO);
	glBindVertexArray(pg_VAO);

	// create/bind Vertex Buffer Object
	glGenBuffers(1, &pg_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, pg_VBO);
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

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	initialized = true;
}

void fractal_generator::drawFractal() const
{
	// bind target VAO
	glBindVertexArray(pg_VAO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	if (enable_triangles && triangle_mode != 0)
		glDrawArrays(triangle_mode, 0, vertex_count);

	if (enable_lines && line_mode != 0)
		glDrawArrays(line_mode, 0, vertex_count);

	if (show_points)
		glDrawArrays(GL_POINTS, 0, vertex_count);

	if (show_palette)
		glDrawArrays(GL_TRIANGLES, vertex_count, palette_vertex_count);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindVertexArray(0);
}

vector< pair<string, mat4> > fractal_generator::generateMatrixVector(const int &count) const
{
	vector< pair<string, mat4> > matrix_vector;

	int total_proportions = translate_weight + rotate_weight + scale_matrices_enabled ? scale_weight : 0;

	for (int i = 0; i < count; i++)
	{
		int random_number = int(mc.getRandomUniform() * (float)total_proportions);

		unsigned int matrix_type;

		if (random_number < translate_weight)
			matrix_type = 0;

		else if (random_number < translate_weight + rotate_weight)
			matrix_type = 1;

		else matrix_type = 2;

		string matrix_category;
		mat4 matrix_to_add(1.0f);

		switch (matrix_type)
		{
		case 0:
			matrix_to_add = is_2D ? mc.getRandomTranslation2D() : mc.getRandomTranslation();
			matrix_category = "translate";
			break;
		case 1:
			matrix_to_add = is_2D ? mc.getRandomRotation2D() : mc.getRandomRotation();
			matrix_category = "rotate";
			break;
		case 2:
			matrix_to_add = is_2D ? mc.getRandomScale2D() : mc.getRandomScale();
			matrix_category = "scale";
			break;
		default: break;
		}

		matrix_vector.push_back(pair<string, mat4>(matrix_category, matrix_to_add));
	}

	return matrix_vector;
}

vector<vec4> fractal_generator::generateColorVector(const vec4 &seed, color_palette palette, const int &count, color_palette &random_selection) const
{
	vector<vec4> color_set;

	color_set = color_man.generatePaletteFromSeed(seed, palette, count, random_selection);

	if (randomize_alpha)
		color_man.randomizeAlpha(color_set, alpha_min, alpha_max);

	if (randomize_lightness)
		color_man.modifyLightness(color_set, mc.getRandomFloatInRange(0.3, 1.2f));

	return color_set;
}

vector<float> fractal_generator::generateSizeVector(const int &count) const
{
	vector<float> size_vector;

	for (int i = 0; i < count; i++)
	{
		size_vector.push_back(mc.getRandomUniform() * 5.0);
	}

	return size_vector;
}

// this method is run once and only once per fractal gen object
void fractal_generator::setMatrices()
{
	generation_seed = base_seed + "_" + std::to_string(generation);
	mc.seed(generation_seed);
	color_man.seed(generation_seed);

	int num_matrices = int(mc.getRandomFloatInRange(2, 10));
	translate_weight = int(mc.getRandomFloatInRange(1, 10));
	rotate_weight = int(mc.getRandomFloatInRange(1, 10));
	scale_weight = int(mc.getRandomFloatInRange(1, 10));

	/*int num_matrices = int(mc.getRandomFloatInRange(3, 7));
	translate_weight = int(mc.getRandomFloatInRange(4, 6));
	rotate_weight = int(mc.getRandomFloatInRange(4, 6));
	scale_weight = int(mc.getRandomFloatInRange(1, 3));*/

	for (int i = 0; i < vertex_count; i++)
	{
		matrix_sequence.push_back(int(mc.getRandomFloatInRange(0.0f, float(num_matrices))));
	}

	int random_palette_index = int(mc.getRandomFloatInRange(0.0f, float(DEFAULT_COLOR_PALETTE)));
	palette_front = color_palette(random_palette_index);
	palette_back = color_palette(random_palette_index);

	//TODO add .reserve() for each vector
	matrices_front.clear();
	colors_front.clear();
	sizes_front.clear();

	matrices_back.clear();
	colors_back.clear();
	sizes_back.clear();

	alpha_min = 0.5f;
	alpha_max = 1.0f;

	//front data set
	matrices_front = generateMatrixVector(num_matrices);
	seed_color_front = mc.getRandomVec4FromColorRanges(
		0.0f, 1.0f,		// red range
		0.0f, 1.0f,		// green range
		0.0f, 1.0f,		// blue range
		alpha_min, alpha_max		// alpha range
		);
	colors_front = generateColorVector(seed_color_front, palette_front, num_matrices, random_palette_front);
	sizes_front = generateSizeVector(num_matrices);
	
	//back data set
	generation++;
	generation_seed = base_seed + "_" + std::to_string(generation);
	mc.seed(generation_seed);
	color_man.seed(generation_seed);
	matrices_back = generateMatrixVector(num_matrices);
	seed_color_back = mc.getRandomVec4FromColorRanges(
		0.0f, 1.0f,		// red range
		0.0f, 1.0f,		// green range
		0.0f, 1.0f,		// blue range
		alpha_min, alpha_max		// alpha range
		);
	colors_back = generateColorVector(seed_color_back, palette_back, num_matrices, random_palette_back);
	sizes_back = generateSizeVector(num_matrices);
}

void fractal_generator::swapMatrices() 
{
	if (reverse)
		generation--;

	else generation++;

	generation_seed = base_seed + "_" + std::to_string(generation);
	mc.seed(generation_seed);

	//TODO use pointers instead of loaded if statements
	if (!reverse)
	{
		matrices_back = matrices_front;
		colors_back = colors_front;
		sizes_back = sizes_front;
		seed_color_back = seed_color_front;

		seed_color_front = mc.getRandomVec4FromColorRanges(
			0.0f, 1.0f,		// red range
			0.0f, 1.0f,		// green range
			0.0f, 1.0f,		// blue range
			alpha_min, alpha_max		// alpha range
			);

		matrices_front = generateMatrixVector(matrices_back.size());
		colors_front = generateColorVector(seed_color_front, palette_front, matrices_back.size(), random_palette_front);
		sizes_front = generateSizeVector(matrices_back.size());
	}

	else
	{
		matrices_front = matrices_back;
		colors_front = colors_back;
		sizes_front = sizes_back;
		seed_color_front = seed_color_back;

		seed_color_back = mc.getRandomVec4FromColorRanges(
			0.0f, 1.0f,		// red range
			0.0f, 1.0f,		// green range
			0.0f, 1.0f,		// blue range
			alpha_min, alpha_max		// alpha range
			);

		matrices_back = generateMatrixVector(matrices_front.size());
		colors_back = generateColorVector(seed_color_back, palette_back, matrices_front.size(), random_palette_back);
		sizes_back = generateSizeVector(matrices_front.size());
	}

	if (print_context_on_swap)
		printContext();
}

void fractal_generator::changeDirection()
{
	reverse = !reverse;
}

void fractal_generator::cycleColorPalette()
{
	// palettes separated in case these change independently at some point
	if (palette_front == DEFAULT_COLOR_PALETTE)
		palette_front = color_palette(0);

	else palette_front = color_palette(int(palette_front) + 1);

	if (palette_back == DEFAULT_COLOR_PALETTE)
		palette_back = color_palette(0);

	else palette_back = color_palette(int(palette_back) + 1);
}

void fractal_generator::printMatrices() const
{
	cout << "-----matrices_front-----" << endl;
	for (const auto &matrix_pair : matrices_front)
	{
		cout << matrix_pair.first << endl;
		cout << glm::to_string(matrix_pair.second) << endl;
		cout << "----------" << endl;
	}

	cout << "-----matrices_back-----" << endl;
	for (const auto &matrix_pair : matrices_back)
	{
		cout << matrix_pair.first << endl;
		cout << glm::to_string(matrix_pair.second) << endl;
		cout << "----------" << endl;
	}

	cout << "------------------" << endl;
}

vec4 fractal_generator::generateInterpolatedColor(int front_index, int back_index) const
{
	vec4 matrix_color_front = colors_front.at(front_index);
	vec4 matrix_color_back = colors_back.at(back_index);

	vec4 interpolated = (matrix_color_front * interpolation_state) + (matrix_color_back * (1.0f - interpolation_state));

	interpolated.r = glm::clamp(interpolated.r, 0.0f, 1.0f);
	interpolated.g = glm::clamp(interpolated.g, 0.0f, 1.0f);
	interpolated.b = glm::clamp(interpolated.b, 0.0f, 1.0f);
	interpolated.a = glm::clamp(interpolated.a, 0.0f, 1.0f);

	return interpolated;
}

float fractal_generator::generateInterpolatedSize(int index) const
{
	float matrix_size_front = sizes_front.at(index);
	float matrix_size_back = sizes_back.at(index);

	return (matrix_size_front * interpolation_state) + (matrix_size_back * (1.0f - interpolation_state));
}

void fractal_generator::generateFractalFromPointSequence()
{
	vector<float> points;
	points.reserve(vertex_count * vertex_size);

	int num_matrices = matrices_front.size();

	vec4 point_color = inverted ? vec4(0.0f, 0.0f, 0.0f, 1.0f) : vec4(1.0f);
	float starting_size = 10.0;

	mat4 origin_matrix = glm::scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));

	for (int i = 0; i < vertex_count / point_sequence.size(); i++)
	{
		int matrix_index = smooth_render ? matrix_sequence.at(i) : int(mc.getRandomFloatInRange(0.0f, float(matrices_front.size())));
		vec4 transformation_color = generateInterpolatedColor(matrix_index, matrix_index);
		float transformation_size = generateInterpolatedSize(matrix_index);

		addPointSequenceAndIterate(origin_matrix, point_color, starting_size, matrix_index, points);
	}

	addPalettePointsAndBufferData(points);
}

void fractal_generator::generateFractal()
{
	vector<float> points;
	points.reserve(vertex_count * vertex_size);

	int num_matrices = matrices_front.size();

	vec4 starting_point = origin;
	vec4 point_color = inverted ? vec4(0.0f, 0.0f, 0.0f, 1.0f) : vec4(1.0f);
	float starting_size = 10.0;

	for (int i = 0; i < vertex_count && num_matrices > 0; i++)
	{
		int matrix_index = smooth_render ? matrix_sequence.at(i) : int(mc.getRandomFloatInRange(0.0f, float(matrices_front.size())));
		
		vec4 transformation_color = generateInterpolatedColor(matrix_index, matrix_index);
		float transformation_size = generateInterpolatedSize(matrix_index);

		addNewPointAndIterate(starting_point, point_color, starting_size, matrix_index, points);
	}

	addPalettePointsAndBufferData(points);
}

void fractal_generator::generateFractalWithRefresh()
{
	vector<float> points;
	points.reserve(vertex_count * vertex_size);

	int num_matrices = matrices_front.size();
	signed int actual_refresh = refresh_value == -1 ? int(mc.getRandomFloatInRange(refresh_min, refresh_max)) : refresh_value;

	for (int i = 0; i < vertex_count && num_matrices > 0; i++)
	{
		vec4 point_color = inverted ? vec4(0.0f, 0.0f, 0.0f, 1.0f) : vec4(1.0f);
		vec4 new_point = origin;
		float new_size = 10.0f;

		for (int n = 0; n < actual_refresh; n++)
		{
			int matrix_index = smooth_render ? matrix_sequence.at((i + n) % matrix_sequence.size()) : int(mc.getRandomFloatInRange(0.0f, float(matrices_front.size())));

			mat4 matrix_front = matrices_front.at(matrix_index).second;
			mat4 matrix_back = matrices_back.at(matrix_index).second;
			vec4 point_front = matrix_front * new_point;
			vec4 point_back = matrix_back * new_point;

			interpolation_state = glm::clamp(interpolation_state, 0.0f, 1.0f);

			vec4 transformation_color = generateInterpolatedColor(matrix_index, matrix_index);
			float transformation_size = generateInterpolatedSize(matrix_index);
			new_point = (point_front * interpolation_state) + (point_back * (1.0f - interpolation_state));

			point_color += transformation_color;
			new_size += transformation_size;
		}

		point_color /= ((float)actual_refresh + 1.0f);
		new_size /= ((float)actual_refresh + 1.0f);

		addNewPoint(new_point, point_color, new_size, points);
	}

	addPalettePointsAndBufferData(points);
}

void fractal_generator::generateFractalFromPointSequenceWithRefresh()
{
	vector<float> points;
	points.reserve(vertex_count * vertex_size);

	int num_matrices = matrices_front.size();

	signed int actual_refresh = refresh_value == -1 ? int(mc.getRandomFloatInRange(refresh_min, refresh_max)) : refresh_value;

	for (int i = 0; i < vertex_count / point_sequence.size() && num_matrices > 0; i++)
	{
		vec4 final_color = inverted ? vec4(0.0f, 0.0f, 0.0f, 1.0f) : vec4(1.0f);
		float final_size = 10.0;
		mat4 final_matrix = glm::scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));

		for (int n = 0; n < actual_refresh; n++)
		{
			int matrix_index = smooth_render ? matrix_sequence.at((i + n) % matrix_sequence.size()) : int(mc.getRandomFloatInRange(0.0f, float(matrices_front.size())));

			mat4 matrix_front = matrices_front.at(matrix_index).second;
			mat4 matrix_back = matrices_back.at(matrix_index).second;
			mat4 interpolated_matrix = (matrix_front * interpolation_state) + (matrix_back * (1.0f - interpolation_state));
			vec4 transformation_color = generateInterpolatedColor(matrix_index, matrix_index);
			float transformation_size = generateInterpolatedSize(matrix_index);

			final_matrix = interpolated_matrix * final_matrix;
			final_color += transformation_color;
			final_size += transformation_size;
		}

		final_color /= float(actual_refresh + 1);
		final_size /= float(actual_refresh + 1);

		for (const vec4 &point : point_sequence)
		{
			addNewPoint(final_matrix * point, final_color, final_size, points);
		}
	}

	addPalettePointsAndBufferData(points);
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
			cout << "Screenshot limit of 64 reached. Remove some shots if you want to take more." << endl;
			return;
		}
	}

	fScreenshot = fopen(cFileName, "wb");

	vector<mat4> matrix_sequence = generateMatrixSequence(10);
	map<int, int> calc_map;
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
		cout << calc_pair.first << " calcs: " << calc_pair.second << endl;
	}

	unsigned char TGAheader[12] = { 0,0,2,0,0,0,0,0,0,0,0,0 };
	unsigned char header[6] = { image_width % 256,image_width / 256, image_height % 256,image_height / 256,24,0 };

	fwrite(TGAheader, sizeof(unsigned char), 12, fScreenshot);
	fwrite(header, sizeof(unsigned char), 6, fScreenshot);
	fwrite(pixels, sizeof(GLubyte), nSize, fScreenshot);
	fclose(fScreenshot);

	delete[] pixels;

	return;
}

vector<mat4> fractal_generator::generateMatrixSequence(const int &sequence_size) const
{
	vector<mat4> matrix_sequence;

	for (int i = 0; i < sequence_size; i++)
	{
		int random_index = mc.getRandomUniform() * (float)matrices_front.size();
		matrix_sequence.push_back(matrices_front.at(random_index).second);
	}

	//return matrix_sequence;

	vector<mat4> dummy_sequence = {
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
	int matrix_index,
	vector<float> &points)
{
	mat4 matrix_front = matrices_front.at(matrix_index).second;
	mat4 matrix_back = matrices_back.at(matrix_index).second;
	vec4 point_front = matrix_front * starting_point;
	vec4 point_back = matrix_back * starting_point;

	vec4 matrix_color_front = (starting_color + colors_front.at(matrix_index)) / 2.0f;
	vec4 matrix_color_back = (starting_color + colors_back.at(matrix_index)) / 2.0f;

	float point_size_front = (starting_size + sizes_front.at(matrix_index)) / 2.0f;
	float point_size_back = (starting_size + sizes_back.at(matrix_index)) / 2.0f;

	interpolation_state = glm::clamp(interpolation_state, 0.0f, 1.0f);

	starting_point = (point_front * interpolation_state) + (point_back * (1.0f - interpolation_state));
	starting_color = (matrix_color_front * interpolation_state) + (matrix_color_back * (1.0f - interpolation_state));
	starting_size = (point_size_front * interpolation_state) + (point_size_back * (1.0f - interpolation_state));

	vec4 point_to_add = starting_point;

	//float max_length = 50.0f;
	//float min_length = 0.3f;

	////scale points to ensure all points fall within clipping plane
	//if (glm::length(point_to_add) > max_length)
	//{
	//	float scale_factor = max_length / glm::length(point_to_add);
	//	mat4 scale_modifier = glm::scale(mat4(1.0f), vec3(scale_factor, scale_factor, scale_factor));
	//	point_to_add = scale_modifier * point_to_add;
	//}

	//if (glm::length(point_to_add) < min_length)
	//{
	//	float scale_factor = min_length / glm::length(point_to_add);
	//	mat4 scale_modifier = glm::scale(mat4(1.0f), vec3(scale_factor, scale_factor, scale_factor));
	//	point_to_add = scale_modifier * point_to_add;
	//}

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
	int matrix_index,
	vector<float> &points)
{
	mat4 matrix_front = matrices_front.at(matrix_index).second;
	mat4 matrix_back = matrices_back.at(matrix_index).second;
	mat4 interpolated_matrix = (matrix_front * interpolation_state) + (matrix_back * (1.0f - interpolation_state));
	mat4 final_matrix = interpolated_matrix * origin_matrix;

	vec4 matrix_color_front = (starting_color + colors_front.at(matrix_index)) / 2.0f;
	vec4 matrix_color_back = (starting_color + colors_back.at(matrix_index)) / 2.0f;

	float point_size_front = (starting_size + sizes_front.at(matrix_index)) / 2.0f;
	float point_size_back = (starting_size + sizes_back.at(matrix_index)) / 2.0f;

	starting_color = (matrix_color_front * interpolation_state) + (matrix_color_back * (1.0f - interpolation_state));
	starting_size = (point_size_front * interpolation_state) + (point_size_back * (1.0f - interpolation_state));

	for (const vec4 &point : point_sequence)
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

	origin_matrix = final_matrix;
}

void fractal_generator::addNewPoint(
	const vec4 &point,
	const vec4 &color,
	const float &size,
	vector<float> &points)
{
	vec4 point_to_add = point;

	//float max_length = 50.0f;
	//float min_length = 0.3f;

	////scale points to ensure all points fall within clipping plane
	//if (glm::length(point_to_add) > max_length)
	//{
	//	float scale_factor = max_length / glm::length(point_to_add);
	//	mat4 scale_modifier = glm::scale(mat4(1.0f), vec3(scale_factor, scale_factor, scale_factor));
	//	point_to_add = scale_modifier * point_to_add;
	//}

	//if (glm::length(point_to_add) < min_length)
	//{
	//	float scale_factor = min_length / glm::length(point_to_add);
	//	mat4 scale_modifier = glm::scale(mat4(1.0f), vec3(scale_factor, scale_factor, scale_factor));
	//	point_to_add = scale_modifier * point_to_add;
	//}

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

vec4 fractal_generator::getSampleColor(const int &samples, const vector<vec4> &color_pool) const
{
	if (samples > color_pool.size())
	{
		cout << "color samples requested are greater than the number of colors in the targeted generator" << endl;
		return vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	vec4 out_color(0.0f, 0.0f, 0.0f, 0.0f);

	if (samples <= 0)
		return out_color;

	for (int i = 0; i < samples; i++)
	{
		int random_index = (int)(mc.getRandomUniform() * color_pool.size());
		out_color += color_pool.at(random_index);
	}

	out_color /= (float)samples;
	return out_color;
}

void fractal_generator::checkKeys(const shared_ptr<key_handler> &keys)
{
	if (keys->checkPress(GLFW_KEY_O, false))
	{
		size_enabled = !size_enabled;

		if (size_enabled)
			glEnable(GL_PROGRAM_POINT_SIZE);

		else glDisable(GL_PROGRAM_POINT_SIZE);
	}

	if (keys->checkPress(GLFW_KEY_L, false) && enable_lines)
	{
		switch (line_mode)
		{
		case 0: line_mode = GL_LINES; break;
		case GL_LINES: line_mode = GL_LINE_STRIP; break;
		case GL_LINE_STRIP: line_mode = 0; break;
		default: break;
		}
	}

	if (keys->checkPress(GLFW_KEY_K, false) && enable_triangles && !is_2D)
	{
		switch (triangle_mode)
		{
		case 0: triangle_mode = GL_TRIANGLES; break;
		case GL_TRIANGLES: triangle_mode = GL_TRIANGLE_STRIP; break;
		case GL_TRIANGLE_STRIP: triangle_mode = GL_TRIANGLE_FAN; break;
		case GL_TRIANGLE_FAN: triangle_mode = 0; break;
		default: break;
		}
	}

	if (keys->checkPress(GLFW_KEY_M, false))
		newColors();

	if (keys->checkPress(GLFW_KEY_P, false))
		show_points = !show_points;

	if (keys->checkPress(GLFW_KEY_1, false))
		background_front_index == colors_front.size() - 1 ? background_front_index = 0 : background_front_index++;

	if (keys->checkPress(GLFW_KEY_2, false))
	{
		solid_geometry = !solid_geometry;
		gm.setExportAsTriangles(solid_geometry);
		solid_geometry ? cout << "solid geometry enabled" << endl : cout << "solid geometry disabled" << endl;
	}

	if (keys->checkPress(GLFW_KEY_3, false))
		cycleGeometryType();

	if (keys->checkPress(GLFW_KEY_4, false))
		adjustBackgroundBrightness(-0.1f);

	if (keys->checkPress(GLFW_KEY_5, false))
		adjustBackgroundBrightness(0.05f);

	if (keys->checkPress(GLFW_KEY_Q, false)) 
	{
		randomize_lightness = !randomize_lightness;
		if (randomize_lightness)
			cout << "randomize lightness enabled" << endl;

		else cout << "randomize lightness disabled" << endl;
	}

	if (keys->checkPress(GLFW_KEY_U, false))
	{
		randomize_alpha = !randomize_alpha;
		if (randomize_alpha)
			cout << "randomize alpha enabled" << endl;

		else cout << "randomize alpha disabled" << endl;
	}

	if (keys->checkPress(GLFW_KEY_PERIOD, false))
	{
		if (line_width == 5.0f)
			line_width = 1.0f;

		else line_width += 1.0f;

		glLineWidth(line_width);
	}

	if (keys->checkPress(GLFW_KEY_COMMA, false))
	{
		if (line_width == 1.0f)
			line_width = 5.0f;

		else line_width -= 1.0f;

		glLineWidth(line_width);
	}

	if (keys->checkPress(GLFW_KEY_B))
	{
		fractal_scale *= 1.1f;
		fractal_scale_matrix = glm::scale(mat4(1.0f), vec3(fractal_scale, fractal_scale, fractal_scale));
		glUniformMatrix4fv(context->getShaderGLint("fractal_scale"), 1, GL_FALSE, &fractal_scale_matrix[0][0]);
		cout << "scale: " << fractal_scale << endl;
	}

	if (keys->checkPress(GLFW_KEY_V))
	{
		fractal_scale /= 1.1f;
		fractal_scale_matrix = glm::scale(mat4(1.0f), vec3(fractal_scale, fractal_scale, fractal_scale));
		glUniformMatrix4fv(context->getShaderGLint("fractal_scale"), 1, GL_FALSE, &fractal_scale_matrix[0][0]);
		cout << "scale: " << fractal_scale << endl;
	}

	if (keys->checkPress(GLFW_KEY_I, false))
		invertColors();

	if (keys->checkPress(GLFW_KEY_N, false))
		regenerateFractal();

	if (keys->checkPress(GLFW_KEY_Z, false))
		toggleSmooth();

	if (keys->checkPress(GLFW_KEY_Y, false))
	{
		cycleColorPalette();
		cout << "front palette: " + color_man.getPaletteName(palette_front) << endl;
		cout << "back palette: " + color_man.getPaletteName(palette_back) << endl;
	}

	if (keys->checkPress(GLFW_KEY_SEMICOLON, false))
		printContext();

	if (keys->checkPress(GLFW_KEY_APOSTROPHE, false))
		changeDirection();

	if (keys->checkPress(GLFW_KEY_BACKSLASH, false))
		print_context_on_swap = !print_context_on_swap;

	if (keys->checkPress(GLFW_KEY_E, false))
		show_palette = !show_palette;

	if (keys->checkPress(GLFW_KEY_RIGHT_BRACKET, true) || keys->checkPress(GLFW_KEY_LEFT_BRACKET, true)) 
	{
		float increment_min = .01f;
		float increment_max = .5f;
		float step_amount = .002f;

		if (keys->checkPress(GLFW_KEY_RIGHT_BRACKET, true))
			interpolation_increment += step_amount;

		else interpolation_increment -= step_amount;

		interpolation_increment = glm::clamp(interpolation_increment, increment_min, increment_max);
		cout << "transition speed: " << interpolation_increment / increment_max << endl;
	}

	if (keys->checkPress(GLFW_KEY_8, false))
	{
		lighting_enabled = !lighting_enabled;
		glUniform1i(context->getShaderGLint("lighting_enabled"), lighting_enabled ? 1 : 0);
		updateBackground();
	}

	if (refresh_value != -1 && keys->checkPress(GLFW_KEY_6, false))
	{
		refresh_value == refresh_min ? refresh_value = -1 : refresh_value--;
		cout << "refresh value: " << refresh_value << endl;
	}

	if (refresh_value != refresh_max && keys->checkPress(GLFW_KEY_7, false))
	{
		refresh_value == -1 ? refresh_value = refresh_min : refresh_value++;
		cout << "refresh value: " << refresh_value << endl;
	}

	if (keys->checkPress(GLFW_KEY_MINUS, false))
	{
		scale_matrices_enabled = !scale_matrices_enabled;
		scale_matrices_enabled ? cout << "scale matrices enabled" << endl : cout << "scale matrices disabled" << endl;
	}

	if (keys->checkPress(GLFW_KEY_EQUAL, false))
		refresh_loaded = !refresh_loaded;
}

void fractal_generator::tickAnimation() {
	float increment_coefficient = 1.0f - (abs(0.5f - interpolation_state) * 2.0f);

	float actual_increment = glm::clamp(interpolation_increment * increment_coefficient * increment_coefficient, interpolation_increment * 0.05f, interpolation_increment);

	reverse ? interpolation_state -= actual_increment : interpolation_state += actual_increment;

	if (interpolation_state <= 0.0f)
	{
		interpolation_state = 1.0f;
		swapMatrices();
	}

	else if (interpolation_state >= 1.0f)
	{
		interpolation_state = 0.0f;
		swapMatrices();
	}

	regenerateFractal();
	updateBackground();
}

void fractal_generator::updateBackground()
{
	if (lighting_enabled)
		inverted ? context->setBackgroundColor(vec4(1.0f, 1.0f, 1.0f, 1.0f)) : context->setBackgroundColor(vec4(0.0f, 0.0f, 0.0f, 1.0f));

	else
	{
		vec4 new_background = generateInterpolatedColor(background_front_index, background_back_index);
		color_man.adjustLightness(new_background, 0.1f);

		if (inverted)
			new_background = vec4(1.0f) - new_background;

		background_color = new_background;
		context->setBackgroundColor(background_color);
	}
}

void fractal_generator::invertColors()
{
	inverted = !inverted;
	glUniform1i(context->getShaderGLint("invert_colors"), inverted ? 1 : 0);
	updateBackground();
}

void fractal_generator::newColors()
{
	seed_color_front = mc.getRandomVec4FromColorRanges(
		0.0f, 1.0f,		// red range
		0.0f, 1.0f,		// green range
		0.0f, 1.0f,		// blue range
		alpha_min, alpha_max		// alpha range
		);

	seed_color_back = mc.getRandomVec4FromColorRanges(
		0.0f, 1.0f,		// red range
		0.0f, 1.0f,		// green range
		0.0f, 1.0f,		// blue range
		alpha_min, alpha_max		// alpha range
		);

	colors_front = generateColorVector(seed_color_front, palette_front, matrices_front.size(), random_palette_front);
	colors_back = generateColorVector(seed_color_back, palette_back, matrices_front.size(), random_palette_back);

	updateBackground();
}

void fractal_generator::regenerateFractal()
{
	if (refresh_loaded)
	{
		if (use_point_sequence)
			generateFractalFromPointSequenceWithRefresh();

		else generateFractalWithRefresh();
	}

	else
	{
		if (use_point_sequence)
			generateFractalFromPointSequence();

		else generateFractal();
	}
}

void fractal_generator::applyBackground(const int &num_samples)
{
	background_color = inverted ? vec4(1.0f) - getSampleColor(num_samples, colors_front) : getSampleColor(num_samples, colors_front);

	color_man.adjustLightness(background_color, jep::floatRoll(0.0f, 1.0f, 2));
	context->setBackgroundColor(background_color);
}

void fractal_generator::adjustBackgroundBrightness(float adjustment)
{
	float current_lightness = color_man.getHSLFromRGBA(background_color).L;
	color_man.adjustLightness(background_color, current_lightness + adjustment);
	context->setBackgroundColor(background_color);
}

void fractal_generator::loadPointSequence(const vector<vec4> &sequence)
{
	if (sequence.size() == 0)
		return;

	custom_sequence = sequence;
	point_sequence = custom_sequence;
	gt = LOADED_SEQUENCE;
	use_point_sequence = true;
}

void fractal_generator::printContext()
{
	cout << "------------------------------------------------" << endl;
	cout << "base seed: " << base_seed << endl;
	cout << "generation seed: " << generation_seed << endl;
	cout << "current generation: " << generation;
	reverse ? cout << " <-" << endl : cout << " ->" << endl;

	printMatrices();

	cout << "point count: " << vertex_count << endl;
	refresh_loaded ? cout << "refresh enabled (" << refresh_value << ")" << endl : cout << "refresh disabled" << endl;
	cout << "focal point: " + glm::to_string(focal_point) << endl;
	cout << "max x: " << max_x << endl;
	cout << "max y: " << max_y << endl;
	cout << "max z: " << max_z << endl;

	cout << "front palette: " + color_man.getPaletteName(palette_front) << endl;
	if (palette_front == RANDOM_PALETTE)
		cout << "current front palette: " + color_man.getPaletteName(random_palette_front) << endl;

	cout << "front color set, seed = " + color_man.toRGBAString(seed_color_front) + ":" << endl;
	color_man.printColorSet(colors_front);
	cout << "front background index: " << background_front_index << endl;
	cout << endl;

	cout << "back palette: " + color_man.getPaletteName(palette_back) << endl;
	if (palette_back == RANDOM_PALETTE)
		cout << "current back palette: " + color_man.getPaletteName(random_palette_back) << endl;

	cout << "back color set, seed = " + color_man.toRGBAString(seed_color_back) + ":" << endl;
	color_man.printColorSet(colors_back);
	cout << "back background index: " << background_back_index << endl;
	cout << endl;

	cout << "line width: " << line_width << endl;
	cout << "interpolation state: " << interpolation_state << endl;
	cout << "current scale: " << fractal_scale << endl;
	smooth_render ? cout << "smooth rendering enabled" << endl : cout << "smooth rendering disabled" << endl;
	randomize_lightness ? cout << "lightness randomization enabled" << endl : cout << "lightness randomization disabled" << endl;
	randomize_alpha ? cout << "alpha randomization enabled (" << alpha_min << ", " << alpha_max << ")" << endl : cout << "alpha randomization disabled" << endl;
	refresh_loaded ? cout << "refresh mode enabled (" << refresh_value << ")" << endl : cout << "refresh mode disabled" << endl;
	is_2D ? cout << "2D mode enabled" << endl : cout << "2D mode disabled" << endl;
	scale_matrices_enabled ? cout << "scale matrices enabled" << endl : cout << "scale matrices disabled" << endl;
	cout << "geometry type: " << getStringFromGeometryType(gt) << endl;
	cout << "------------------------------------------------" << endl;
}

vector<float> fractal_generator::getPalettePoints()
{
	float swatch_height = 2.0f / colors_front.size();

	vector<float> point_data;

	for (int i = 0; i < colors_front.size(); i++)
	{
		vec4 current_color_front = colors_front.at(i);
		vec4 current_color_back = colors_back.at(i);
		vec4 current_color_interpolated = (current_color_front * interpolation_state) + (current_color_back * (1.0f - interpolation_state));

		float top_height = 1.0f - (float(i) * swatch_height);
		float bottom_height = 1.0f - ((i * swatch_height) + swatch_height);
		float swatch_width = 0.05f;

		vector<vec2> front_points;
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

		vector<vec2> interpolated_points;
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

		vector<vec2> back_points;
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

	palette_vertex_count = point_data.size() / vertex_size;
	return point_data;
}

void fractal_generator::addDataToPalettePoints(const vec2 &point, const vec4 &color, vector<float> &points) const
{
	points.push_back(point.x);
	points.push_back(point.y);
	points.push_back(0.0f);		//to make vec4-compliant
	points.push_back(1.0f);		//to make vec4-compliant
	points.push_back(color.r);
	points.push_back(color.g);
	points.push_back(color.b);
	points.push_back(color.a);
	points.push_back(0.0f);		//point size
}

void fractal_generator::addPalettePointsAndBufferData(const vector<float> &vertex_data)
{
	glUniform1i(context->getShaderGLint("palette_vertex_id"), vertex_count);
	vector<float> all_data = vertex_data;
	vector<float> palette_points = getPalettePoints();
	all_data.insert(all_data.end(), palette_points.begin(), palette_points.end());
	bufferData(all_data);
}

void fractal_generator::cycleGeometryType()
{
	if (gt == DEFAULT_GEOMETRY_TYPE)
		gt = (geometry_type)0;

	else gt = geometry_type(int(gt) + 1);

	if (gt == LOADED_SEQUENCE && custom_sequence.size() == 0)
		gt = DEFAULT_GEOMETRY_TYPE;

	use_point_sequence = DEFAULT_GEOMETRY_TYPE == gt ? false : true;

	if (use_point_sequence)
	{
		float random_width = mc.getRandomFloatInRange(0.2f, 1.0f);
		float random_height = mc.getRandomFloatInRange(0.2f, 1.0f);
		float random_depth = mc.getRandomFloatInRange(0.2f, 1.0f);

		switch (gt)
		{
		case TRIANGLE: point_sequence = gm.getTriangle(random_width); break;
		case RECTANGLE: point_sequence = gm.getRectangle(random_width, random_height); break;
		case SQUARE: point_sequence = gm.getSquare(random_width); break;
		case CUBOID: point_sequence = gm.getCuboid(random_width, random_height, random_depth); break;
		case CUBE: point_sequence = gm.getCube(random_width); break;
		case TETRAHEDRON: point_sequence = gm.getTetrahedron(random_width); break;
		case OCTAHEDRON: point_sequence = gm.getOctahedron(random_width); break;
		case DODECAHEDRON: point_sequence = gm.getDodecahedron(random_width); break;
		case LOADED_SEQUENCE: point_sequence = custom_sequence;
		case DEFAULT_GEOMETRY_TYPE: break;
		default: break;
		}
	}

	cout << "geometry type: " << getStringFromGeometryType(gt) << endl;
}