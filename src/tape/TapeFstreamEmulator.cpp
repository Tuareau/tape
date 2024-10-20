#include "TapeFstreamEmulator.h"

template<typename T>
TapeFstreamEmulator<T>::TapeFstreamEmulator(const std::string & filename)
{
	using namespace std::chrono_literals;
	auto default_settings = std::make_shared<TapeSettings>();
	default_settings->tape_read_delay = 0ms;
	default_settings->tape_write_delay = 0ms;
	default_settings->tape_shift_delay = 0ms;

	this->settings = default_settings;
	this->filename = filename;
	this->state = ITapeEmulator<T>::TapeState::Unitialized;
}

template<typename T>
TapeFstreamEmulator<T>::TapeFstreamEmulator(const TapeSettingsPointer & settings)
{
	this->settings = settings;
	this->filename = std::string{ "default.bin" };
	this->state = ITapeEmulator<T>::TapeState::Unitialized;
}

template<typename T>
TapeFstreamEmulator<T>::TapeFstreamEmulator(const std::string & filename, const TapeSettingsPointer & settings)
{
	this->filename = filename;
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
ITapeEmulator<T>::TapeState TapeFstreamEmulator<T>::open_tape()
{
	constexpr auto open_mode = std::ios_base::binary | std::ios_base::in | std::ios_base::out;
	this->fstream.open(this->filename, open_mode);
	this->update_state();
	this->write_position = this->fstream.tellp();
	this->read_position = this->fstream.tellg();
	return this->state;
}

template<typename T>
ITapeEmulator<T>::TapeState TapeFstreamEmulator<T>::reset_tape()
{
	this->close_tape();
	const auto state = this->open_tape();
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
