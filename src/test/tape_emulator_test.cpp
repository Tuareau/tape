#include "gtest/gtest.h"

#include "yaml-cpp/yaml.h"

#include "TapeEmulatorFabric.h"
#include "TapeSettingsLoader.h"

#define CONFIG_FILENAME "tape_settings.yaml"

#define TAPE_LENGTH 20
#define TAPE_SETTING_ID "tape_3"

TEST(TapeFunctionalTests, WriteTapeAndReadWithAnother)
{
	std::vector<int> output_tape_elements;
	auto output_tape = TapeEmulatorFabric<int>::CreateEmulator("tape.bin");
	output_tape->open_tape();
	for (int i = 0; i < TAPE_LENGTH; i++) {
		if (output_tape->good()) {
			auto element = static_cast<int>(std::pow(i, 2));
			output_tape_elements.push_back(element);
			output_tape->write_element(element);
			output_tape->shift_forward();			
		}
	}
	output_tape->close_tape();

	auto tape_settings_loader = std::make_shared<TapeSettingsLoader>("tape_settings.yaml");
	auto input_tape_settings_ptr = std::make_shared<TapeSettings>();
	auto load_result = tape_settings_loader->load_setting(TAPE_SETTING_ID, input_tape_settings_ptr);
	EXPECT_EQ(load_result, TapeSettingsLoader::load_result::load_success);

	auto input_tape = TapeEmulatorFabric<int>::CreateEmulator("tape.bin", input_tape_settings_ptr);
	input_tape->open_tape();

	std::vector<int> input_tape_elements;
	while (input_tape->good()) {
		int element = 0;
		auto state = input_tape->read_element(element);
		if (input_tape->good()) {
			input_tape_elements.push_back(element);
		}
		input_tape->shift_forward();
	}
	input_tape->close_tape();

	EXPECT_EQ(input_tape_elements.size(), output_tape_elements.size());
	for (int i = 0; i < input_tape_elements.size(); i++) {
		EXPECT_EQ(input_tape_elements[i], output_tape_elements[i]);
	}	
}