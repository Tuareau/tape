#pragma once

template <typename T>
class ITape
{
protected:
	virtual ~ITape() = default;

public:
	virtual bool good() = 0;

	ITape & operator=(const ITape &) = delete;
};