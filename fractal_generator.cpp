#include "fractal_generator.h"

fractal_generator::fractal_generator(
	const shared_ptr<ogl_context> &con,
	int num_points, 
	bool two_dimensional)
{
	vertex_count = num_points;
	base_seed = mc.generateAlphanumericString(32);
	generation_seed = base_seed + "_" + std::to_string(generation);
	mc.seed(generation_seed);
	color_man.seed(generation_seed);

	int num_matrices = int(mc.getRandomFloatInRange(2, 7));
	translate_weight = int(mc.getRandomFloatInRange(2, 6));
	rotate_weight = int(mc.getRandomFloatInRange(2, 6));
	scale_weight = int(mc.getRandomFloatInRange(1, 3));

	context = con;
	is_2D = two_dimensional;
	setMatrices(num_matrices);
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
	generation_seed = base_seed + "_" + std::to_string(generation);
	mc.seed(generation_seed);
	color_man.seed(generation_seed);

	int num_matrices = int(mc.getRandomFloatInRange(2, 7));
	translate_weight = int(mc.getRandomFloatInRange(2, 6));
	rotate_weight = int(mc.getRandomFloatInRange(2, 6));
	scale_weight = int(mc.getRandomFloatInRange(1, 3));

	context = con;
	is_2D = two_dimensional;
	setMatrices(num_matrices);
	initialized = false;

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_CLAMP);
	glDepthRange(0.0, 1.0);
}

//fractal_generator::fractal_generator(
//	const string &randomization_seed, 
//	const shared_ptr<ogl_context> &con, 
//	const int &num_matrices, 
//	const int &translate, 
//	const int &rotate, 
//	const int &scale, 
//	int num_points, 
//	bool two_dimensional)
//{
//	vertex_count = num_points;
//	base_seed = randomization_seed;
//	generation_seed = base_seed + "_" + std::to_string(generation);
//	mc.seed(generation_seed);
//	color_man.seed(generation_seed);
//
//	translate_weight = translate;
//	rotate_weight = rotate;
//	scale_weight = scale;
//
//	context = con;
//	is_2D = two_dimensional;
//	setMatrices(num_matrices);
//	initialized = false;
//
//	glEnable(GL_PROGRAM_POINT_SIZE);
//	glEnable(GL_DEPTH_CLAMP);
//	glDepthRange(0.0, 1.0);
//}

void fractal_generator::bufferData(const vector<float> &vertex_data)
{
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

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindVertexArray(0);
}

