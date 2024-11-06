#pragma once

#include <vector>
#include <array>
#include <memory>
#include <thread>

#include "threadsafe_queue.h"
#include "ITapeEmulator.h"
#include "DataBlockSorter.h"
#include "TapeEmulatorFabric.h"

template <typename T, typename Container>
class TapeMergeEngine
{
private:
	using TapePtr = std::shared_ptr<ITapeEmulator<T>>;
	TapePtr output_tape_ptr;

	struct TempTapesController {
		constexpr size_t temp_tapes_count = 8;
		std::array<size_t, temp_tapes_count> temp_tapes_sizes;
	};

	TempTapesController tapes_controller;

	threadsafe_queue<Container> data_queue;

	size_t merged_blocks_count;

	void merge_temp_tapes();

public:
	TapeMergeEngine(TapePtr output_tape_ptr);

	void push_block(Container container);
	size_t merged_blocks() const;
	void complete_merge();

};

template <typename T, typename Container>
inline TapeMergeEngine<typename T, typename Container>::TapeMergeEngine(TapePtr output_tape_ptr)
{
	this->output_tape_ptr = output_tape_ptr;
}

template<typename T, typename Container>
inline void TapeMergeEngine<T, Container>::push_block(Container container)
{
	this->data_queue->push(container);
}

template<typename T, typename Container>
inline size_t TapeMergeEngine<T, Container>::merged_blocks() const
{
	return this->merged_blocks_count;
}
