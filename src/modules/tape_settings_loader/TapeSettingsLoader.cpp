#include "TapeSettingsLoader.h"

TapeSettingsLoader::TapeSettingsLoader(const std::string & filename)
{
	this->settings_filename = filename;
}

TapeSettingsLoader::load_result TapeSettingsLoader::load_setting(const std::string & setting_key, std::shared_ptr<TapeSettings> & settings_ptr)
{
	load_result result = load_result::setting_not_found;

	YAML::Node config;
	try {
		config = YAML::LoadFile(this->settings_filename);
	}
	catch (const YAML::BadFile & badfile_exept) {
		result = load_result::file_not_opened;
		return result;
	}
	catch (const YAML::ParserException & parser_exept) {
		result = load_result::file_parse_error;
		return result;
	}

	if (config[setting_key]) {
		const auto & node = config[setting_key];
		this->load_option_string(node, "setting_id", settings_ptr->setting_id);
		int time_ms = 0;
		this->load_option_int(node, "tape_write_delay", time_ms);
		settings_ptr->tape_write_delay = std::chrono::milliseconds(time_ms);
		this->load_option_int(node, "tape_read_delay", time_ms);
		settings_ptr->tape_read_delay = std::chrono::milliseconds(time_ms);
		this->load_option_int(node, "tape_shift_delay", time_ms);
		settings_ptr->tape_shift_delay = std::chrono::milliseconds(time_ms);
		result = load_result::load_success;
	}
	else {
		result = load_result::setting_not_found;
	}
	return result;
}

TapeSettingsLoader::load_result TapeSettingsLoader::print_setting(const std::string & setting_key, std::shared_ptr<TapeSettings> & settings_ptr)
{
	load_result result = this->load_setting(setting_key, settings_ptr);
	std::cout << *settings_ptr;
	return result;
}

void TapeSettingsLoader::print_result_description(load_result result)
{
	switch (result) {
		case load_result::file_not_opened:
			std::cout << "\nFile " << this->settings_filename << " was not opened";
			break;
		case load_result::file_parse_error:
			std::cout << "\nFile " << this->settings_filename << " parse error";
			break;
		case load_result::invalid_option:
			std::cout << "\nInvalid option";
			break;
		case load_result::load_success:
			std::cout << "\nLoaded successfuly";
			break;
		case load_result::setting_not_found:
			std::cout << "\nSetting not found";
			break;
		case load_result::option_not_found:
			std::cout << "\nOption not found";
			break;
	}
}

TapeSettingsLoader::load_result TapeSettingsLoader::load_option_string(const YAML::Node & node, const std::string & option, std::string & value)
{
	auto result = load_result::invalid_option;
	if (node[option]) {
		value = node[option].as<std::string>();
		result = load_result::load_success;
	}
	return result;
}

TapeSettingsLoader::load_result TapeSettingsLoader::load_option_int(const YAML::Node & node, const std::string & option, int & value)
{
	auto result = load_result::invalid_option;
	if (node[option]) {
		value = node[option].as<int>();
		result = load_result::load_success;
	}
	return result;
}
