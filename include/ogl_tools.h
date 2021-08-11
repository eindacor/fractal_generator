#ifndef OPENGL_TOOLS_H
#define OPENGL_TOOLS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <boost/smart_ptr/shared_ptr.hpp>
//#include "jep_util.h"

enum DATA_TYPE {
	UNDEFINED_DATA_TYPE, OBJ_MTLLIB, OBJ_F, OBJ_V, OBJ_VT, OBJ_VN, OBJ_VP, OBJ_G, OBJ_USEMTL,
	MTL_NEWMTL, MTL_KA, MTL_KD, MTL_KS, MTL_NS, MTL_D,
	MTL_MAP_KA, MTL_MAP_KD, MTL_MAP_KS, MTL_MAP_D, MTL_MAP_NS, MTL_MAP_BUMP, MTL_MAP_DISP, MTL_DECAL
};

namespace jep
{
	class ogl_context;
	class key_handler;
	class ogl_camera;
	class text_handler;
	class texture_handler;
	class text_character;
	class material_data;
	class mesh_data;
	class obj_contents;
	enum text_justification { LL, UL, UR, LR };
	enum render_type { NORMAL, TEXT, ABSOLUTE, UNDEFINED_RENDER_TYPE };

	const float getLineAngle(glm::vec2 first, glm::vec2 second, bool right_handed);
	const glm::vec4 rotatePointAroundOrigin(const glm::vec4 &point, const glm::vec4 &origin, const float degrees, const glm::vec3 &axis);
	void loadTexture(const char* imagepath, GLuint &textureID);
	void loadBMP(const char* imagepath, GLuint &textureID);
	void loadTGA(const char* imagepath, GLuint &textureID);

	const std::vector<float> extractFloats(const std::string &s);
	const std::vector<std::vector<int> > extractFaceSequence(const std::string &s);
	const std::vector<mesh_data> generateMeshes(const char* file_path);
	const std::map<std::string, std::shared_ptr<material_data> > generateMaterials(const char* file_path, std::shared_ptr<texture_handler> &textures, const std::shared_ptr<ogl_context> &context);
	const DATA_TYPE getDataType(const std::string &line);
	const std::string extractName(const std::string &line);
	const bool floatsAreEqual(float first, float second);

	//ogl_context initializes glew, creates a glfw window, generates programs using shaders provided, 
	//and stores program and texture GLuints to be used by other objects
	class ogl_context
	{
	public:
		ogl_context(std::string title, std::string vert_file, std::string frag_file,
			int window_width, int window_height, bool raw_string_shaders = false);
		~ogl_context();

		void printErrors();

		GLFWwindow* getWindow() { return window; }
		bool getErrors() { return errors; }

		void clearBuffers() const {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); glUseProgram(program_ID);
		}
		void swapBuffers() const { glfwSwapBuffers(window); }
		void enableDiffuseMap()
		{
		    const GLchar* test = "enable_diffuse_map";
		    //GLchar test = testStr.at(0);
		    glUniform1i(getShaderGLint(test), GLint(1));
		}
		void disableDiffuseMap() { glUniform1i(getShaderGLint("enable_diffuse_map"), GLint(0)); }
		void enableBumpMap(float f) { glUniform1i(getShaderGLint("enable_bump_map"), GLint(1)); glUniform1f(getShaderGLint("bump_value"), GLfloat(f)); }
		void disableBumpMap() { glUniform1i(getShaderGLint("enable_bump_map"), GLint(0)); }
		void enableNormalMap() { glUniform1i(getShaderGLint("enable_normal_map"), GLint(1)); }
		void disableNormalMap() { glUniform1i(getShaderGLint("enable_normal_map"), GLint(0)); }
		void enableTransparencyMap() { glUniform1i(getShaderGLint("enable_transparency_map"), GLint(1)); }
		void disableTransparencyMap() { glUniform1i(getShaderGLint("enable_transparency_map"), GLint(0)); }
		void enableSpecularMap() { glUniform1i(getShaderGLint("enable_specular_map"), GLint(1)); }
		void disableSpecularMap() { glUniform1i(getShaderGLint("enable_specular_map"), GLint(0)); }

		void setUniform1i(const char* name, int value)
		{
		    glUniform1i(getShaderGLint(name), GLint(value));
		}

		void setUniform3fv(const char* name, int count, glm::vec3 value)
		{
		    glUniform3fv(getShaderGLint(name), GLint(count), &value[0]);
		}

		void setUniform4fv(const char* name, int count, glm::vec4 value)
		{
		    glUniform4fv(getShaderGLint(name), GLint(count), &value[0]);
		}

		void setUniformMatrix4fv(const char* name, int count, bool transpose, glm::mat4 matrix)
		{
		    glUniformMatrix4fv(getShaderGLint(name), GLint(count), transpose, &matrix[0][0]);
		}

