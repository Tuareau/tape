#pragma once

#include <queue>
#include <mutex>

template <typename T>
class threadsafe_queue
{
private:
	std::queue<T> queue;
	mutable std::mutex mutex;
	std::condition_variable condition;

public:
	threadsafe_queue() = default;

	T wait_and_pop() {
		std::unique_lock<std::mutex> locker(this->mutex);
		this->condition.wait(locker, [this]() { return !queue.empty(); });
		T element = std::move(this->queue.front());
		this->queue.pop();
		return element;
	}

	bool try_pop(T & element) {
		std::lock_guard<std::mutex> guard(this->mutex);
		if (this->queue.empty()) {
			return false;
		}
		element = std::move(this->queue.front());
		this->queue.pop();
		return true;
	}

	void push(T element) {
		std::lock_guard<std::mutex> guard(this->mutex);
		this->queue.push(std::move(element));
		this->condition.notify_one();
	}

	bool empty() const {
		std::lock_guard<std::mutex> guard(this->mutex);
		return this->queue.empty();
	}
};

