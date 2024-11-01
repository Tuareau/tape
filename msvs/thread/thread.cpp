#include <iostream>
#include <vector>
#include <functional>
#include <chrono>

#include "thread_pool.h"

#define LENGTH 10000000
#define BLOCK 1000000

#define THREADS 1

struct collector {

    int block = 0;
    bool done = false;

    std::vector<int> collect_next_block(const std::vector<int> & in) {
        std::vector<int> out;
        const auto begin = block * BLOCK;
        const auto end = begin + BLOCK;
        if (end > in.size()) {
            done = true;
            return std::vector<int>();
        }
        for (int i = begin; i < end; i++) {
            out.push_back(in.at(i));
        }
        block += 1;
        return out;
    }

};

class vector_sorter {
public:
    std::vector<int> operator()(const std::vector<int> & in) {
        auto out = in;
        std::sort(out.begin(), out.end(), std::less<int>());
        return out;
    }
};


int main()
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

    // ============================================================================================================

    blocks_sorted = 0;

    start = std::chrono::high_resolution_clock::now();
    collector collector1;
    while (blocks_sorted != blocks_total) {
        if (!collector1.done) {
            auto next_block = collector1.collect_next_block(input_tape);
            if (!next_block.empty()) {
                std::sort(next_block.begin(), next_block.end());
                blocks_sorted++;
            }
        }
    }
    end = std::chrono::high_resolution_clock::now();
    diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "\nStandard: " << diff.count() << " us\n";

    return 0;
}
