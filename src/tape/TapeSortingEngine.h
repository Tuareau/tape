#pragma once

#include <vector>
#include <memory>
#include <thread>

#include "thread_pool.h"

#include "ITapeEmulator.h"
#include "DataBlockSorter.h"
#include "TapeEmulatorFabric.h"
#include "TapeIterativeDataCollector.h"

#define LENGTH 10000000
#define BLOCK 1000000

#define THREADS 4

template <typename T>
class TapeSortingEngine
{
private:
	using TapePtr = std::shared_ptr<ITapeEmulator<T>>;

	TapePtr input_tape_ptr;
	TapePtr output_tape_ptr;

	using DataBlock = std::vector<T>;
	constexpr size_t threads_count = 4;
	constexpr size_t data_block_size = 131072000;
	thread_pool<DataBlockSorter, DataBlock> sorters_pool;

	struct DataBlocksCounter {
		std::mutex mutex;
		size_t counter;
		size_t threshold;
	};

	DataBlocksCounter data_blocks_counter;

    TapeIterativeDataCollector<T, DataBlock> collector;

public:
	TapeSortingEngine(TapePtr input_tape_ptr, TapePtr output_tape_ptr);

	void run();

};

template<typename T>
inline TapeSortingEngine<T>::TapeSortingEngine(TapePtr input_tape_ptr, TapePtr output_tape_ptr)
{
	this->input_tape_ptr = input_tape_ptr;
	this->output_tape_ptr = output_tape_ptr;
	this->sorters_pool = thread_pool<DataBlockSorter<DataBlock>, DataBlock>(this->threads_count);
	this->data_blocks_counter.counter = 0;
	this->data_blocks_counter.threshold = this->data_block_size / this->threads_count;
    this->collector = TapeIterativeDataCollector<T, DataBlock>(input_tape_ptr, this->data_block_size);
}

template<typename T>
inline void TapeSortingEngine<T>::run()
{
    constexpr int blocks_total = LENGTH / BLOCK;
    int blocks_sorted = 0;

    auto start = std::chrono::high_resolution_clock::now();

    while (blocks_sorted != blocks_total) {
        auto next_data_block = this->collector->collect_next_data();
        if (!next_data_block.empty()) {
            this->sorters_pool.insert_task_data(next_data_block);
        }
        DataBlock sorted_block;
        pool.get_processed_data(sorted_block);
        if (!sorted_block.empty()) {
            blocks_sorted++;
            // TODO: MergeEngine get block
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "\nParallel: " << diff.count() << " us\n";
}
