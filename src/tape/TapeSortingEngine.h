#pragma once

#include <vector>
#include <memory>
#include <thread>

#include "sort_thread_pool.h"
#include "merge_thread_pool.h"

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

    std::queue<TapePtr> temp_tapes;

	const size_t threads_count = 4;
	const size_t data_block_size = 131072000;

    using DataBlock = std::vector<T>;
    using SorterThreadPool = sort_thread_pool<DataBlockSorter<DataBlock>, DataBlock>;
    using SorterThreadPoolPtr = std::shared_ptr<SorterThreadPool>;
    SorterThreadPoolPtr sorters_pool_ptr;

    using MergerThreadPool = merge_thread_pool<DataBlockSorter<DataBlock>, DataBlock, TapePtr>;
    using MergerThreadPoolPtr = std::shared_ptr<MergerThreadPool>;
    MergerThreadPoolPtr mergers_pool_ptr;

    using DataCollector = TapeIterativeDataCollector<T, DataBlock>;
    using DataCollectorPtr = std::shared_ptr<DataCollector>;
    DataCollectorPtr collector;

    

public:
	TapeSortingEngine(TapePtr input_tape_ptr, TapePtr output_tape_ptr);

	void run();

};

template<typename T>
inline TapeSortingEngine<T>::TapeSortingEngine(TapePtr input_tape_ptr, TapePtr output_tape_ptr)
{
	this->input_tape_ptr = input_tape_ptr;
	this->output_tape_ptr = output_tape_ptr;
    this->sorters_pool_ptr = std::make_shared<SorterThreadPool>(this->threads_count);
    this->mergers_pool_ptr = std::make_shared<MergerThreadPool>(this->threads_count);
    this->collector = std::make_shared<DataCollector>(input_tape_ptr, this->data_block_size);
}

template<typename T>
inline void TapeSortingEngine<T>::run()
{
    constexpr int blocks_total = LENGTH / BLOCK;
    int blocks_processed = 0;
	int blocks_completed = 0;

    //auto start = std::chrono::high_resolution_clock::now();

    while (blocks_completed != blocks_total) {
        if (blocks_processed <= this->threads_count) {
            auto next_data_block = this->collector->collect_next_data();
            if (!next_data_block->empty()) {
                this->sorters_pool_ptr->sort_data_block(*next_data_block);
                blocks_processed++;
            }
        }
        DataBlock sorted_block;
        this->sorters_pool_ptr->get_sorted_data_block(sorted_block);
        if (!sorted_block.empty()) {
            this->mergers_pool_ptr->merge_data_block(sorted_block);
   //         //TapePtr temp_tape = 
   //         if (this->mergers_pool_ptr->get_merged_tape())

			//merge_engine->merge_block(sorted_block);
   //         auto blocks_completed_updated = merge_engine->merged_blocks();
   //         blocks_processed -= blocks_completed_updated - blocks_completed;
   //         blocks_completed = blocks_completed_updated;
        }
    }

    //merge_engine->complete_merge();

    //auto end = std::chrono::high_resolution_clock::now();
    //auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    //std::cout << "\nParallel: " << diff.count() << " us\n";
}
