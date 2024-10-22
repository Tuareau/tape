#pragma once

#include <iostream>
#include <string>
#include <memory>

#include "TapeFstreamEmulator.h"

#include "yaml-cpp/yaml.h"

class TapeSettingsLoader
{
public:
	TapeSettingsLoader() = default;
	TapeSettingsLoader(const std::string & filename);

	enum class load_result
	{
		load_success,
		file_not_opened,
		file_parse_error,
		setting_not_found,
		option_not_found,
		invalid_option,
	};

	load_result load_setting(const std::string & setting_key, std::shared_ptr<TapeSettings> & settings_ptr);
	load_result print_setting(const std::string & setting_key, std::shared_ptr<TapeSettings> & settings_ptr);
	void print_result_description(load_result result);

private:
	std::string settings_filename;

	load_result load_option_string(const YAML::Node & node, const std::string & option, std::string & value);
	load_result load_option_int(const YAML::Node & node, const std::string & option, int & value);
};