vector< pair<string, mat4> > fractal_generator::generateMatrixVector(const int &count) const
{
	vector< pair<string, mat4> > matrix_vector;

	int total_proportions = translate_weight + rotate_weight + scale_weight;

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
		color_man.modifyLightness(color_set, mc.getRandomFloatInRange(0.3, 2.0f));

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

void fractal_generator::setMatrices(const int &num_matrices)
{
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

	matrices_front = generateMatrixVector(num_matrices);
	matrices_back = generateMatrixVector(num_matrices);

	alpha_min = 0.5f;
	alpha_max = 1.0f;

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

	colors_front = generateColorVector(seed_color_front, palette_front, num_matrices, random_palette_front);
	colors_back = generateColorVector(seed_color_back, palette_back, num_matrices, random_palette_back);

	sizes_front = generateSizeVector(num_matrices);
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

mat4 fractal_generator::generateInterpolatedMatrix(int index) const
{
	mat4 random_matrix_front = matrices_front.at(index).second;
	mat4 random_matrix_back = matrices_back.at(index).second;

	return (random_matrix_front * interpolation_state) + (random_matrix_back * (1.0f - interpolation_state));
}

vec4 fractal_generator::generateInterpolatedColor(int front_index, int back_index) const
{
	vec4 matrix_color_front = colors_front.at(front_index);
	vec4 matrix_color_back = colors_back.at(back_index);

	return (matrix_color_front * interpolation_state) + (matrix_color_back * (1.0f - interpolation_state));
}

float fractal_generator::generateInterpolatedSize(int index) const
{
	float matrix_size_front = sizes_front.at(index);
	float matrix_size_back = sizes_back.at(index);

	return (matrix_size_front * interpolation_state) + (matrix_size_back * (1.0f - interpolation_state));
}

void fractal_generator::generateFractalFromPointSequence()
{
	if (smooth_render)
		mc.seed(generation_seed);

	vector<float> points;

	if (preloaded_sequence.size() == 0)
		return;

	points.reserve((vertex_count / preloaded_sequence.size()) * vertex_size);

	int num_matrices = matrices_front.size();

	vec4 point_color(0.5f, 0.5f, 0.5f, 0.5f);
	float starting_size = 10.0;

	for (int i = 0; i < (vertex_count / preloaded_sequence.size()) && num_matrices > 0 && preloaded_sequence.size() > 0; i++)
	{
		int random_index = int(mc.getRandomFloatInRange(0.0f, float(matrices_front.size())));

		vec4 transformation_color = generateInterpolatedColor(random_index, random_index);
		float transformation_size = generateInterpolatedSize(random_index);

		for (int n = 0; n < preloaded_sequence.size(); n++)
		{
			vec4* new_point = &preloaded_sequence.at(n);
			addNewPointAndIterate(*new_point, point_color, starting_size, random_index, transformation_color, transformation_size, points);
		}
	}

	bufferData(points);
}

void fractal_generator::generateFractal()
{
	if (smooth_render)
		mc.seed(generation_seed);

	refresh_loaded = false;
	sequence_loaded = false;

	vector<float> points;
	points.reserve(vertex_count * vertex_size);

	int num_matrices = matrices_front.size();

	vec4 starting_point = origin;
	vec4 point_color(1.0f, 1.0f, 1.0f, 1.0f);
	float starting_size = 10.0;

	for (int i = 0; i < vertex_count && num_matrices > 0; i++)
	{
		int random_index = int(mc.getRandomFloatInRange(0.0f, float(matrices_front.size())));
		
		vec4 transformation_color = generateInterpolatedColor(random_index, random_index);
		float transformation_size = generateInterpolatedSize(random_index);

		addNewPointAndIterate(starting_point, point_color, starting_size, random_index, transformation_color, transformation_size, points);
	}

	bufferData(points);
}

void fractal_generator::generateFractalWithRefresh()
{
	if (smooth_render)
		mc.seed(generation_seed);

	sequence_loaded = false;

	vector<float> points;
	points.reserve(vertex_count * vertex_size);

	int num_matrices = matrices_front.size();

	for (int i = 0; i < vertex_count && num_matrices > 0; i++)
	{
		vec4 point_color(0.0f, 0.0f, 0.0f, 0.0f);
		vec4 new_point = origin;
		float new_size = 0.0f;

		for (int n = 0; n < refresh_value; n++)
		{
			int random_index = int(mc.getRandomFloatInRange(0.0f, float(matrices_front.size())));

			vec4 transformation_color = generateInterpolatedColor(random_index, random_index);
			float transformation_size = generateInterpolatedSize(random_index);

			mat4 matrix_front = matrices_front.at(random_index).second;
			mat4 matrix_back = matrices_back.at(random_index).second;

			vec4 point_front = matrix_front * new_point;
			vec4 point_back = matrix_back * new_point;

			interpolation_state = glm::clamp(interpolation_state, 0.0f, 1.0f);

			new_point += (point_front * interpolation_state) + (point_back * (1.0f - interpolation_state));

			point_color += transformation_color;
			new_size += transformation_size;
		}

		new_point /= (float)refresh_value;
		point_color /= (float)refresh_value;
		new_size /= (float)refresh_value;

		addNewPoint(new_point, point_color, new_size, points);
	}

	bufferData(points);
}

void fractal_generator::generateFractalFromPointSequenceWithRefresh()
{
	if (smooth_render)
		mc.seed(generation_seed);

	sequence_loaded = true;

	vector<float> points;

	if (preloaded_sequence.size() == 0)
		return;

	points.reserve(vertex_count * vertex_size);

	int num_matrices = matrices_front.size();

	for (int i = 0; i < vertex_count / preloaded_sequence.size() && num_matrices > 0; i++)
	{
		vec4 point_color(0.0f, 0.0f, 0.0f, 0.0f);
		float new_size = 0.0f;

		for (int n = 0; n < refresh_value; n++)
		{
			int random_index = (int)(mc.getRandomUniform() * num_matrices);

			mat4 transformation_matrix = generateInterpolatedMatrix(random_index);
			vec4 transformation_color = generateInterpolatedColor(random_index, random_index);
			float transformation_size = generateInterpolatedSize(random_index);

			for (int c = 0; c < preloaded_sequence.size(); c++)
			{
				vec4 *target_point = &preloaded_sequence.at(c);
				*target_point = transformation_matrix * *target_point;
			}

			point_color += transformation_color;
			new_size += transformation_size;
		}

		point_color /= (float)refresh_value;
		new_size /= (float)refresh_value;

		for (int c = 0; c < preloaded_sequence.size(); c++)
		{
			vec4 *target_point = &preloaded_sequence.at(c);
			*target_point /= (float)refresh_value;
			addNewPoint(*target_point, point_color, new_size, points);
		}
	}

	bufferData(points);
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
	const vec4 &matrix_color,
	const float &point_size,
	vector<float> &points)
{
	mat4 matrix_front = matrices_front.at(matrix_index).second;
	mat4 matrix_back = matrices_back.at(matrix_index).second;

	vec4 point_front = matrix_front * starting_point;
	vec4 point_back = matrix_back * starting_point;

	interpolation_state = glm::clamp(interpolation_state, 0.0f, 1.0f);

	starting_point = (point_front * interpolation_state) + (point_back * (1.0f - interpolation_state));

	starting_color = (starting_color + matrix_color) / 2.0f;
	starting_size = (starting_size + point_size) / 2.0f;

	vec4 point_to_add = starting_point;

	float max_length = 50.0f;
	float min_length = 0.3f;

	//scale points to ensure all points fall within clipping plane
	if (glm::length(point_to_add) > max_length)
	{
		float scale_factor = max_length / glm::length(point_to_add);
		mat4 scale_modifier = glm::scale(mat4(1.0f), vec3(scale_factor, scale_factor, scale_factor));
		point_to_add = scale_modifier * point_to_add;
	}

	if (glm::length(point_to_add) < min_length)
	{
		float scale_factor = min_length / glm::length(point_to_add);
		mat4 scale_modifier = glm::scale(mat4(1.0f), vec3(scale_factor, scale_factor, scale_factor));
		point_to_add = scale_modifier * point_to_add;
	}

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

void fractal_generator::addNewPoint(
	const vec4 &point,
	const vec4 &color,
	const float &size,
	vector<float> &points)
{
	vec4 point_to_add = point;

	float max_length = 50.0f;
	float min_length = 0.3f;

	//scale points to ensure all points fall within clipping plane
	if (glm::length(point_to_add) > max_length)
	{
		float scale_factor = max_length / glm::length(point_to_add);
		mat4 scale_modifier = glm::scale(mat4(1.0f), vec3(scale_factor, scale_factor, scale_factor));
		point_to_add = scale_modifier * point_to_add;
	}

	if (glm::length(point_to_add) < min_length)
	{
		float scale_factor = min_length / glm::length(point_to_add);
		mat4 scale_modifier = glm::scale(mat4(1.0f), vec3(scale_factor, scale_factor, scale_factor));
		point_to_add = scale_modifier * point_to_add;
	}

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
		background_back_index == colors_front.size() - 1 ? background_back_index = 0 : background_back_index++;

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
}

void fractal_generator::tickAnimation() {
	float increment_coefficient = 1.0f - (abs(0.5f - interpolation_state) * 2.0f);

	float actual_increment = glm::clamp(interpolation_increment * increment_coefficient * increment_coefficient, interpolation_increment * 0.05f, interpolation_increment);

	reverse ? interpolation_state -= actual_increment : interpolation_state += actual_increment;

	if (interpolation_state < 0.0f)
	{
		interpolation_state = 1.0f;
		swapMatrices();
	}

	else if (interpolation_state > 1.0f)
	{
		interpolation_state = 0.0f;
		swapMatrices();
	}

	regenerateFractal();

	vec4 new_background = generateInterpolatedColor(background_front_index, background_back_index);
	background_color = inverted ? vec4(1.0f) - new_background : new_background;
	background_color.a = new_background.a;

	color_man.adjustLightness(background_color, inverted ?  1.0f - (color_man.calcLightness(background_color) * 0.2f) : color_man.calcLightness(background_color) * 0.2f);
	context->setBackgroundColor(background_color);
}

void fractal_generator::invertColor(vec4 &original)
{
	original = 1.0f - original;
}

void fractal_generator::invertColors()
{
	inverted = !inverted;
	glUniform1i(context->getShaderGLint("invert_colors"), inverted ? 1 : 0);
}

void fractal_generator::newColors()
{
	for (auto &color : colors_front)
	{
		color = mc.getRandomVec4AlphaClamp(0.5f, 1.0f);
	}

	for (auto &color : colors_back)
	{
		color = mc.getRandomVec4AlphaClamp(0.5f, 1.0f);
	}

	cout << "new colors" << endl;
}

void fractal_generator::regenerateFractal()
{
	if (refresh_loaded)
	{
		if (sequence_loaded)
			generateFractalFromPointSequenceWithRefresh();

		else generateFractalWithRefresh();
	}

	else
	{
		if (sequence_loaded)
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
	sequence_loaded ? cout << "point sequence loaded" << endl : cout << "point sequence not loaded" << endl;
	cout << "------------------------------------------------" << endl;
}