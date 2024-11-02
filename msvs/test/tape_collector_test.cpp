#include "gtest/gtest.h"

#include "TapeEmulatorFabric.h"
#include "TapeIterativeDataCollector.h"

#define TAPE_LENGTH 100
#define DATA_BLOCK_SIZE 10

TEST(TapeCollectorTests, TapeCollectorTest)
{
	auto tape = TapeEmulatorFabric<int>::CreateEmulator("test_collector_tape.bin");
	tape->open_tape();
	for (int i = 0; i < TAPE_LENGTH; i++) {
		if (tape->good()) {
			auto element = static_cast<int>(std::pow(i, 2));
			tape->write_element(element);
			tape->shift_forward();
		}
	}

	tape->reset_tape();
	std::vector<int> reference_data_block;
	while (tape->good()) {
		int element = 0;
		auto state = tape->read_element(element);
		if (tape->good()) {
			reference_data_block.push_back(element);
		}
		tape->shift_forward();
		if (reference_data_block.size() == DATA_BLOCK_SIZE) {
			break;
		}
	}
	tape->close_tape();

	auto test_tape = TapeEmulatorFabric<int>::CreateEmulator("test_collector_tape.bin");
	TapeIterativeDataCollector<int, std::vector<int>> collector(test_tape, DATA_BLOCK_SIZE);

	auto data_block_ptr = collector.collect_next_data();

	EXPECT_EQ(*data_block_ptr, reference_data_block);
}