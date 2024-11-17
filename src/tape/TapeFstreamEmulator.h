#pragma once

#include "ITapeEmulator.h"

#include <fstream>
#include <string>
#include <memory>
#include <chrono>
#include <filesystem>

struct TapeSettings
{
	std::string setting_id;
	std::chrono::milliseconds tape_write_delay;
	std::chrono::milliseconds tape_read_delay;
	std::chrono::milliseconds tape_shift_delay;
};

using TapeSettingsPointer = std::shared_ptr<TapeSettings>;

template <typename T>
class TapeFstreamEmulator : public ITapeEmulator<T>
{
private:
	TapeSettingsPointer settings;

	std::filesystem::path filepath;
	std::fstream fstream;

	std::fstream::pos_type read_position;
	std::fstream::pos_type write_position;

	ITapeEmulator<T>::TapeState state;

	ITapeEmulator<T>::TapeState update_state();

public:
	~TapeFstreamEmulator();
	TapeFstreamEmulator() = delete;
	TapeFstreamEmulator(const TapeFstreamEmulator &) = default;
	TapeFstreamEmulator(TapeFstreamEmulator &&) = default;
	TapeFstreamEmulator & operator=(const TapeFstreamEmulator &) = default;

	explicit TapeFstreamEmulator(const std::filesystem::path & filepath);
	explicit TapeFstreamEmulator(const TapeSettingsPointer & settings);
	TapeFstreamEmulator(const std::filesystem::path & filepath, const TapeSettingsPointer & settings);

	bool good() override;

	ITapeEmulator<T>::TapeState open_tape(std::ios_base::openmode mode = std::ios_base::out | std::ios_base::in) override;
	ITapeEmulator<T>::TapeState reset_tape(std::ios_base::openmode mode = std::ios_base::out | std::ios_base::in) override;
	ITapeEmulator<T>::TapeState close_tape() override;

	ITapeEmulator<T>::TapeState read_element(T &) override;
	ITapeEmulator<T>::TapeState write_element(const T &) override;
	ITapeEmulator<T>::TapeState shift_forward() override;
	ITapeEmulator<T>::TapeState shift_backward() override;

	ITapeEmulator<T>::TapeState copy_tape(ITapeEmulator<T> & swap_tape) override;

	std::string tape_path() override;
};

template<typename T>
TapeFstreamEmulator<T>::TapeFstreamEmulator(const std::filesystem::path & filepath)
{
	using namespace std::chrono_literals;
	auto default_settings = std::make_shared<TapeSettings>();
	default_settings->setting_id = std::string{};
	default_settings->tape_read_delay = 0ms;
	default_settings->tape_write_delay = 0ms;
	default_settings->tape_shift_delay = 0ms;

	this->settings = default_settings;
	this->filepath = filepath;
	this->state = ITapeEmulator<T>::TapeState::Unitialized;
}

template<typename T>
TapeFstreamEmulator<T>::TapeFstreamEmulator(const TapeSettingsPointer & settings)
{
	this->settings = settings;
	this->filepath = std::filesystem::path(std::string{ "./default.bin" });
	this->state = ITapeEmulator<T>::TapeState::Unitialized;
}

template<typename T>
TapeFstreamEmulator<T>::TapeFstreamEmulator(const std::filesystem::path & filepath, const TapeSettingsPointer & settings)
{
	this->filepath = filepath;
	this->settings = settings;
	this->state = ITapeEmulator<T>::TapeState::Unitialized;
}

template<typename T>
ITapeEmulator<T>::TapeState TapeFstreamEmulator<T>::update_state()
{
	if (this->fstream.good()) {
		this->state = ITapeEmulator<T>::TapeState::Good;
	}
	else if (this->fstream.eof()) {
		this->state = ITapeEmulator<T>::TapeState::EndOfFile;
	}
	else {
		this->state = ITapeEmulator<T>::TapeState::Unitialized;
	}
	return this->state;
}

