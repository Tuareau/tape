#pragma once

#include <vector>
#include <memory>

#include "thread_pool.h"
#include "../../src/tape/ITapeEmulator.h"
#include "DataBlockSorter.h"

using data_block = std::vector<int>;

template <typename T>
class TapeSortingEngine
{
private:
	std::shared_ptr<ITapeEmulator<T>> input_tape_ptr;
	std::shared_ptr<ITapeEmulator<T>> output_tape_ptr;

	std::shared_ptr<DataBlockSorter> sorter;

	thread_pool<


public:



};

