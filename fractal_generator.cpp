#include "fractal_generator.h"

fractal_generator::fractal_generator(
	const string &randomization_seed,
	const shared_ptr<ogl_context> &con,
	bool two_dimensional) : mc(randomization_seed)
{
	int num_matrices = int(mc.getRandomFloatInRange(3, 6));
	int translate = int(mc.getRandomFloatInRange(2, 6));
	int rotate = int(mc.getRandomFloatInRange(1, 4));
	int scale = int(mc.getRandomFloatInRange(1, 3));

	context = con;
	is_2D = two_dimensional;
	setMatrices(num_matrices, translate, rotate, scale);
	initialized = false;

	context->setBackgroundColor(getSampleColor(2) * 0.2f);

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
	context = con;
	is_2D = two_dimensional;
	setMatrices(num_matrices, translate, rotate, scale);
	initialized = false;

	context->setBackgroundColor(getSampleColor(2) * 0.2f);

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
	context = con;
	is_2D = two_dimensional;
	setMatrices(num_matrices, translate, rotate, scale);
	initialized = false;

	context->setBackgroundColor(getSampleColor(2) * 0.2f);

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_CLAMP);
	glDepthRange(0.0, 1.0);
}

void fractal_generator::bufferData(const vector<float> &vertex_data)
{
	vertex_count = vertex_data.size() / vertex_size;

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

	// draw type, offset, number of vertices
	if (line_mode != 0)
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

		mat4 matrix_to_add(1.0f);

		switch (matrix_type)
		{
		case 0: 
			matrix_to_add = is_2D ? mc.getRandomTranslation2D() : mc.getRandomTranslation(); 
			cout << "adding translation matrix" << endl;
			break;
		case 1: matrix_to_add = is_2D ? mc.getRandomRotation2D() : mc.getRandomRotation();
			cout << "adding rotation matrix" << endl;
			break;
		case 2: matrix_to_add = is_2D ? mc.getRandomScale2D() : mc.getRandomScale();
			cout << "adding scale matrix" << endl;
			break;
		default: break;
		}		

		matrices.push_back(matrix_to_add);
		colors.push_back(vec4(mc.getRandomUniform(), mc.getRandomUniform(), mc.getRandomUniform(), mc.getRandomUniform()));
		sizes.push_back(mc.getRandomUniform() * 5.0);

		cout << glm::to_string(matrix_to_add) << endl;
		cout << "----------" << endl;
	}
}

void fractal_generator::generateFractal(vec4 origin, const int &num_points, const int &transformation_refresh)
{
	vector<float> points;

	int num_matrices = matrices.size();

	for (int i = 0; i < num_points && num_matrices > 0; i++)
	{
		vec4 point_color(0.0f, 0.0f, 0.0f, 0.0f);
		vec4 new_point = origin;
		float new_size = 0.0f;

		for (int n = 0; n < transformation_refresh; n++)
		{
			int random_index = (int)(mc.getRandomUniform() * num_matrices);
			mat4 random_matrix = matrices.at(random_index);
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

void fractal_generator::generateFractal(const int &num_points, const int &transformation_refresh)
{
	generateFractal(mc.getRandomVec4(), transformation_refresh);
}

void fractal_generator::generateFractal(vector<vec4> point_sequence, const int &num_points, const int &transformation_refresh)
{
	vector<float> points;

	if (point_sequence.size() == 0)
		return;

	points.reserve(num_points * 9);

	int num_matrices = matrices.size();

	for (int i = 0; i < num_points / point_sequence.size() && num_matrices > 0; i++)
	{
		vec4 point_color(0.0f, 0.0f, 0.0f, 0.0f);
		float new_size = 0.0f;

		for (int n = 0; n < transformation_refresh; n++)
		{
			int random_index = (int)(mc.getRandomUniform() * num_matrices);
			mat4 random_matrix = matrices.at(random_index);
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
	vector<float> points;

	if (point_sequence.size() == 0)
		return;

	points.reserve(num_points * 9);

	int num_matrices = matrices.size();

	vec4 point_color(0.5f, 0.5f, 0.5f, 0.5f);
	float starting_size = 10.0;

	for (int i = 0; i < num_points / point_sequence.size() && num_matrices > 0 && point_sequence.size() > 0; i++)
	{
		int random_index = (int)(mc.getRandomUniform() * num_matrices);
		mat4 random_matrix = matrices.at(random_index);
		vec4 matrix_color = colors.at(random_index);
		float matrix_size = sizes.at(random_index);

		if (i < 10)
			continue;

		for (int n = 0; n < point_sequence.size(); n++)
		{
			vec4* new_point = &point_sequence.at(n);
			addNewPointAndIterate(*new_point, point_color, starting_size, random_matrix, matrix_color, matrix_size, points);
		}
	}

	bufferData(points);
}

void fractal_generator::generateFractal(vec4 origin, const int &num_points)
{
	vector<float> points;
	points.reserve(num_points * 9);

	int num_matrices = matrices.size();

	vec4 point_color(0.5f, 0.5f, 0.5f, 0.5f);
	float starting_size = 10.0;

	for (int i = 0; i < num_points && num_matrices > 0; i++)
	{
		int random_index = (int)(mc.getRandomUniform() * num_matrices);
		mat4 random_matrix = matrices.at(random_index);
		vec4 matrix_color = colors.at(random_index);
		float matrix_size = sizes.at(random_index);

		if (i < 10)
			continue;

		addNewPointAndIterate(origin, point_color, starting_size, random_matrix, matrix_color, matrix_size, points);
	}

	bufferData(points);
}

void fractal_generator::generateFractal(const int &num_points)
{
	generateFractal(mc.getRandomVec4(), num_points);
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

vec4 fractal_generator::getSampleColor(const int &samples) const
{
	if (samples > colors.size())
	{
		cout << "color samples requested are greater than the number of colors in the targeted generator" << endl;
		return vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	vec4 out_color(0.0f, 0.0f, 0.0f, 0.0f);

	if (samples <= 0)
		return out_color;

	for (int i = 0; i < samples; i++)
	{
		int random_index = (int)(mc.getRandomUniform() * colors.size());
		out_color += colors.at(random_index);
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

	if (keys->checkPress(GLFW_KEY_L, false))
	{
		switch (line_mode)
		{
		case 0: line_mode = GL_LINES; break;
		case GL_LINES: line_mode = GL_LINE_STRIP; break;
		case GL_LINE_STRIP: line_mode = 0; break;
		default: break;
		}
	}

	if (keys->checkPress(GLFW_KEY_P, false))
		show_points = !show_points;

	if (keys->checkPress(GLFW_KEY_1, false))
		context->setBackgroundColor(getSampleColor(1) * 0.2f);

	if (keys->checkPress(GLFW_KEY_2, false))
		context->setBackgroundColor(getSampleColor(2) * 0.2f);

	if (keys->checkPress(GLFW_KEY_3, false))
		context->setBackgroundColor(getSampleColor(3) * 0.2f);

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
}