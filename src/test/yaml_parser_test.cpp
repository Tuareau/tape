#include "gtest/gtest.h"

#include <fstream>
#include <sstream>
#include <chrono>

#include "yaml-cpp/yaml.h"
#include "TapeFstreamEmulator.h"

#define NODES 5

#define WRITE_DELAY_STEP 15
#define READ_DELAY_STEP 5
#define SHIFT_DELAY_STEP 20

#define CONFIG_FILENAME "tape_settings.yaml"

bool EqualTapeSettings(const TapeSettings & lhs, const TapeSettings & rhs)
{
	bool equal = lhs.setting_id == rhs.setting_id
		&& lhs.tape_read_delay == rhs.tape_read_delay
		&& lhs.tape_shift_delay == rhs.tape_shift_delay
		&& lhs.tape_write_delay == rhs.tape_write_delay;
	return equal;
}

TEST(YamlCppParserTests, WriteAndReadConfig)
{
	// generate settings for YAML nodes
	std::vector<TapeSettings> tapes_settings_out;
	for (int i = 0; i < NODES; i++) {
		TapeSettings setting;
		setting.setting_id = (std::stringstream() << i).str();
		setting.tape_write_delay = std::chrono::milliseconds(i * WRITE_DELAY_STEP);
		setting.tape_read_delay = std::chrono::milliseconds(i * READ_DELAY_STEP);
		setting.tape_shift_delay = std::chrono::milliseconds(i * SHIFT_DELAY_STEP);
		tapes_settings_out.push_back(std::move(setting));
	}

	// fill YAML nodes with settings
	std::vector<YAML::Node> tape_content_nodes;
	for (const auto & setting : tapes_settings_out) {
		YAML::Node node;
		node["setting_id"] = setting.setting_id;
		node["tape_write_delay"] = setting.tape_write_delay.count();
		node["tape_read_delay"] = setting.tape_read_delay.count();
		node["tape_shift_delay"] = setting.tape_shift_delay.count();
		tape_content_nodes.push_back(std::move(node));
	}

	// create YAML tape_node for each tape setting
	std::vector<YAML::Node> tape_nodes;
	for (const auto & tape_content_node : tape_content_nodes) {
		YAML::Node tape_node;
		const auto node_key = std::string("tape_") + tape_content_node["setting_id"].as<std::string>();
		tape_node[node_key] = tape_content_node;
		tape_nodes.push_back(std::move(tape_node));
	}

	// write YAML file with generated nodes
	std::ofstream fout(CONFIG_FILENAME, std::ios_base::out | std::ios_base::trunc);
	for (const auto & tape : tape_nodes) {
		fout << tape << std::endl;
	}
	fout.close();

	// parse generated YAML file
	YAML::Node yaml_nodes = YAML::LoadFile(CONFIG_FILENAME);

	// collect each tape setting
	std::vector<TapeSettings> tapes_settings_in;
	for (YAML::const_iterator tape_iter = yaml_nodes.begin(); tape_iter != yaml_nodes.end(); ++tape_iter) {
		TapeSettings setting;
		setting.setting_id = tape_iter->second["setting_id"].as<std::string>();
		setting.tape_write_delay = std::chrono::milliseconds(tape_iter->second["tape_write_delay"].as<int>());
		setting.tape_read_delay = std::chrono::milliseconds(tape_iter->second["tape_read_delay"].as<int>());
		setting.tape_shift_delay = std::chrono::milliseconds(tape_iter->second["tape_shift_delay"].as<int>());
		tapes_settings_in.push_back(std::move(setting));
	}

	// check that generated settings are equal to which were parsed
	EXPECT_EQ(tapes_settings_in.size(), tapes_settings_out.size());
	for (int i = 0; i < tapes_settings_in.size(); i++) {
		EXPECT_TRUE(EqualTapeSettings(tapes_settings_in[i], tapes_settings_out[i]));
	}
}