template<typename T>
TapeFstreamEmulator<T>::~TapeFstreamEmulator()
{
	this->fstream.close();
	this->state = ITapeEmulator<T>::TapeState::Unitialized;
}

template<typename T>
bool TapeFstreamEmulator<T>::good()
{
	return this->state == ITapeEmulator<T>::TapeState::Good;
}

template<typename T>
ITapeEmulator<T>::TapeState TapeFstreamEmulator<T>::open_tape(std::ios_base::openmode mode)
{
	auto open_mode = std::ios_base::binary | mode;
	this->fstream.open(this->filepath, open_mode);
	this->update_state();
	if (this->state == ITapeEmulator<T>::TapeState::Unitialized) {
		open_mode = std::ios_base::binary | std::ios_base::out;
		this->fstream.open(this->filepath, open_mode);
		this->update_state();
	}
	this->write_position = this->fstream.tellp();
	this->read_position = this->fstream.tellg();
	return this->state;
}

template<typename T>
ITapeEmulator<T>::TapeState TapeFstreamEmulator<T>::reset_tape(std::ios_base::openmode mode)
{
	this->close_tape();
	const auto state = this->open_tape(mode);
	return state;
}

template<typename T>
ITapeEmulator<T>::TapeState TapeFstreamEmulator<T>::close_tape()
{
	this->fstream.close();
	this->state = ITapeEmulator<T>::TapeState::Unitialized;
	return this->state;
}

template<typename T>
ITapeEmulator<T>::TapeState TapeFstreamEmulator<T>::read_element(T & element)
{
	if (this->state != ITapeEmulator<T>::TapeState::Good) {
		return this->state;
	}

	std::this_thread::sleep_for(this->settings->tape_read_delay);

	T value = 0;
	this->fstream.seekg(this->read_position, std::ios_base::beg);
	if (this->fstream.read(reinterpret_cast<char *>(&value), sizeof(T))) {
		this->update_state();
	}
	else {
		this->state = ITapeEmulator<T>::TapeState::ReadError;
	}

	element = value;
	return this->state;
}

template<typename T>
ITapeEmulator<T>::TapeState TapeFstreamEmulator<T>::write_element(const T & element)
{
	if (this->state != ITapeEmulator<T>::TapeState::Good) {
		return this->state;
	}

	std::this_thread::sleep_for(this->settings->tape_write_delay);

	T value = element;
	this->fstream.seekp(this->write_position, std::ios_base::beg);
	this->fstream.write(reinterpret_cast<char *>(&value), sizeof(T));
	this->update_state();
	return this->state;
}

template<typename T>
ITapeEmulator<T>::TapeState TapeFstreamEmulator<T>::shift_forward()
{
	if (this->state != ITapeEmulator<T>::TapeState::Good) {
		return this->state;
	}

	std::this_thread::sleep_for(this->settings->tape_shift_delay);

	this->read_position += sizeof(T);
	this->write_position += sizeof(T);
	return this->state;
}

template<typename T>
ITapeEmulator<T>::TapeState TapeFstreamEmulator<T>::shift_backward()
{
	if (this->state != ITapeEmulator<T>::TapeState::Good) {
		return this->state;
	}

	std::this_thread::sleep_for(this->settings->tape_shift_delay);

	this->read_position -= sizeof(T);
	this->write_position -= sizeof(T);
	return this->state;
}

template<typename T>
inline ITapeEmulator<T>::TapeState TapeFstreamEmulator<T>::copy_tape(ITapeEmulator<T> & swap_tape)
{
	swap_tape.close_tape();
	this->filepath = swap_tape.filepath;
	this->read_position = swap_tape.read_position;
	this->write_position = swap_tape.write_position;
	this->settings = swap_tape.settings;
	this->state = swap_tape.state;
	this->reset_tape();
	return this->state;
}

template<typename T>
inline std::string TapeFstreamEmulator<T>::tape_path()
{
	return this->filepath;
}
