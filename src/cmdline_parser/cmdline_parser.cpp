#include "cmdline_parser.h"

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

void cmdline_parser::set_usage_message(const std::string & usage_message)
{
	this->usage_message = usage_message;
}

void cmdline_parser::print_usage_message() const
{
	//std::cout << "\nUsage: tape.exe -i <input_file> -o <output_file>\n"
	//	"where <input_file> and <output_file> are .dat files with integers\n" << std::endl;
	std::cout << this->usage_message << std::endl;
}

cmdline_parser::parse_result cmdline_parser::parse(int argc, char ** argv)
{
	for (int i = 1; i < argc; i++) {
		auto option_key = argv[i]; 

	}
	return parse_result();
}

cmdline_parser::parse_result cmdline_parser::get_option(const std::string & key, std::string & value)
{
	auto result = parse_result::option_not_found;
	const auto iter = this->options.find(key);
	if (iter != this->options.end()) {
		const auto & [key, option] = *iter;
		value = option;
		result = parse_result::valid_option;
	}
	return result;
}
