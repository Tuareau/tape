#pragma once

#include <vector>
#include <array>
#include <memory>
#include <thread>
#include <future>
#include <queue>
#include <filesystem>

#include "threadsafe_queue.h"
#include "ITapeEmulator.h"
#include "TapeEmulatorFabric.h"

template <typename T, typename Container>
class TapeMergeEngine
{
private:
	using TapePtr = std::shared_ptr<ITapeEmulator<T>>;

public:
	TapeMergeEngine() = default;

	TapePtr operator()(TapePtr & tape_ptr, Container & container);

};

template<typename T, typename Container>
inline TapeMergeEngine<T, Container>::TapePtr TapeMergeEngine<T, Container>::operator()(TapePtr & tape_ptr, Container & container)
{
	tape_ptr->reset_tape(std::ios_base::in);

	const auto thread_id = std::hash<std::thread::id>{}(std::this_thread::get_id());
	const auto tmp_dir = std::filesystem::temp_directory_path();
	const auto file_path = tmp_dir / std::filesystem::path("temp_tape_") / std::filesystem::path(std::to_string(thread_id));
	auto temp_tape = TapeEmulatorFabric<T>::CreateEmulator(file_path);
	temp_tape->reset_tape(std::ios_base::out);

	for (const auto & container_element : container) {

		auto element_stored_flag = false;

		while (tape_ptr->good()) {
			T tape_element;
			auto state = tape_ptr->read_element(tape_element);
			if (tape_ptr->good()) {
				if (tape_element <= container_element) {
					temp_tape->write_element(tape_element);
					temp_tape->shift_forward();
					continue;
				}
				else {
					temp_tape->write_element(container_element);
					temp_tape->shift_forward();
					element_stored_flag = true;
					break;
				}
			}
		}

		if (!element_stored_flag) {
			temp_tape->write_element(container_element);
			temp_tape->shift_forward();
		}
	}

	const std::filesystem::path old_tape_path = tape_ptr->tape_path();
	std::filesystem::remove(old_tape_path);
	std::filesystem::rename(file_path, old_tape_path);
	temp_tape->copy_tape(*tape_ptr);
	return temp_tape;
}
