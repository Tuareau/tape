#pragma once

#include <memory>

#include "../../src/tape/ITapeEmulator.h"

template <typename T, class IterableContainer>
class TapeIterativeDataCollector
{
private:
	using TapePtr = std::shared_ptr<ITapeEmulator<T>>;
	TapePtr tape_ptr;

	size_t data_block_size;

	enum class collector_status {
		uninitialized,
		operating,
		done,
	};

	collector_status status;

public:
	TapeIterativeDataCollector(TapePtr tape_ptr, size_t data_block_size);

	using IterableContainerPtr = std::shared_ptr<IterableContainer>;
	IterableContainerPtr collect_next_data();

};

template<typename T, class IterableContainer>
inline TapeIterativeDataCollector<T, IterableContainer>::TapeIterativeDataCollector(TapePtr tape_ptr, size_t data_block_size)
{
	this->tape_ptr = tape_ptr;
	this->data_block_size = data_block_size;
	this->status = collector_status::uninitialized;
}

template<typename T, class IterableContainer>
inline TapeIterativeDataCollector<T, IterableContainer>::IterableContainerPtr TapeIterativeDataCollector<T, IterableContainer>::collect_next_data()
{
	if (this->status == collector_status::uninitialized) {
		auto tape_status = this->tape_ptr->open_tape();
		this->status = collector_status::operating;
	}

	IterableContainerPtr container_ptr = std::make_shared<IterableContainer>();

	if (this->status != collector_status::done) {
		for (int i = 0; i < this->data_block_size; i++) {
			if (this->tape_ptr->good()) {
				T element;
				auto tape_state = this->tape_ptr->read_element(element);
				if (this->tape_ptr->good()) {
					container_ptr->push_back(element);
				}
				this->tape_ptr->shift_forward();
			}
			else {
				this->status = collector_status::done;
				this->tape_ptr->close_tape();
			}
		}
	}

	return container_ptr;
}
