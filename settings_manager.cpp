#include "settings_manager.h"

void settings_manager::init()
{
	string_settings["base_seed"] = "";	

	int_settings["refresh_value"] = 5;	
	int_settings["num_points"] = 10000;
	int_settings["window_width"] = 1366;
	int_settings["window_height"] = 768;
	int_settings["background_front_index"] = 0;
	int_settings["background_back_index"] = 0;
	int_settings["generation"] = 0;
	int_settings["refresh_min"] = 3;
	int_settings["refresh_max"] = 15;
	int_settings["translate_weight"] = 1; //randomize
	int_settings["rotate_weight"] = 1; //randomize
	int_settings["scale_weight"] = 1; //randomize
	int_settings["num_matrices"] = 5; //randomize

	/*int num_matrices = int(mc.getRandomFloatInRange(2, 10));
	translate_weight = int(mc.getRandomFloatInRange(1, 10));
	rotate_weight = int(mc.getRandomFloatInRange(1, 10));
	scale_weight = int(mc.getRandomFloatInRange(1, 10));*/

	float_settings["line_width"] = 1.0f;
	float_settings["interpolation_state"] = 0.0f;
	float_settings["interpolation_increment"] = 0.02f;
	float_settings["alpha_min"] = 0.0f;
	float_settings["alpha_max"] = 1.0f;

	bool_settings["auto_tracking"] = false;
	bool_settings["smooth"] = true;
	bool_settings["use_point_sequence"] = false;
	bool_settings["two_dimensional"] = false;
	bool_settings["refresh_enabled"] = false;
	bool_settings["size_enabled"] = true;
	bool_settings["show_points"] = true;
	bool_settings["enable_triangles"] = false;
	bool_settings["enable_lines"] = false;
	bool_settings["show_palette"] = false;
	bool_settings["smooth_render"] = true;
	bool_settings["randomize_lightness"] = true;
	bool_settings["randomize_alpha"] = true;
	bool_settings["reverse"] = false;
	bool_settings["print_context_on_swap"] = false;
	bool_settings["lighting_enabled"] = false;
	bool_settings["inverted"] = false;
	bool_settings["scale_matrices"] = true;
	bool_settings["solid_geometry"] = true;
}

string settings_manager::getString(string name) const
{
	if (string_settings.find(name) != string_settings.cend())
		return string_settings.at(name);

	else return "";
}

int settings_manager::getInt(string name) const
{
	if (int_settings.find(name) != int_settings.cend())
		return int_settings.at(name);

	else return -1;
}

bool settings_manager::getBool(string name) const
{
	if (bool_settings.find(name) != bool_settings.cend())
		return bool_settings.at(name);

	else return false;
}

float settings_manager::getFloat(string name) const
{
	if (float_settings.find(name) != float_settings.cend())
		return float_settings.at(name);

	else return 0.0f;
}

bool settings_manager::setString(string name, string value)
{
	if (string_settings.find(name) != string_settings.cend())
	{
		string_settings[name] = value;
		return true;
	}

	else return false;
}

bool settings_manager::setInt(string name, int value)
{
	if (int_settings.find(name) != int_settings.cend())
	{
		int_settings[name] = value;
		return true;
	}

	else return false;
}

bool settings_manager::setBool(string name, bool value)
{
	if (bool_settings.find(name) != bool_settings.cend())
	{
		bool_settings[name] = value;
		return true;
	}

	else return false;
}

bool settings_manager::setFloat(string name, float value)
{
	if (float_settings.find(name) != float_settings.cend())
	{
		float_settings[name] = value;
		return true;
	}

	else return false;
}

bool settings_manager::addString(string name, string value)
{
	if (string_settings.find(name) == string_settings.cend())
	{
		string_settings[name] = value;
		return true;
	}

	else return false;
}

bool settings_manager::addInt(string name, int value)
{
	if (int_settings.find(name) == int_settings.cend())
	{
		int_settings[name] = value;
		return true;
	}

	else return false;
}

bool settings_manager::addBool(string name, bool value)
{
	if (bool_settings.find(name) == bool_settings.cend())
	{
		bool_settings[name] = value;
		return true;
	}

	else return false;
}

bool settings_manager::addFloat(string name, float value)
{
	if (float_settings.find(name) == float_settings.cend())
	{
		float_settings[name] = value;
		return true;
	}

	else return false;
}

bool settings_manager::toggleBool(string name)
{
	if (bool_settings.find(name) != bool_settings.cend())
	{
		bool_settings[name] = !bool_settings.at(name);
		return true;
	}

	else return false;
}

void settings_manager::copySettings(const settings_manager &other)
{
	string_settings = other.string_settings;
	int_settings = other.int_settings;
	float_settings = other.float_settings;
	bool_settings = other.bool_settings;
}

