#pragma once

#include <hidapi.h>

class Driver
{
public:
	Driver();

	void connect();
	void disconnect();
	void update();

protected:

};

