#include <fstream>
#include <map>

#include "cmdline_parser.h"
#include "yaml-cpp/yaml.h"

#include "TapeEmulatorFabric.h"
#include "TapeSettingsLoader.h"

//#define CMD_PARSER_TEST
//#define YAML_TEST
//#define BIN_FILE_TEST
#define TAPE_TEST

int main(int argc, char ** argv)
{
#ifdef CMD_PARSER_TEST
	const std::string usage_message{
		"\nUsage: tape -i <input_file> -o <output_file>\n"
		"where <input_file> and <output_file> are .bin files with 4-byte signed integers\n"
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
#endif

#ifdef YAML_TEST

	YAML::Node config = YAML::LoadFile("tape_settings.yaml");

	if (config["input_tape_settings"]) {
		auto input_tape_settings = config["input_tape_settings"];

		if (input_tape_settings["setting_id"]) {
			auto setting_id = input_tape_settings["setting_id"].as<double>();
			auto wait = 0;
		}

		auto setting_id = input_tape_settings["setting_id"].as<std::string>();
		auto tape_write_delay = input_tape_settings["tape_write_delay"].as<int>();
		auto tape_read_delay = input_tape_settings["tape_read_delay"].as<int>();
		auto tape_shift_delay = input_tape_settings["tape_shift_delay"].as<int>();
	}

	if (config["output_tape_settings"]) {
		auto output_tape_settings = config["output_tape_settings"];
		auto setting_id = output_tape_settings["setting_id"].as<std::string>();
		auto tape_write_delay = output_tape_settings["tape_write_delay"].as<int>();
		auto tape_read_delay = output_tape_settings["tape_read_delay"].as<int>();
		auto tape_shift_delay = output_tape_settings["tape_shift_delay"].as<int>();
	}

#endif


#ifdef BIN_FILE_TEST
	std::ofstream ofile("input.bin", std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
	int x = 0;
	while (ofile.good() && x < 20) {
		int d = static_cast<int>(std::pow(x++, 2));
		ofile.write(reinterpret_cast<const char *>(&d), sizeof(int));
	}
	std::cout << std::endl;
	ofile.close();

	std::ifstream ifile("input.bin", std::ios_base::binary | std::ios_base::in);
	while (ifile.good() && !ifile.eof()) {
		int x = 0;
		if (ifile.read(reinterpret_cast<char *>(&x), sizeof(int))) {
			std::cout << x << "\n";
		}
	}
	std::cout << std::endl;
	ifile.close();

	std::fstream file("input.bin", std::ios_base::binary | std::ios_base::out | std::ios_base::in);
	if (file.good()) {
		int x = -77;
		std::streampos pos = 5 * sizeof(int);
		file.seekp(pos, std::ios_base::beg);
		file.write(reinterpret_cast<const char *>(&x), sizeof(int));
		auto wr_pos = file.tellp();
		std::cout << "WRITE = " << wr_pos / sizeof(int) << ": " << x << std::endl;

		pos = 3 * sizeof(int);
		file.seekg(pos, std::ios_base::beg);
		file.read(reinterpret_cast<char *>(&x), sizeof(int));
		auto rd_pos = file.tellg();
		std::cout << "READ = " << rd_pos / sizeof(int) << ": " << x << std::endl;

	}
	file.close();

	std::ifstream ifiled("input.bin", std::ios_base::binary | std::ios_base::in);
	while (ifiled.good() && !ifiled.eof()) {
		int x = 0;
		if (ifiled.read(reinterpret_cast<char *>(&x), sizeof(int))) {
			std::cout << x << "\n";
		}
	}
	std::cout << std::endl;
	ifiled.close();
#endif


#ifdef TAPE_TEST
	//auto output_tape = TapeEmulatorFabric<int>::CreateEmulator("tape.bin");

	//output_tape->open_tape();

	//for (int i = 0; i < 20; i++) {
	//	if (output_tape->good()) {
	//		output_tape->write_element(static_cast<int>(std::pow(i, 2)));
	//		output_tape->shift_forward();			
	//	}
	//}

	//output_tape->close_tape();

	auto tape_settings_loader = std::make_shared<TapeSettingsLoader>("tape_settings.yaml");
	auto input_tape_settings_ptr = std::make_shared<TapeSettings>();
	auto load_result = tape_settings_loader->load_setting("input_tape_settings", input_tape_settings_ptr);
	if (load_result != TapeSettingsLoader::load_result::load_success) {
		return 1;
	}

	auto input_tape = TapeEmulatorFabric<int>::CreateEmulator("input.bin", input_tape_settings_ptr);

	input_tape->open_tape();

	std::cout << "Tape:" << std::endl;
	while (input_tape->good()) {
		int element = 0;
		auto state = input_tape->read_element(element);
		if (input_tape->good()) {
			std::cout << "element " << element << std::endl;
		}
		input_tape->shift_forward();
	}

	input_tape->close_tape();
#endif

	return 0;
}