#pragma once

#include <thread>
#include <atomic>
#include <future>
#include <condition_variable>
#include <vector>

#include "threadsafe_queue.h"

template <class CallableSorter, class DataBlock>
class sort_thread_pool
{
public:
	explicit sort_thread_pool(size_t thread_count);

	~sort_thread_pool();

	bool sorted_data_block_ready() const;

	void sort_data_block(DataBlock & data_block);
	void get_sorted_data_block(DataBlock & data_block);

private:
	std::atomic<bool> join_enable;

	threadsafe_queue<DataBlock> data_block_queue;
	threadsafe_queue<DataBlock> sorted_data_block_queue;

	std::vector<std::thread> threads;

	bool data_block_ready() const;
	void run_thread();
};

template<class CallableSorter, class DataBlock>
inline sort_thread_pool<CallableSorter, DataBlock>::sort_thread_pool(size_t thread_count)
{
	this->join_enable.store(false);
	this->threads.reserve(thread_count);
	for (int i = 0; i < thread_count; i++) {
		this->threads.emplace_back(&sort_thread_pool::run_thread, this);
	}
}

template<class CallableSorter, class DataBlock>
inline sort_thread_pool<CallableSorter, DataBlock>::~sort_thread_pool()
{
	this->join_enable.store(true);
	for (auto & thread : this->threads) {
		thread.join();
	}
}

template<class CallableSorter, class DataBlock>
inline bool sort_thread_pool<CallableSorter, DataBlock>::sorted_data_block_ready() const
{
	return !this->sorted_data_block_queue.empty();
}

template<class CallableSorter, class DataBlock>
inline void sort_thread_pool<CallableSorter, DataBlock>::sort_data_block(DataBlock & data_block)
{
	this->data_block_queue.push(std::move(data_block));
}

template<class CallableSorter, class DataBlock>
inline void sort_thread_pool<CallableSorter, DataBlock>::get_sorted_data_block(DataBlock & data_block)
{
	this->sorted_data_block_queue.try_pop(data_block);
}

template<class CallableSorter, class DataBlock>
inline bool sort_thread_pool<CallableSorter, DataBlock>::data_block_ready() const
{
	return !this->data_block_queue.empty();
}

template<class CallableSorter, class DataBlock>
inline void sort_thread_pool<CallableSorter, DataBlock>::run_thread()
{
	while (!this->join_enable) {
		if (this->data_block_ready()) {
			DataBlock data_block;
			if (this->data_block_queue.try_pop(data_block)) {
				CallableSorter sorter;
				auto processed_data_block = sorter(data_block);
				this->sorted_data_block_queue.push(processed_data_block);
			}
		}
		else {
			std::this_thread::yield();
		}
	}
}
