#pragma once

class DriverBase
{
public:
	virtual	~DriverBase() {}

	virtual bool Initialize() = 0;
};
