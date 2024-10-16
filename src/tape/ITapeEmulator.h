#pragma once

#include "ITape.h"

class ITapeEmulator : public ITape
{
protected:
	virtual ~ITapeEmulator() = default;

public:
	virtual void destroy() = 0;
	ITapeEmulator & operator=(const ITapeEmulator &) = delete;
};