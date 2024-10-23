#include "cmdline_parser.h"

const std::regex cmdline_parser::regex_short_option = std::regex(REGEX_SHORT_OPTION);
const std::regex cmdline_parser::regex_long_option = std::regex(REGEX_LONG_OPTION);
const std::regex cmdline_parser::regex_long_option_with_suffix = std::regex(REGEX_LONG_OPTION_WITH_SUFFIX);

std::regex cmdline_parser::get_option_regex(const std::string & option) const
{
	std::regex option_regex;
	if (std::regex_match(option, cmdline_parser::regex_short_option)) {
		option_regex = cmdline_parser::regex_short_option;
	}
	if (std::regex_match(option, cmdline_parser::regex_long_option)) {
		option_regex = cmdline_parser::regex_long_option;
	}
	if (std::regex_match(option, cmdline_parser::regex_long_option_with_suffix)) {
		option_regex = cmdline_parser::regex_long_option_with_suffix;
	}
	return option_regex;
}

cmdline_parser::option_type cmdline_parser::get_option_type(const std::string & option) const
{
	auto type = option_type::none;
	if (std::regex_match(option, cmdline_parser::regex_short_option)) {
		type = option_type::short_option;
	}
	if (std::regex_match(option, cmdline_parser::regex_long_option)) {
		type = option_type::long_option;
	}
	if (std::regex_match(option, cmdline_parser::regex_long_option_with_suffix)) {
		type = option_type::long_option_with_suffix;
	}
	return type;
}

std::string cmdline_parser::extract_option_name(const std::string & option) const
{
	return std::string();
}

void cmdline_parser::set_usage_message(const std::string & usage_message)
{
	this->usage_message = usage_message;
}

void cmdline_parser::print_usage_message() const
{
	std::cout << this->usage_message << std::endl;
}

void cmdline_parser::print_options() const
{
	std::cout << "\nParsed options:\n";
	if (this->options.empty()) {
		std::cout << "\tno options parsed\n";
	}
	for (const auto & option : this->options) {
		const auto & [key, value] = option;
		std::cout << "\toption: " << key << " = " << value << std::endl;
	}
}

cmdline_parser::parse_result cmdline_parser::parse(int argc, const char ** argv)
{
	auto parse_result = parse_result::no_options;
	for (int i = 1; i < argc; i++) {
		const std::string argument{ argv[i] };
		const auto option_type = this->get_option_type(argument);
		const auto option_type_regex = this->get_option_regex(argument);
		if (option_type == option_type::long_option_with_suffix) {
			const auto key = std::regex_replace(argument, option_type_regex, "$1");
			const auto value = std::regex_replace(argument, option_type_regex, "$2");
			this->options.emplace(key, value);
		}
		else if (option_type != option_type::none) {
			if (argc > i + 1) {
				const std::string value{ argv[i + 1] };
				if (this->get_option_type(value) == option_type::none) {
					this->options.emplace(std::regex_replace(argument, option_type_regex, "$1"), value);
				}
			}
		}
	}
	if (!this->options.empty()) {
		parse_result = parse_result::parse_success;
	}
	return parse_result;
}

cmdline_parser::parse_result cmdline_parser::get_option(const std::string & key, std::string & value)
{
	auto result = parse_result::option_not_found;
	if (this->options.contains(key)) {
		value = this->options.at(key);
		result = parse_result::valid_option;
	}
	return result;
}

bool cmdline_parser::contains_option(const std::string & key)
{
	return this->options.contains(key);
}
