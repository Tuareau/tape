#pragma once

#include <map>
#include <string>
#include <iostream>
#include <regex>

#define REGEX_SHORT_OPTION              "(-)([A-Za-z0-9]+)"
#define REGEX_LONG_OPTION               "-{1,2}([A-Za-z0-9]+)"
#define REGEX_LONG_OPTION_WITH_SUFFIX   "-{1,2}([A-Za-z0-9]+)=([^=]+)"

using options_map = std::map<std::string, std::string>;

class cmdline_parser
{
private:
	options_map options;
	std::string usage_message;

	static const std::regex regex_short_option;
	static const std::regex regex_long_option;
	static const std::regex regex_long_option_with_suffix;

	enum class option_type
	{
		none,
		short_option,
		long_option,
		long_option_with_suffix,
	};

	option_type get_option_type(const std::string & option) const;

public:
	cmdline_parser() = default;
	cmdline_parser(const cmdline_parser &) = default;
	cmdline_parser(cmdline_parser &&) = default;
	cmdline_parser & operator=(const cmdline_parser &) = default;
	cmdline_parser & operator=(cmdline_parser &&) = default;
	~cmdline_parser() = default;



	enum class parse_result
	{
		parse_success,
		option_not_found,
		valid_option,
	};

	void set_usage_message(const std::string & usage_message);
	void print_usage_message() const;
	parse_result parse(int argc, char ** argv);
	parse_result get_option(const std::string & key, std::string & value);
};

const std::regex cmdline_parser::regex_short_option = std::regex(REGEX_SHORT_OPTION);
const std::regex cmdline_parser::regex_long_option = std::regex(REGEX_LONG_OPTION);
const std::regex cmdline_parser::regex_long_option_with_suffix = std::regex(REGEX_LONG_OPTION_WITH_SUFFIX);