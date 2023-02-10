#pragma once

#include <hidapi.h>

#include "JoyCon.h"


class Driver
{
public:
	Driver();

	bool is_connected() const;


	void connect();
	void disconnect();
	void update();

protected:
	int _lines_printed;
	JoyCon *_joycon_left, *_joycon_right;

};

