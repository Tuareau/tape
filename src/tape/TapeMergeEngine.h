#pragma once

#include <vector>
#include <array>
#include <memory>
#include <thread>
#include <queue>
#include <filesystem>

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

	constexpr size_t temp_tapes_count = 8;
	threadsafe_queue<ITapeEmulator<T>> temp_tapes_queue;

	threadsafe_queue<Container> data_queue;

	std::mutex merged_blocks_mutex;
	size_t merged_blocks_count;

	std::vector<std::future<void>> merge_futures;
	void merge_block_async(Container data_block);

public:
	TapeMergeEngine(TapePtr output_tape_ptr);

	void merge_block(Container container);
	size_t merged_blocks() const;
	TapePtr complete_merge();

};

template<typename T, typename Container>
inline void TapeMergeEngine<T, Container>::merge_block_async(Container data_block)
{
	auto temp_source_tape = this->temp_tapes_queue.wait_and_pop();
	auto container = this->data_queue.wait_and_pop();

	// TODO: async merging
	
	auto temp_tapes_queue.front();
}

template <typename T, typename Container>
inline TapeMergeEngine<typename T, typename Container>::TapeMergeEngine(TapePtr output_tape_ptr)
{
	this->output_tape_ptr = output_tape_ptr;
	for (int i = 0; i, this->temp_tapes_count; i++) {
		auto tmp_dir = std::filesystem::temp_directory_path();
		auto file_path = tmp_dir / std::filesystem::path("temp_tape_") / std::filesystem::path(std::to_string(i));
		auto temp_tape = TapeEmulatorFabric<T>::CreateEmulator(file_path.string());
		this->temp_tapes_queue.push(temp_tape);
	}
}

template<typename T, typename Container>
inline void TapeMergeEngine<T, Container>::merge_block(Container container)
{
	this->data_queue->push(container);
	auto merge_future = std::async(std::launch::async, TapeMergeEngine<T, Container>::merge_block_async(), this);
	this->merge_futures.push_back(merge_future);
}

template<typename T, typename Container>
inline size_t TapeMergeEngine<T, Container>::merged_blocks() const
{
	return this->merged_blocks_count;
}

template<typename T, typename Container>
inline TapeMergeEngine<T, Container>::TapePtr TapeMergeEngine<T, Container>::complete_merge()
{
	return TapePtr();
}
