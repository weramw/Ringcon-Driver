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
	JoyCon *_joycon_left, *_joycon_right;

};

