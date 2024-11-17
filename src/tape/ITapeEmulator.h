#pragma once

#include "ITape.h"

#include <memory>

template <typename T>
class ITapeEmulator : public ITape<T>
{
protected:
	virtual ~ITapeEmulator() = default;

public:
	ITapeEmulator & operator=(const ITapeEmulator &) = delete;

	virtual bool good() = 0;

	enum class TapeState {
		Unitialized,
		Good,
		ReadError,
		EndOfFile,
	};

	virtual TapeState open_tape(std::ios_base::openmode mode = std::ios_base::out | std::ios_base::in) = 0;
	virtual TapeState reset_tape(std::ios_base::openmode mode = std::ios_base::out | std::ios_base::in) = 0;
	virtual TapeState close_tape() = 0;

	virtual TapeState read_element(T &) = 0;
	virtual TapeState write_element(const T &) = 0;
	virtual TapeState shift_forward() = 0;
	virtual TapeState shift_backward() = 0;

	virtual TapeState copy_tape(ITapeEmulator & swap_tape) = 0;

	virtual std::string tape_path() = 0;
};