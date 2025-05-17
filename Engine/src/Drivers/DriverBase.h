#pragma once

class DriverBase
{
public:
	virtual	~DriverBase() {}

	virtual void Initialize() = 0;
};