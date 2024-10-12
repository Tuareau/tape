#include <fstream>
#include <map>

#include "cmdline_parser.h"

int main(int argc, char ** argv)
{
	const std::string usage_message{
		"\nUsage: tape -i <input_file> -o <output_file>\n"
		"where <input_file> and <output_file> are .dat files with integers\n"
	};

	cmdline_parser cli;
	cli.set_usage_message(usage_message);
	cli.parse(argc, argv);

	std::map<std::string, std::string> options;
	options.emplace("i", std::string());
	options.emplace("o", std::string());

	for (auto & option : options) {
		auto & [option_key, option_value] = option;
		auto parse_result = cli.get_option(option_key, option_value);
		if (parse_result != cmdline_parser::parse_result::valid_option) {
			cli.print_usage_message();
			std::exit(0);
		}
	}

	cli.print_options();

	return 0;
}