#include "fractal_generator.h"

fractal_generator::fractal_generator(
	const shared_ptr<ogl_context> &con,
	bool two_dimensional)
{
	cout << "generating seed..." << endl;
	seed = mc.generateAlphanumericString(32, true);
	mc.seed(seed);
	cout << endl << "seed: " << seed << endl << endl;

	int num_matrices = int(mc.getRandomFloatInRange(3, 8));
	int translate = int(mc.getRandomFloatInRange(1, 6));
	int rotate = int(mc.getRandomFloatInRange(1, 6));
	int scale = int(mc.getRandomFloatInRange(1, 6));

	context = con;
	is_2D = two_dimensional;
	setMatrices(num_matrices, translate, rotate, scale);
	initialized = false;

	applyBackground(2);

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_CLAMP);
	glDepthRange(0.0, 1.0);
}

fractal_generator::fractal_generator(
	const string &randomization_seed,
	const shared_ptr<ogl_context> &con,
	bool two_dimensional) : mc(randomization_seed)
{
	cout << "seed: " << randomization_seed << endl << endl;
	seed = randomization_seed;

	int num_matrices = int(mc.getRandomFloatInRange(3, 6));
	int translate = int(mc.getRandomFloatInRange(2, 6));
	int rotate = int(mc.getRandomFloatInRange(1, 4));
	int scale = int(mc.getRandomFloatInRange(1, 3));

	context = con;
	is_2D = two_dimensional;
	setMatrices(num_matrices, translate, rotate, scale);
	initialized = false;

	applyBackground(2);

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_CLAMP);
	glDepthRange(0.0, 1.0);
}

fractal_generator::fractal_generator(
	const string &randomization_seed, 
	const shared_ptr<ogl_context> &con, 
	const int &num_matrices, 
	const int &translate, 
	const int &rotate, 
	const int &scale, 
	bool two_dimensional) : mc(randomization_seed)
{
	cout << "seed: " << randomization_seed << endl << endl;
	seed = randomization_seed;

	context = con;
	is_2D = two_dimensional;
	setMatrices(num_matrices, translate, rotate, scale);
	initialized = false;

	applyBackground(2);

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_CLAMP);
	glDepthRange(0.0, 1.0);
}

fractal_generator::fractal_generator(
	const shared_ptr<ogl_context> &con, 
	const int &num_matrices, 
	const int &translate, 
	const int &rotate, 
	const int &scale, 
	bool two_dimensional)
{
	seed = "unseeded";

	context = con;
	is_2D = two_dimensional;
	setMatrices(num_matrices, translate, rotate, scale);
	initialized = false;

	applyBackground(2);

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_CLAMP);
	glDepthRange(0.0, 1.0);
}

void fractal_generator::bufferData(const vector<float> &vertex_data)
{
	vertex_count = vertex_data.size() / vertex_size;
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

void fractal_generator::setMatrices(const int &num_matrices, const int &translate, const int &rotate, const int &scale)
{
	matrices.clear();

	int total_proportions = translate + rotate + scale;

	for (int i = 0; i < num_matrices; i++)
	{
		int random_number = int(mc.getRandomUniform() * (float)total_proportions);

		unsigned int matrix_type;

		if (random_number < translate)
			matrix_type = 0;

		else if (random_number < translate + rotate)
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
		case 1: matrix_to_add = is_2D ? mc.getRandomRotation2D() : mc.getRandomRotation();
			matrix_category = "rotate";
			break;
		case 2: matrix_to_add = is_2D ? mc.getRandomScale2D() : mc.getRandomScale();
			matrix_category = "scale";
			break;
		default: break;
		}		

		matrices.push_back(pair<string, mat4>(matrix_category, matrix_to_add));
		colors.push_back(vec4(mc.getRandomUniform(), mc.getRandomUniform(), mc.getRandomUniform(), mc.getRandomUniform()));
		sizes.push_back(mc.getRandomUniform() * 5.0);
	}

	cout << endl;
	printMatrices();
	cout << endl;
}

void fractal_generator::printMatrices() const
{
	cout << "-----matrices-----" << endl;
	for (const auto &matrix_pair : matrices)
	{
		cout << matrix_pair.first << endl;
		cout << glm::to_string(matrix_pair.second) << endl;
		cout << "----------" << endl;
	}
}

