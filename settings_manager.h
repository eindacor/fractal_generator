#pragma once

#include "header.h"
#include "matrix_creator.h"

class settings_manager
{
public:
	settings_manager() { init(); }
	~settings_manager() {};

	void init();

	string getString(string name) const;
	int getInt(string name) const;
	float getFloat(string name) const;
	bool getBool(string name) const;

	bool setString(string name, string value);
	bool setInt(string name, int value);
	bool setFloat(string name, float value);
	bool setBool(string name, bool value);

	bool addString(string name, string value);
	bool addInt(string name, int value);
	bool addFloat(string name, float value);
	bool addBool(string name, bool value);

	bool toggleBool(string name);

	void operator = (const settings_manager &other) { copySettings(other); }
	void copySettings(const settings_manager &other);

	std::map<string, string> string_settings;
	std::map<string, int> int_settings;
	std::map<string, bool> bool_settings;
	std::map<string, float> float_settings;
};