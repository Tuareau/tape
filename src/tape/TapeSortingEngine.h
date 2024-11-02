#pragma once

#include <vector>
#include <memory>
#include <thread>

#include "thread_pool.h"
#include "../../src/tape/ITapeEmulator.h"
#include "DataBlockSorter.h"
#include "../../src/tape/TapeEmulatorFabric.h"

#define LENGTH 10000000
#define BLOCK 1000000

#define THREADS 1

template <typename T>
class TapeSortingEngine
{
private:
	using TapePtr = std::shared_ptr<ITapeEmulator<T>>;

	TapePtr input_tape_ptr;
	TapePtr output_tape_ptr;

	using IterableDataBlock = std::vector<T>;
	constexpr size_t threads_count = 4;
	constexpr size_t data_block_size = 131072000;
	thread_pool<DataBlockSorter, IterableDataBlock> sorters_pool;

	struct DataBlocksCounter {
		std::mutex mutex;
		size_t counter;
		size_t threshold;
	};

	DataBlocksCounter data_blocks_counter;

public:
	TapeSortingEngine(TapePtr input_tape_ptr, TapePtr output_tape_ptr);

	void run();

};

template<typename T>
inline TapeSortingEngine<T>::TapeSortingEngine(TapePtr input_tape_ptr, TapePtr output_tape_ptr)
{
	this->input_tape_ptr = input_tape_ptr;
	this->output_tape_ptr = output_tape_ptr;
	this->sorters_pool = thread_pool<DataBlockSorter<IterableDataBlock>, IterableDataBlock>(this->threads_count);
	this->data_blocks_counter.counter = 0;
	this->data_blocks_counter.threshold = this->data_block_size / this->threads_count;
}

template<typename T>
inline void TapeSortingEngine<T>::run()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    std::vector<int> input_tape;
    for (int i = 0; i < LENGTH; i++) {
        input_tape.push_back(std::rand() % 100);
    }

    using data_block = std::vector<int>;

    thread_pool<vector_sorter, data_block> pool(THREADS);

    constexpr int blocks_total = LENGTH / BLOCK;
    int blocks_in = 0;
    int blocks_sorted = 0;

    auto start = std::chrono::high_resolution_clock::now();
    collector collector0;
    while (blocks_sorted != blocks_total) {
        if (!collector0.done/*&& blocks_in < THREADS*/) {
            auto next_block = collector0.collect_next_block(input_tape);
            if (!next_block.empty()) {
                pool.insert_task_data(next_block);
                blocks_in++;
            }
        }
        data_block sorted_block;
        pool.get_processed_data(sorted_block);
        if (!sorted_block.empty()) {
            //std::copy(sorted_block.begin(), sorted_block.end(), std::ostream_iterator<int>(std::cout, ", "));
            //std::cout << std::endl;
            blocks_sorted++;
            blocks_in--;
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "\nParallel: " << diff.count() << " us\n";
}
