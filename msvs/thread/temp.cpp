#include <iostream>

#include <algorithm>
#include <vector>

#include <ctime>
#include <cstdlib>

#include <thread>
#include <future>
#include <atomic>

#define LENGTH 1000
#define BLOCK 100
#define SUBBLOCK 25

#define THREADS 4

//struct collector {
//
//    int block = 0;
//    int done = 0;
//
//    std::vector<int> collect_next_block(const std::vector<int> & in) {
//        std::vector<int> out;
//        const auto begin = block * BLOCK;
//        const auto end = begin + BLOCK;
//        if (end > in.size()) {
//            done = 1;
//            return std::vector<int>();
//        }
//        for (int i = begin; i < end; i++) {
//            out.push_back(in.at(i));
//        }
//        return out;
//    }
//};
//
//struct thread_pool {
//    std::vector<std::atomic<bool>> threads_ready_flags;
//
//    thread_pool(const size_t thread_count) {
//        this->threads_ready_flags = std::vector< std::atomic<bool>>(thread_count, std::atomic<bool>(true));
//    }
//
//    bool thread_ready() const {
//        auto found = std::find(this->threads_ready_flags.begin(), this->threads_ready_flags.end(), std::atomic<bool>(true));
//        if (found != this->threads_ready_flags.end()) {
//            return true;
//        }
//        return false;
//    }
//
//    bool try_launch_thread() {
//        auto found = std::find(this->threads_ready_flags.begin(), this->threads_ready_flags.end(), std::atomic<bool>(true));
//        if (found != this->threads_ready_flags.end()) {
//
//
//
//            return true;
//        }
//        return false;
//    }
//
//
//
//};
//
//struct sorter {
//    std::vector<int> sort(const std::vector<int> & in) {
//        std::vector<int> out = in;
//        std::sort(out.begin(), out.end(), [](const auto & lhs, const auto & rhs) {return lhs < rhs; });
//        return out;
//    }
//};