void fractal_generator::generateFractal(const int &num_points)
{
	generateFractal(mc.getRandomVec4(), num_points);
}

void fractal_generator::generateFractal(const int &num_points, const int &transformation_refresh)
{
	generateFractal(mc.getRandomVec4(), num_points, transformation_refresh);
}

void fractal_generator::generateFractal(vec4 or, const int &num_points, const int &transformation_refresh)
{
	refresh_loaded = true;
	refresh_value = transformation_refresh;
	origin = or;
	sequence_loaded = false;

	vector<float> points;
	points.reserve(num_points * vertex_size);

	int num_matrices = matrices.size();

	for (int i = 0; i < num_points && num_matrices > 0; i++)
	{
		vec4 point_color(0.0f, 0.0f, 0.0f, 0.0f);
		vec4 new_point = or;
		float new_size = 0.0f;

		for (int n = 0; n < transformation_refresh; n++)
		{
			int random_index = (int)(mc.getRandomUniform() * num_matrices);
			mat4 random_matrix = matrices.at(random_index).second;
			vec4 matrix_color = colors.at(random_index);
			float matrix_size = sizes.at(random_index);

			new_point = random_matrix * new_point;
			point_color += matrix_color;
			new_size += matrix_size;
		}

		new_point /= (float)transformation_refresh;
		point_color /= (float)transformation_refresh;
		new_size /= (float)transformation_refresh;

		addNewPoint(new_point, point_color, new_size, points);
	}

	bufferData(points);
}

void fractal_generator::generateFractal(vector<vec4> point_sequence, const int &num_points, const int &transformation_refresh)
{
	refresh_loaded = true;
	refresh_value = transformation_refresh;
	sequence_loaded = true;
	preloaded_sequence = point_sequence;

	vector<float> points;

	if (point_sequence.size() == 0)
		return;

	points.reserve(num_points * vertex_size);

	int num_matrices = matrices.size();

	for (int i = 0; i < num_points / point_sequence.size() && num_matrices > 0; i++)
	{
		vec4 point_color(0.0f, 0.0f, 0.0f, 0.0f);
		float new_size = 0.0f;

		for (int n = 0; n < transformation_refresh; n++)
		{
			int random_index = (int)(mc.getRandomUniform() * num_matrices);
			mat4 random_matrix = matrices.at(random_index).second;
			vec4 matrix_color = colors.at(random_index);
			float matrix_size = sizes.at(random_index);

			for (int c = 0; c < point_sequence.size(); c++)
			{
				vec4 *target_point = &point_sequence.at(c);
				*target_point = random_matrix * *target_point;
				
			}
			
			point_color += matrix_color;
				new_size += matrix_size;
		}

		point_color /= (float)transformation_refresh;
		new_size /= (float)transformation_refresh;

		for (int c = 0; c < point_sequence.size(); c++)
		{
			vec4 *target_point = &point_sequence.at(c);
			*target_point /= (float)transformation_refresh;
			addNewPoint(*target_point, point_color, new_size, points);
		}
	}

	bufferData(points);
}

void fractal_generator::generateFractal(vector<vec4> point_sequence, const int &num_points)
{
	refresh_loaded = false;
	sequence_loaded = true;
	preloaded_sequence = point_sequence;

	vector<float> points;

	if (point_sequence.size() == 0)
		return;

	points.reserve((num_points / point_sequence.size()) * vertex_size);

	int num_matrices = matrices.size();

	vec4 point_color(0.5f, 0.5f, 0.5f, 0.5f);
	float starting_size = 10.0;

	for (int i = 0; i < (num_points / point_sequence.size()) + discard_count && num_matrices > 0 && point_sequence.size() > 0; i++)
	{
		int random_index = (int)(mc.getRandomUniform() * num_matrices);
		mat4 random_matrix = matrices.at(random_index).second;
		vec4 matrix_color = colors.at(random_index);
		float matrix_size = sizes.at(random_index);

		if (i < discard_count)
			continue;

		for (int n = 0; n < point_sequence.size(); n++)
		{
			vec4* new_point = &point_sequence.at(n);
			addNewPointAndIterate(*new_point, point_color, starting_size, random_matrix, matrix_color, matrix_size, points);
		}
	}

	bufferData(points);
}

