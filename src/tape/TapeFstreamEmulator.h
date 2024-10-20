#pragma once

#include "ITapeEmulator.h"

#include <fstream>
#include <string>
#include <memory>
#include <chrono>

struct TapeSettings {
	std::chrono::milliseconds tape_write_delay;
	std::chrono::milliseconds tape_read_delay;
	std::chrono::milliseconds tape_shift_delay;
};

using TapeSettingsPointer = std::shared_ptr<TapeSettings>;

template <typename T>
class TapeFstreamEmulator : private ITapeEmulator<T>
{
private:
	TapeSettingsPointer settings;

	std::string filename;
	std::fstream fstream;

	std::fstream::pos_type position;

	ITapeEmulator<T>::TapeState state;

	ITapeEmulator<T>::TapeState update_state();

protected:
	~TapeFstreamEmulator();

public:
	TapeFstreamEmulator() = delete;
	TapeFstreamEmulator(const TapeFstreamEmulator &) = default;
	TapeFstreamEmulator(TapeFstreamEmulator &&) = default;
	TapeFstreamEmulator & operator=(const TapeFstreamEmulator &) = default;

	explicit TapeFstreamEmulator(std::string filename);
	explicit TapeFstreamEmulator(TapeSettingsPointer settings);
	TapeFstreamEmulator(std::string filename, TapeSettingsPointer settings);

	bool good() override;

	ITapeEmulator<T>::TapeState open_stream();
	ITapeEmulator<T>::TapeState reset_stream();
	ITapeEmulator<T>::TapeState close_stream();

	ITapeEmulator<T>::TapeState read_element(T &) override;
	ITapeEmulator<T>::TapeState write_element(const T &) override;
	ITapeEmulator<T>::TapeState shift_forward() override;
	ITapeEmulator<T>::TapeState shift_backward() override;
};