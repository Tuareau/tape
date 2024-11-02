#pragma once

#include <vector>
#include <memory>
#include <thread>

#include "thread_pool.h"
#include "../../src/tape/ITapeEmulator.h"
#include "DataBlockSorter.h"
#include "../../src/tape/TapeEmulatorFabric.h"

template <typename T>
class TapeMergeEngine
{
private:
	using TapePtr = std::shared_ptr<ITapeEmulator<T>>;
	constexpr size_t temp_tapes_count 8;
	std::vector<TapePtr> tapes;
	TapePtr input_tape_ptr;
	TapePtr output_tape_ptr;

	using IterableDataBlock = std::vector<T>;
	thread_pool<DataBlockSorter, IterableDataBlock> thread_pool;


public:
	TapeMergeEngine();

	void run();

};

template<typename T>
inline TapeMergeEngine<T>::TapeMergeEngine(TapePtr input_tape_ptr, TapePtr output_tape_ptr)
{
	this->input_tape_ptr = input_tape_ptr;
	this->output_tape_ptr = output_tape_ptr;
	this->thread_pool = thread_pool<DataBlockSorter, IterableDataBlock>(std::thread::hardware_concurrency());
}
