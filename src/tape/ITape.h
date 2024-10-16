#pragma once

class ITape
{
protected:
	virtual ~ITape() = default;

public:
	virtual void destroy() = 0;
	ITape & operator=(const ITape &) = delete;
};