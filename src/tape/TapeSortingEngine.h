#pragma once

#include <vector>
#include <memory>
#include <thread>

#include "thread_pool.h"

#include "ITapeEmulator.h"
#include "DataBlockSorter.h"
#include "TapeEmulatorFabric.h"
#include "TapeIterativeDataCollector.h"
#include "TapeMergeEngine.h"

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
    this->collector = TapeIterativeDataCollector<T, DataBlock>(input_tape_ptr, this->data_block_size);
}

template<typename T>
inline void TapeSortingEngine<T>::run()
{
    constexpr int blocks_total = LENGTH / BLOCK;
    int blocks_processed = 0;
	int blocks_completed = 0;

	auto merge_engine = std::make_shared<TapeMergeEngine<T, std::vector<T>>>(this->output_tape_ptr);

    auto start = std::chrono::high_resolution_clock::now();

    while (blocks_completed != blocks_total) {
        if (blocks_processed <= this->threads_count) {
            auto next_data_block = this->collector->collect_next_data();
            if (!next_data_block.empty()) {
                this->sorters_pool.insert_task_data(next_data_block);
                blocks_processed++;
            }
        }
        DataBlock sorted_block;
        this->sorters_pool.get_sorted_data(sorted_block);
        if (!sorted_block.empty()) {
			merge_engine.merge_block(sorted_block);
            auto blocks_completed_updated = merge_engine.merged_blocks();
            blocks_processed -= blocks_completed_updated - blocks_completed;
            blocks_completed = blocks_completed_updated;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "\nParallel: " << diff.count() << " us\n";
}
