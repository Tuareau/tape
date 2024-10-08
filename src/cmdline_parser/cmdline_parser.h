#pragma once

#include <map>
#include <string>

using options_map = std::map<std::string, std::string>;

class cmdline_parser
{
private:
	options_map options;
	

public:
	cmdline_parser() = default;
	cmdline_parser(const cmdline_parser &) = default;
	cmdline_parser(cmdline_parser &&) = default;
	cmdline_parser & operator=(const cmdline_parser &) = default;
	cmdline_parser & operator=(cmdline_parser &&) = default;
	~cmdline_parser() = default;

	enum class parse_result
	{
		option_not_found,
		argument_not_found,
		valid_argument
	};
};