		void setUniform1f(const char* name, float value)
		{
		    glUniform1f(getShaderGLint(name), GLfloat(value));
		}

		const GLuint getProgramID() const { return program_ID; }
		const float getAspectRatio() const { return aspect_ratio; }
		const glm::vec4 getBackgroundColor() const { return background_color; }

		int getWindowHeight() const { return window_height; }
		int getWindowWidth() const { return window_width; }

		GLint getShaderGLint(const GLchar* name);

		void setBackgroundColor(glm::vec4 color) { glClearColor(color.x, color.y, color.z, color.w); background_color = color; }

	private:
		GLuint createShader(std::string file, GLenum type, bool raw_string);
		GLuint createProgram(std::string vert_file, std::string frag_file, bool raw_string_shaders);

		GLint element_color_ID;
		glm::vec4 background_color;
		GLFWwindow* window;
		bool errors = true;
		int window_height, window_width;
		std::string window_title;
		std::vector<std::string> display_errors;

		std::map<GLchar*, std::shared_ptr<GLint> > glint_map;

		GLuint program_ID;

		float aspect_ratio;
	};

	//ogl_camera is a projection matrix manipulator, this is a base class, from which specific camera types should be derived
	//TODO make this class an abstract class
	class ogl_camera
	{
	public:
		ogl_camera(const std::shared_ptr<key_handler> &kh, const std::shared_ptr<ogl_context> &context, const glm::vec3 &position, const glm::vec3 &focus, float fov);
		~ogl_camera(){};

		void setViewMatrix(const glm::mat4 &vm) { view_matrix = vm; }
		const glm::mat4 getViewMatrix() const { return view_matrix; }
		const glm::mat4 getProjectionMatrix() const { return projection_matrix; }
		std::shared_ptr<key_handler> getKeys() { return keys; }
		void setMVP(const std::shared_ptr<ogl_context> &context, const glm::mat4 &model_matrix, const render_type &rt);

		const glm::vec3 getFocus() const { return camera_focus; }
		const glm::vec3 getPosition() const { return camera_position; }

		void setFocus(glm::vec3 focus) { camera_focus = focus; }
		void setPosition(glm::vec3 position) { camera_position = position; }

		void adjustFocalLength(float degree);

		void setFOV(float fov) { camera_fov = fov; projection_matrix = glm::perspective(glm::clamp(camera_fov, 1.0f, 180.0f) * 0.017453f, aspect_scale, .01f, 500.0f); }

		const glm::vec3 getCameraDirectionVector() const { return glm::normalize(camera_focus - camera_position); }

		virtual void updateCamera();

	private:
		glm::mat4 view_matrix;
		glm::mat4 projection_matrix;
		glm::mat4 previous_model_matrix;
		glm::mat4 previous_view_matrix;
		glm::mat4 previous_projection_matrix;
		glm::mat4 aspect_scale_matrix;
		std::shared_ptr<key_handler> keys;
		float aspect_scale;

		render_type current_render_type;

		glm::vec3 camera_focus;
		glm::vec3 camera_position;

		float camera_fov;
	};

	/*
	class ogl_camera_rigged : public ogl_camera
	{
	public:
		ogl_camera_rigged(glm::vec3 p_offset, glm::vec3 f_offset) :
			ogl_camera(NULL, p_offset, f_offset),
			position_offset(p_offset),
			focal_offset(f_offset) {
			update();
		}
		~ogl_camera_rigged(){};

		void setTarget(std::shared_ptr<model> t) { target = t; updateCamera(); }

		void update();

	private:
		glm::vec3 position_offset;
		glm::vec3 focal_offset;
		std::shared_ptr<model> target;
	};
	*/

	//ogl_camera_free is a specific derived class of ogl_camera that allows keyboard inputs to modify the position
	//TODO enable customization of stepping distances
	class ogl_camera_free : public ogl_camera
	{
	public:
		ogl_camera_free(const std::shared_ptr<key_handler> &kh, const std::shared_ptr<ogl_context> &context, const glm::vec3 &position, float fov) :
			ogl_camera(kh, context, position, glm::vec3(position.x, position.y, position.z - 100.0f), fov)
		{
			strafe_distance = .1f;
			step_distance = .1f;
			tilt_angle = 2.0f;
			rotate_angle = 2.0f;

			camera_tilt = 0.0f;
			camera_rotation = 0.0f;

			move_forward = false;
			move_backward = false;
			rotate_left = false;
			rotate_right = false;
			tilt_up = false;
			tilt_down = false;
			strafe_left = false;
			strafe_right = false;
		}

		~ogl_camera_free(){};

		void printErrors();

		virtual void updateCamera();
		void setPrintMovement(bool b) { print_movement = b; }
		void setStepDistance(float f) { step_distance = f; }
		void setRotateAngle(float f) { rotate_angle = f; }
		void setTiltAngle(float f) { tilt_angle = f; }
		void setStrafeDistance(float f) { strafe_distance = f; }

		void stepCamera(float dist);
		void strafeCamera(float dist);
		void rotateCamera(float degrees);
		void tiltCamera(float degrees);

		float camera_tilt;
		float camera_rotation;

		void move(signed short n);
		void rotate(signed short n);
		void tilt(signed short n);
		void strafe(signed short n);

	private:
		float strafe_distance;
		float step_distance;
		float tilt_angle;
		float rotate_angle;

		bool move_forward;
		bool move_backward;
		bool rotate_left;
		bool rotate_right;
		bool tilt_up;
		bool tilt_down;
		bool strafe_left;
		bool strafe_right;
		bool print_movement = false;
	};

	//ogl_camera_free is a specific derived class of ogl_camera that allows keyboard inputs to modify the position
	//TODO enable customization of stepping distances
	class ogl_camera_flying : public ogl_camera
	{
	public:
		ogl_camera_flying(const std::shared_ptr<key_handler> &kh, const std::shared_ptr<ogl_context> &context, const glm::vec3 &position, float fov) :
			ogl_camera(kh, context, position, glm::vec3(position.x, position.y, position.z - 1.0f), fov)
		{
			strafe_distance = .1f;
			step_distance = .1f;
			tilt_angle = 2.0f;
			rotate_angle = 2.0f;

			camera_tilt = 0.0f;
			camera_rotation = 0.0f;

			move_forward = false;
			move_backward = false;
			rotate_left = false;
			rotate_right = false;
			tilt_up = false;
			tilt_down = false;
			strafe_left = false;
			strafe_right = false;
		}

		~ogl_camera_flying() {};

		void printErrors();

		virtual void updateCamera();
		void setPrintMovement(bool b) { print_movement = b; }

		void stepCamera(float dist);
		void twistCamera(float dist);
		void rotateCamera(float degrees);
		void tiltCamera(float degrees);

		void setStepDistance(float f) { step_distance = f; }
		void setRotateAngle(float f) { rotate_angle = f; }
		void setTiltAngle(float f) { tilt_angle = f; }
		void setStrafeDistance(float f) { strafe_distance = f; }

		float camera_tilt;
		float camera_rotation;

		void move(signed short n);
		void rotate(signed short n);
		void tilt(signed short n);
		void strafe(signed short n);

	private:
		float strafe_distance;
		float step_distance;
		float tilt_angle;
		float rotate_angle;

		bool move_forward;
		bool move_backward;
		bool rotate_left;
		bool rotate_right;
		bool tilt_up;
		bool tilt_down;
		bool strafe_left;
		bool strafe_right;
		bool print_movement = false;
	};

	class ogl_camera_iso : public ogl_camera
	{
	public:
		ogl_camera_iso(const std::shared_ptr<key_handler> &kh, const std::shared_ptr<ogl_context> &context, glm::vec3 position, float fov) :
			ogl_camera(kh, context, position, glm::vec3(position.x, position.y, position.z - 10.0f), fov)
		{
			strafe_distance = .1f;
			step_distance = .1f;
			tilt_angle = 2.0f;
			rotate_angle = 2.0f;

			camera_tilt = 0.0f;
			camera_rotation = 0.0f;

			move_forward = false;
			move_backward = false;
			rotate_left = false;
			rotate_right = false;
			tilt_up = false;
			tilt_down = false;
			strafe_left = false;
			strafe_right = false;
		}

		~ogl_camera_iso(){};

		void printErrors();

		virtual void updateCamera();
		void setPrintMovement(bool b) { print_movement = b; }

		void stepCamera(float dist);
		void strafeCamera(float dist);
		void rotateCamera(float degrees);
		void tiltCamera(float degrees);

		float camera_tilt;
		float camera_rotation;

		void move(signed short n);
		void rotate(signed short n);
		void tilt(signed short n);
		void strafe(signed short n);

	private:
		float strafe_distance;
		float step_distance;
		float tilt_angle;
		float rotate_angle;

		bool move_forward;
		bool move_backward;
		bool rotate_left;
		bool rotate_right;
		bool tilt_up;
		bool tilt_down;
		bool strafe_left;
		bool strafe_right;
		bool print_movement;
	};

	//key_handler interprets keystrokes using the ogl_context provided
	class key_handler
	{
	public:
		key_handler(std::shared_ptr<ogl_context> ch) { context = ch; }
		~key_handler(){};

		bool checkPress(int key, bool hold = true);
		bool checkMouse(int key, bool hold = true);
		bool checkShiftHold() { return checkPress(GLFW_KEY_LEFT_SHIFT, true) || checkPress(GLFW_KEY_RIGHT_SHIFT, true); }
		bool checkCtrlHold() { return checkPress(GLFW_KEY_LEFT_CONTROL, true) || checkPress(GLFW_KEY_RIGHT_CONTROL, true); }
		bool checkAltHold() { return checkPress(GLFW_KEY_LEFT_ALT, true) || checkPress(GLFW_KEY_RIGHT_ALT, true); }
		glm::vec2 getCursorPosition() { 
			updateCursorPosition(); return glm::vec2(x_window_position, y_window_position); }

	private:
		void updateCursorPosition();
		double x_window_position;
		double y_window_position;
		std::map<int, bool> keys;
		std::map<int, bool> mouse;
		std::shared_ptr<ogl_context> context;
	};

	//class that handles VBO/VAO data for meshes that share a texture map
	class ogl_data
	{
	public:
		ogl_data(const std::shared_ptr<ogl_context> &context,
			const std::shared_ptr<material_data> &material,
			GLenum draw_type,
			const std::vector<unsigned short> &indices,
			const std::vector<float> &vertex_data,
			int v_data_size,
			int vt_data_size,
			int vn_data_size);
		~ogl_data();

		const int getVertexCount() const { return vertex_count; }
		const int getIndexCount() const { return index_count; }

		std::shared_ptr<GLuint> getVBO() const { return VBO; }
		std::shared_ptr<GLuint> getVAO() const { return VAO; }
		std::shared_ptr<GLuint> getIND() const { return IND; }

		void overrideVBO(std::shared_ptr<GLuint> new_VBO) { VBO = new_VBO; }
		void overrideVAO(std::shared_ptr<GLuint> new_VAO) { VAO = new_VAO; }

		void overrideIND(std::shared_ptr<GLuint> new_IND) { IND = new_IND; }

		std::shared_ptr<material_data> getMaterial() { return mesh_material; }

	private:
		void initializeGLuints() {
			VAO = std::shared_ptr<GLuint>(new GLuint);
			VBO = std::shared_ptr<GLuint>(new GLuint);
			IND = std::shared_ptr<GLuint>(new GLuint);
		}

		std::shared_ptr<GLuint> VBO;
		std::shared_ptr<GLuint> VAO;
		std::shared_ptr<GLuint> IND;
		
		bool element_array_enabled;
		unsigned short index_count;
		int vertex_count;

		std::shared_ptr<material_data> mesh_material;
	};

	//class that stores/renders multiple ogl_data objects
	class ogl_model
	{
	public:
		ogl_model(const std::shared_ptr<ogl_context> &existing_context) { context = existing_context; }
		~ogl_model() {};

		virtual void draw(std::shared_ptr<ogl_camera> &camera);
		std::shared_ptr<ogl_data> getOGLData() { return opengl_data; }
		glm::mat4 getModelMatrix() const { return model_matrix; }
		void addData(const std::shared_ptr<ogl_data> &toAdd) { model_data.push_back(toAdd); }

	private:
		std::shared_ptr<ogl_data> opengl_data;
		glm::mat4 model_matrix = glm::mat4(1.0);
		std::shared_ptr<ogl_context> context;

		std::vector<std::shared_ptr<ogl_data> > model_data;
	};

	/*
	class ogl_model_static : public ogl_model
	{
	public: 
		ogl_model_static(std::shared_ptr<ogl_data> ogld) : ogl_model(ogld) {};
		~ogl_model_static(){};

		virtual void draw(std::shared_ptr<ogl_context> context, std::shared_ptr<ogl_camera> camera);

	private:

	};

	class ogl_model_mobile : public ogl_model
	{
	public:
		ogl_model_mobile(std::shared_ptr<ogl_data> ogld) : ogl_model(ogld) {};
		~ogl_model_mobile(){};

		virtual void draw(std::shared_ptr<ogl_context> context, std::shared_ptr<ogl_camera> camera);

	private:
		glm::mat4 model_matrix;
	};

	class ogl_model_animated : public ogl_model
	{
	public:
		ogl_model_animated(std::shared_ptr<ogl_data> ogld) : ogl_model(ogld) {};
		~ogl_model_animated(){};

		virtual void draw(const std::shared_ptr<ogl_context> &context, const std::shared_ptr<ogl_camera> &camera);

	private:
		//first int is animation index, second is frame index, third is byte offset in opengl array
		std::map<int, std::map<int, int> > animation_indices;
		int current_animation;
		int current_frame;
	};
	*/

	class ogl_scene
	{
	public:
		ogl_scene(std::shared_ptr<ogl_camera> cam, std::shared_ptr<ogl_context> con);
		~ogl_scene(){};

	private:
		std::shared_ptr<ogl_camera> camera;
		std::shared_ptr<ogl_context> context;
	};

	//TODO make static_text class format similar to dynamic_hud_array with justification options
	//TODO enable word-wrapping without cutting off words
	class static_text
	{
	public:
		static_text(const std::vector< std::pair<std::shared_ptr<ogl_data>, glm::mat4> > &char_array,
			const glm::vec4 &color, const glm::vec4 &trans_color, GLchar* text_ID, GLchar* text_color_ID,
			GLchar* transparent_color_ID, bool transparent, const glm::vec2 &upper_left_position,
			const glm::vec2 &lower_right_position, float scale, float box_x = -1.0f, float box_y = -1.0f) : character_array(char_array)
		{
			text_shader_ID = text_ID;
			text_color_shader_ID = text_color_ID;
			text_color = color;
			text_scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale));
			text_translation_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(upper_left_position.x, upper_left_position.y, 0.0f));
			upper_left = upper_left_position;
			lower_right = lower_right_position;
			x_bound = (box_x > 0.0f);
			y_bound = (box_y > 0.0f);
			box_width = box_x;
			box_height = box_y;
		};

		static_text(std::string s, text_justification tj, const std::shared_ptr<text_handler> &text,
			const glm::vec4 &color, GLchar* text_enable_ID, GLchar* text_color_ID,
			const glm::vec2 &on_screen_position, float scale, float box_x = -1.0f, float box_y = -1.0f);

		~static_text(){};

		void draw(const std::shared_ptr<ogl_camera> &camera, const std::shared_ptr<ogl_context> &context);
		void draw(const std::shared_ptr<ogl_camera> &camera, const std::shared_ptr<ogl_context> &context, const glm::mat4 &position_matrix_override);

		glm::vec2 getUpperLeft() const { return upper_left; }
		glm::vec2 getLowerRight() const;
		glm::vec2 getLowerLeft() const;
		glm::vec2 getUpperRight() const;

	private:
		void setPageData();
		void setVisible();
		std::string raw_text;
		glm::vec4 text_color;
		glm::mat4 text_scale_matrix;
		glm::mat4 text_translation_matrix;

		std::map <int, std::vector<std::shared_ptr<text_character> > >visible_lines;
		std::map <int, std::vector< std::shared_ptr<text_character> >::iterator > page_map;
		float array_padding;
		text_justification justification;
		//TODO move lines/rectangles to ogl_tools
		//vector< std::shared_ptr<line> > lines;

		GLchar* text_shader_ID;
		GLchar* text_color_shader_ID;
		GLchar* transparent_color_shader_ID;

		glm::vec2 upper_left, lower_right;

		bool x_bound, y_bound;
		float box_width, box_height;
	
		std::vector< std::pair<std::shared_ptr<ogl_data>, glm::mat4> > character_array;

		int line_count;
		int character_count;
	};

	class text_character
	{
	public:
		text_character(char character, const std::shared_ptr<text_handler> &text, const glm::vec2 &anchor_point, text_justification tj,
			const glm::vec2 &screen_dimensions, bool italics = false);
		~text_character(){};

		void draw(const std::shared_ptr<ogl_context> &context, const std::shared_ptr<ogl_camera> &camera);

		void adjustPosition(glm::vec2 new_position) { position = new_position; setPositionMatrix(); }
		void adjustDimensions(glm::vec2 new_dimensions) { dimensions = new_dimensions; setPositionMatrix(); }

		glm::vec2 getLowerLeft() const { return lower_left; }
		glm::vec2 getLowerRight() const { return lower_right; }
		glm::vec2 getUpperLeft() const { return upper_left; }
		glm::vec2 getUpperRight() const { return upper_right; }

		void switchFont(const std::shared_ptr<GLuint> &new_TEX) { TEX = new_TEX; }

	private:
		void setPositionMatrix();

		glm::vec2 position;
		glm::vec2 dimensions;
		text_justification justification;
		glm::mat4 position_matrix;

		glm::vec2 lower_left, upper_left, upper_right, lower_right;

		std::shared_ptr<GLuint> VBO, VAO, IND, TEX;

		int grid_index;
		char c;
	};

	class text_handler
	{
	public:
		text_handler(const std::shared_ptr<ogl_context> &context, const std::shared_ptr<GLuint> &TEX,
			GLchar* transparent_color_shader_ID, glm::vec4 transparency_color);
		~text_handler();

		std::shared_ptr<ogl_data> getOGLData() const { return opengl_data; }
		void addFont(const std::string &font_name, const char* text_image_path);
		void switchFont(const std::string &font_name);

	private:
	    std::shared_ptr<GLuint> default_TEX;
	    std::shared_ptr<ogl_data> opengl_data;

		std::map<std::string, std::shared_ptr<GLuint> > font_map;
	};

	class texture_handler
	{
	public:
	    texture_handler(const std::string &default_path){ default_file_path = default_path; }
		~texture_handler();

	    std::shared_ptr<GLuint> addTextureByFilename(const std::string &texture_handle, const std::string &file_name);
	    std::shared_ptr<GLuint> addTextureByPath(const std::string &texture_handle, const std::string &file_path);
	    std::shared_ptr<GLuint> getTexture(const std::string &texture_handle);

	    void addTextureUnloaded(const std::string &file_name, const std::string &file_path);
	    void addTextureUnloaded(const std::string &file_name);
	    void unloadTexture(const std::string &name);

	private:
		// map handle, gluint
		std::map<std::string, std::shared_ptr<GLuint> > map_gluints;

		//path, map handle
		std::map<std::string, std::string> map_paths;

		std::string default_file_path;
	};

	class line
	{
	public:
		line(glm::vec4 first, glm::vec4 second, glm::vec4 c);
		~line();

		void moveFirstRelative(glm::mat4 translation) { p1 = translation * p1; }
		void moveFirstAbsolute(glm::vec4 new_point) { p1 = new_point; }
		void moveSecondRelative(glm::mat4 translation) { p2 = translation * p2; }
		void moveSecondAbsolute(glm::vec4 new_point) { p2 = new_point; }

		void draw(const std::shared_ptr<ogl_context> &context, const std::shared_ptr<ogl_camera> &camera, bool absolute = false) const;

	private:
		glm::vec4 p1;
		glm::vec4 p2;

		std::shared_ptr<GLuint> VBO;
		std::shared_ptr<GLuint> VAO;
		glm::vec4 color;
	};

	class rectangle
	{
	public:
		rectangle(glm::vec2 centerpoint, glm::vec2 dimensions, glm::vec4 c);
		~rectangle();

		void draw(const std::shared_ptr<ogl_context> &context, const std::shared_ptr<ogl_camera> &camera, bool absolute = false) const;
		void draw(const std::shared_ptr<ogl_context> &context, const std::shared_ptr<ogl_camera> &camera,
			const glm::mat4 &model_matrix, bool absolute = false) const;
		void setColor(glm::vec4 c) { color = c; }

	private:
	    std::vector<float> vec_vertices;
		std::shared_ptr<GLuint> VBO;
		std::shared_ptr<GLuint> VAO;
		glm::vec4 color;
	};

	class vertex_data
	{
	public:
	    vertex_data(const std::vector<float> &p, const std::vector<float> &uv, const std::vector<float> &n) :
			v_data(p), vt_data(uv), vn_data(n) {
			v_count = p.size();
			vt_count = uv.size();
			vn_count = n.size();
			setVertexData();

			if (v_count < 3 || v_count > 4)
				throw;

			if (vt_count != 0 && vt_count != 2)
				throw;

			if (vn_count != 3 && vn_count != 0)
				throw;
		}

		~vertex_data() {};

		const int getUVOffset() const { return v_data.size() * sizeof(float); }
		const int getNOffset() const { return getUVOffset() + (vt_data.size() * sizeof(float)); }
		const int getStride() const { return all_data.size() * sizeof(float); }
		const int getVSize() const { return v_count; }
		const int getVTSize() const { return vt_count; }
		const int getVNSize() const { return vn_count; }
		const std::vector<float> getVData() const { return v_data; }
		const std::vector<float> getVTData() const { return vt_data; }
		const std::vector<float> getVNData() const { return vn_data; }

		//modifyPosition does not affect normals
		void modifyPosition(const glm::mat4 &translation_matrix);
		//rotate modifies position data and normals
		void rotate(const glm::mat4 &rotation_matrix);

		std::vector<float> getAllData() const { return all_data; }

		bool operator == (const vertex_data &other) const;
		bool operator != (const vertex_data &other) { return !((*this) == other); }

		float x, y, z, w;
		glm::vec2 xy;
		glm::vec3 xyz;
		glm::vec4 xyzw;

		float u, v;
		glm::vec2 uv;

		float n_x, n_y, n_z;
		glm::vec2 n_xy;
		glm::vec3 n_xyz;

	private:

		void setVertexData();
		std::vector<float> v_data;
		std::vector<float> vt_data;
		std::vector<float> vn_data;
		std::vector<float> vp_data;

		std::vector<float> all_data;

		unsigned short v_count, vt_count, vn_count, vp_count;
	};

	class mesh_data
	{
	public:
		mesh_data() : total_face_count(0), vertex_count(0) {};
		~mesh_data() {};

		void setMeshName(std::string n) { mesh_name = n; }
		void setMaterialName(std::string n) { material_name = n; }

		const std::string getMaterialName() const { return material_name; }
		const std::string getMeshlName() const { return mesh_name; }

		//this data keeps list of vertex information as used by OpenGL
		void addVData(const std::vector<float> &data) { all_v_data.insert(all_v_data.end(), data.begin(), data.end()); }
		void addVTData(const std::vector<float> &data) { all_vt_data.insert(all_vt_data.end(), data.begin(), data.end()); }
		void addVNData(const std::vector<float> &data) { all_vn_data.insert(all_vn_data.end(), data.begin(), data.end()); }
		void addVPData(const std::vector<float> &data) { all_vp_data.insert(all_vp_data.end(), data.begin(), data.end()); }
		void addFace(const std::vector<vertex_data> &data);
		void addTangentBitangent(const std::vector<glm::vec3> &tb);
		std::vector<glm::vec3> calcTangentBitangent(const std::vector<vertex_data> &face_data);

		const int getInterleaveStride() const { return interleave_stride; }
		const int getInterleaveVTOffset() const { return interleave_vt_offset; }
		const int getInterleaveVNOffset() const { return interleave_vn_offset; }
		const std::vector<float> getInterleaveData() const;
		const std::vector<float> getIndexedVertexData(std::vector<unsigned short> &indices) const;
		const std::vector<float> getIndexedVertexData() const;
		const std::vector<unsigned short> getElementIndex() const { return element_index; }

		//returns # of floats per vertex type
		const int getVSize() const { return v_size; }
		const int getVTSize() const { return vt_size; }
		const int getVNSize() const { return vn_size; }
		const int getTanSize() const { return tan_size; }
		const int getBitanSize() const { return bitan_size; }
		//returns # of vertices stored
		const int getVertexCount() const { return vertex_count; }
		//returns # of faces stored
		const int getFaceCount() const { return total_face_count; }
		const int getFloatCount() const { return total_float_count; }

		const std::vector<float> getVData() const { return all_v_data; }
		const std::vector<float> getVTData() const { return all_vt_data; }
		const std::vector<float> getVNData() const { return all_vn_data; }
		const std::vector<float> getVPData() const { return all_vp_data; }

		//modifyPosition does not affect normals
		void modifyPosition(const glm::mat4 &translation_matrix);
		//rotate modifies position data and normals
		void rotate(const glm::mat4 &rotation_matrix);

		std::vector<std::pair<glm::vec4, glm::vec4> > getMeshEdgesVec4() const;
		std::vector<std::pair<glm::vec3, glm::vec3> > getMeshEdgesVec3() const;
		std::vector<std::vector<glm::vec4> >getMeshTrianglesVec4() const;
		std::vector<std::vector<glm::vec3> >getMeshTrianglesVec3() const;

		std::map<unsigned short, vertex_data> getVertexMap() const { return vertex_map; }

		void setMeshData();

	private:
		//vector of faces, each face is a vector of vertices
		std::vector<std::vector<vertex_data> > faces;
		std::map<unsigned short, vertex_data > vertex_map;
		std::map<unsigned short, glm::vec3> tangent_map;
		std::map<unsigned short, glm::vec3> bitangent_map;

		std::vector<unsigned short> element_index;

		std::vector<glm::vec3> tangents;
		std::vector<glm::vec3> bitangents;

		std::string mesh_name;
		std::string material_name;

		//contain all vertices for all faces, accessed when
		//vertices are not to be interleaved
		std::vector<float> all_v_data;
		std::vector<float> all_vt_data;
		std::vector<float> all_vn_data;
		std::vector<float> all_vp_data;

		//interleaved data values
		int interleave_stride;
		int interleave_vt_offset;
		int interleave_vn_offset;

		//counts floats per vertex of each type (3 = vec3, 4 = vec4)
		int v_size;
		int vt_size;
		int vn_size;
		int vp_size;
		int tan_size;
		int bitan_size;
		//# of vertices are stored total
		int vertex_count;
		//# of faces are stored total
		int total_face_count;
		int total_float_count;
	};

	class obj_contents
	{
	public:
		obj_contents(const char* obj_file);
		~obj_contents() {};

		const std::map<int, std::vector<float> > getAllRawVData() const { return raw_v_data; }
		const std::map<int, std::vector<float> > getAllRawVTData() const { return raw_vt_data; }
		const std::map<int, std::vector<float> > getAllRawVNData() const { return raw_vn_data; }
		const std::map<int, std::vector<float> > getAllRawVPData() const { return raw_vp_data; }

		const std::vector<float> getRawVData(int n) const { return raw_v_data.at(n); }
		const std::vector<float> getRawVTData(int n) const { return raw_vt_data.at(n); }
		const std::vector<float> getRawVNData(int n) const { return raw_vn_data.at(n); }
		const std::vector<float> getRawVPData(int n) const { return raw_vp_data.at(n); }

		const int getMeshCount() const { return meshes.size(); }
		const std::vector<mesh_data> getMeshes() const { return meshes; }

		std::vector<std::string> getErrors() const { return error_log; }

		std::vector<glm::vec4> getAllVerticesOfAllMeshes() const;

		const std::string getMTLFilename() const { return mtl_filename; }

	private:
	    void addRawData(const std::vector<float> &floats, DATA_TYPE dt);

		//uses vector<float> because # of floats per vertex varies
		//data direct from obj file, unformatted
		std::map<int, std::vector<float> > raw_v_data;
		std::map<int, std::vector<float> > raw_vt_data;
		std::map<int, std::vector<float> > raw_vn_data;
		std::map<int, std::vector<float> > raw_vp_data;
		std::string mtl_filename;

		int v_index_counter;
		int vt_index_counter;
		int vn_index_counter;
		int vp_index_counter;

		std::vector<std::string> error_log;
		std::vector<mesh_data> meshes;
	};

	class material_data
	{
	public:
		material_data();
		material_data(const std::string &name,
			const std::shared_ptr<ogl_context> &existing_context,
			std::shared_ptr<texture_handler> &existing_textures);
		material_data(
		        const std::string &name,
			std::shared_ptr<ogl_context> &existing_context,
			std::shared_ptr<texture_handler> &existing_textures,
			const std::string &diffuse_name,
			const std::string &bump_name,
			const std::string &normal_name,
			const std::string &transparency_name,
			const std::string &specular_name);
		~material_data() {};

		void setMaterialName(std::string s) { material_name = s; }
		void setData(DATA_TYPE dt, std::vector<float> floats) { data[dt] = floats; }

		const std::string getMaterialName() const { return material_name; }
		const std::vector<float> getData(DATA_TYPE dt) const;

		void setMapStatus(const std::string &map_handle, bool b);
		void setBumpValue(const float &f) { bump_value = glm::clamp(f, 0.0f, 1.0f); }
		void setSpecularValue(const float &f) { specular_value = glm::clamp(f, 0.0f, 1.0f); }
		void setGlobalTransparency(const float &f) { global_transparency = glm::clamp(f, 0.0f, 1.0f); }
		void setSpecularColor(const glm::vec3 &color) { specular_color = color; }
		void setSpecularDampening(const int &i) { specular_dampening = (i < 0 ? 0 : i); }
		void setDefaultDiffuseColor(const glm::vec3 &color) { default_diffuse_color = color; }
		void setSpecularIgnoresTransparency(const bool b) { specular_ignores_transparency = b; }

		std::shared_ptr<GLuint> getGluint(const std::string &map_handle) const;
		std::string getTextureName(const std::string &map_handle) const;

		bool getMapStatus(const std::string &map_handle) const;
		float getBumpValue() const { return bump_value; }
		float getSpecularValue() const { return specular_value; }
		float getGlobalTransparency() const { return global_transparency; }
		glm::vec3 getSpecularColor() const { return specular_color; }
		int getSpecularDampening() const { return specular_dampening; }
		glm::vec3 getDefaultDiffuseColor() const { return default_diffuse_color; }
		bool getSpecularIgnoresTransparency() const { return specular_ignores_transparency; }

		void setShader() const;
		bool overrideMap(const std::string &map_handle, const std::shared_ptr<GLuint> &new_gluint);

		void setTextureData(const std::string &map_handle, const std::string &texture_handle);

	private:
	    std::string material_name;

		std::map<DATA_TYPE, std::vector<float> > data;

		void initializeTextureData();

		std::map<std::string, bool> map_statuses;
		std::map<std::string, std::string> texture_names;
		std::map<std::string, std::shared_ptr<GLuint> > texture_gluints;

		float bump_value = 0.5;
		float specular_value = 0.5;
		glm::vec3 specular_color = glm::vec3(1.0, 1.0, 1.0);
		int specular_dampening = 10;
		glm::vec3 default_diffuse_color = glm::vec3(0.5f, 0.5f, 0.5f);
		float global_transparency = 0.0;

		bool unique_texture = false;
		bool specular_ignores_transparency = true;

		std::shared_ptr<ogl_context> context;
		std::shared_ptr<texture_handler> textures;
	};

	class mtl_contents
	{
	public:
		mtl_contents(const char* mtl_file, std::shared_ptr<texture_handler> &textures, const std::shared_ptr<ogl_context> &context);
		~mtl_contents() {};

		//const string getTextureFilename(string material_name) const;
		const std::map<std::string, std::shared_ptr<material_data> >  getMaterials() const { return materials; }

	private:
	    std::vector<std::string> error_log;
		std::map<std::string, std::shared_ptr<material_data> > materials;
	};
}

#endif