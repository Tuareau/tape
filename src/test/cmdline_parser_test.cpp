#include "gtest/gtest.h"

#include "cmdline_parser.h"

TEST(CmdlineParserTests, ExpectedOptions) {

	const int argc = 5;
	const char * argv[] = { "tape.exe", "-i", "input.bin", "-o", "output.bin", "--undefined", "deadcafe.bin"};

	cmdline_parser cli;
	cli.parse(argc, argv);

	std::map<std::string, std::string> options;
	options.emplace("i", std::string());
	options.emplace("o", std::string());

	for (auto & option : options) {
		auto & [option_key, option_value] = option;
		auto parse_result = cli.get_option(option_key, option_value);
		EXPECT_EQ(parse_result, cmdline_parser::parse_result::valid_option);
	}

	EXPECT_TRUE(options.contains("i"));
	EXPECT_EQ(options["i"], std::string("input.bin"));
	EXPECT_TRUE(options.contains("o"));
	EXPECT_EQ(options["o"], std::string("output.bin"));
}