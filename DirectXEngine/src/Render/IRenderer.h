#pragma once


class IRenderer
{
public:
	virtual bool init() = 0;
	virtual void begin() = 0;
	virtual void end() = 0;
};