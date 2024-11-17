#pragma once

#include <thread>
#include <atomic>
#include <future>
#include <condition_variable>
#include <vector>

#include "threadsafe_queue.h"

template <class CallableMerger, class DataBlock, class ITapeEmulatorPtr>
class merge_thread_pool
{
public:
	explicit merge_thread_pool(size_t thread_count);

	~merge_thread_pool();

	bool merged_tape_ready() const;

	void merge_data_block(DataBlock & data_block);
	bool get_merged_tape(ITapeEmulatorPtr tape_ptr);

private:
	std::atomic<bool> join_enable;

	threadsafe_queue<DataBlock> data_block_queue;
	threadsafe_queue<ITapeEmulatorPtr> merged_tape_queue;

	std::vector<std::thread> threads;

	bool data_block_ready() const;
	void run_thread();
};

template <class CallableMerger, class DataBlock, class ITapeEmulatorPtr>
inline merge_thread_pool<CallableMerger, DataBlock, ITapeEmulatorPtr>::merge_thread_pool(size_t thread_count)
{
	this->join_enable.store(false);
	this->threads.reserve(thread_count);
	for (int i = 0; i < thread_count; i++) {
		this->threads.emplace_back(&merge_thread_pool::run_thread, this);
	}
}

template <class CallableMerger, class DataBlock, class ITapeEmulatorPtr>
inline merge_thread_pool<CallableMerger, DataBlock, ITapeEmulatorPtr>::~merge_thread_pool()
{
	this->join_enable.store(true);
	for (auto & thread : this->threads) {
		thread.join();
	}
}

template <class CallableMerger, class DataBlock, class ITapeEmulatorPtr>
inline bool merge_thread_pool<CallableMerger, DataBlock, ITapeEmulatorPtr>::merged_tape_ready() const
{
	return !this->merged_tape_queue.empty();
}

template <class CallableMerger, class DataBlock, class ITapeEmulatorPtr>
inline void merge_thread_pool<CallableMerger, DataBlock, ITapeEmulatorPtr>::merge_data_block(DataBlock & data_block)
{
	this->data_block_queue.push(std::move(data_block));
}

template <class CallableMerger, class DataBlock, class ITapeEmulatorPtr>
inline bool merge_thread_pool<CallableMerger, DataBlock, ITapeEmulatorPtr>::get_merged_tape(ITapeEmulatorPtr tape_ptr)
{
	return this->merged_tape_queue.try_pop(tape_ptr);
}

template <class CallableMerger, class DataBlock, class ITapeEmulatorPtr>
inline bool merge_thread_pool<CallableMerger, DataBlock, ITapeEmulatorPtr>::data_block_ready() const
{
	return !this->data_block_queue.empty();
}

template <class CallableMerger, class DataBlock, class ITapeEmulatorPtr>
inline void merge_thread_pool<CallableMerger, DataBlock, ITapeEmulatorPtr>::run_thread()
{
	while (!this->join_enable) {
		if (this->data_block_ready()) {
			DataBlock data_block;
			if (this->data_block_queue.try_pop(data_block)) {
				CallableMerger merger;
				auto merged_tape = merger(data_block);
				this->merged_tape_queue.push(merged_tape);
			}
		}
		else {
			std::this_thread::yield();
		}
	}
}