void fractal_generator::generateFractal(vec4 or, const int &num_points)
{
	refresh_loaded = false;
	origin = or;
	sequence_loaded = false;

	vector<float> points;
	points.reserve(num_points * vertex_size);

	int num_matrices = matrices.size();

	vec4 point_color(0.5f, 0.5f, 0.5f, 0.5f);
	float starting_size = 10.0;

	for (int i = 0; i < num_points + discard_count && num_matrices > 0; i++)
	{
		int random_index = (int)(mc.getRandomUniform() * num_matrices);
		mat4 random_matrix = matrices.at(random_index).second;
		vec4 matrix_color = colors.at(random_index);
		float matrix_size = sizes.at(random_index);

		if (i < discard_count)
			continue;

		addNewPointAndIterate(origin, point_color, starting_size, random_matrix, matrix_color, matrix_size, points);
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

		//while distance from 0,0 origin is less than 2 and calcs < 1000
		/*while (x*x + y*y < 2*2 && calc_counter < 1000)
		{
			float x_temp = x*x - y*y + x_pos;
			y = 2 * x * y + y_pos;
			x = x_temp;
			calc_counter++;
		}*/

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
		int random_index = mc.getRandomUniform() * (float)matrices.size();
		matrix_sequence.push_back(matrices.at(random_index).second);
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
	const mat4 &matrix,
	const vec4 &matrix_color,
	const float &point_size,
	vector<float> &points)
{
	starting_point = matrix * starting_point;
	starting_color = (starting_color + matrix_color) / 2.0f;
	starting_size = (starting_size + point_size) / 2.0f;

	points.push_back((float)starting_point.x);
	points.push_back((float)starting_point.y);
	points.push_back((float)starting_point.z);
	points.push_back((float)starting_point.w);
	points.push_back((float)starting_color.r);
	points.push_back((float)starting_color.g);
	points.push_back((float)starting_color.b);
	points.push_back((float)starting_color.a);
	points.push_back(starting_size);
}

void fractal_generator::addNewPoint(
	const vec4 &point,
	const vec4 &color,
	const float &size,
	vector<float> &points)
{
	points.push_back((float)point.x);
	points.push_back((float)point.y);
	points.push_back((float)point.z);
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
		applyBackground(1);

	if (keys->checkPress(GLFW_KEY_2, false))
		applyBackground(2);

	if (keys->checkPress(GLFW_KEY_3, false))
		applyBackground(3);

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
	}

	if (keys->checkPress(GLFW_KEY_V))
	{
		fractal_scale /= 1.1f;
		fractal_scale_matrix = glm::scale(mat4(1.0f), vec3(fractal_scale, fractal_scale, fractal_scale));
		glUniformMatrix4fv(context->getShaderGLint("fractal_scale"), 1, GL_FALSE, &fractal_scale_matrix[0][0]);
	}

	if (keys->checkPress(GLFW_KEY_I, false))
		invertColors();

	if (keys->checkPress(GLFW_KEY_N, false))
		regenerateFractal();
}

void fractal_generator::invertColor(vec4 &original)
{
	original = 1.0f - original;
}

void fractal_generator::invertColors()
{
	for (auto &color : colors)
	{
		invertColor(color);
	}

	inverted = !inverted;

	regenerateFractal();
}

void fractal_generator::newColors()
{
	for (auto &color : colors)
	{
		color = mc.getRandomVec4();
	}

	regenerateFractal();
}

void fractal_generator::regenerateFractal()
{
	if (refresh_loaded)
	{
		if (sequence_loaded)
			generateFractal(preloaded_sequence, vertex_count, refresh_value);

		else generateFractal(origin, vertex_count, refresh_value);
	}

	else
	{
		if (sequence_loaded)
			generateFractal(preloaded_sequence, vertex_count);

		else generateFractal(origin, vertex_count);
	}

	applyBackground(2);
}

void fractal_generator::applyBackground(const int &num_samples)
{
	background_color = getSampleColor(num_samples, colors);

	adjustBrightness(background_color, inverted ? 0.8f : -0.8f);

	context->setBackgroundColor(background_color);
}

void fractal_generator::adjustBrightness(vec4 &color, float degree)
{
	degree = glm::clamp(degree, -1.0f, 1.0f);

	if (degree < 0)
		color = color * (1.0f + degree);

	else
	{
		vec4 inverted_color = 1.0f - color;
		vec4 adjustment_color = degree * inverted_color;
		color += adjustment_color;
	}
}