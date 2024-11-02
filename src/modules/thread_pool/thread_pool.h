#pragma once

#include <thread>
#include <atomic>
#include <future>
#include <condition_variable>
#include <vector>

#include "threadsafe_queue.h"

template <class Callable, class Data>
class thread_pool
{
public:
	explicit thread_pool(size_t thread_count);

	~thread_pool();

	bool processed_data_ready() const;

	void insert_task_data(Data & data);
	void get_processed_data(Data & data);

private:
	std::atomic<bool> join_enable;

	threadsafe_queue<Data> task_data_queue;
	threadsafe_queue<Data> processed_data_queue;

	std::vector<std::thread> threads;

	bool data_ready() const;
	void run_thread();
};

template<class Callable, class Data>
inline thread_pool<Callable, Data>::thread_pool(size_t thread_count)
{
	this->join_enable.store(false);
	this->threads.reserve(thread_count);
	for (int i = 0; i < thread_count; i++) {
		this->threads.emplace_back(&thread_pool::run_thread, this);
	}
}

template<class Callable, class Data>
inline thread_pool<Callable, Data>::~thread_pool()
{
	this->join_enable.store(true);
	for (auto & thread : this->threads) {
		thread.join();
	}
}

template<class Callable, class Data>
inline bool thread_pool<Callable, Data>::processed_data_ready() const
{
	return !this->processed_data_queue.empty();
}

template<class Callable, class Data>
inline void thread_pool<Callable, Data>::insert_task_data(Data & data)
{
	this->task_data_queue.push(std::move(data));
}

template<class Callable, class Data>
inline void thread_pool<Callable, Data>::get_processed_data(Data & data)
{
	this->processed_data_queue.try_pop(data);
}

template<class Callable, class Data>
inline bool thread_pool<Callable, Data>::data_ready() const
{
	return !this->task_data_queue.empty();
}

template<class Callable, class Data>
inline void thread_pool<Callable, Data>::run_thread()
{
	while (!this->join_enable) {
		if (this->data_ready()) {
			Data data;
			if (this->task_data_queue.try_pop(data)) {
				Callable call;
				auto processed_data = call(data);
				this->processed_data_queue.push(processed_data);
			}
		}
	}
}
