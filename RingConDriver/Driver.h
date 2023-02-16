#pragma once

#include <hidapi.h>

#include "JoyCon.h"
#include "VJoyController.h"


class Driver
{
public:
	Driver();

	bool is_connected() const;


	void connect();
	void disconnect();
	void update();

protected:

	JoyData makeData(JoyCon* joycon) const;

	bool is_connected_joy_cons() const;
	bool is_connected_v_joy_controller() const;

	void connectJoyCons();
	void connectVJoyController();

	void disconnectJoyCons();
	void disconnectVJoyController();

protected:
	int _lines_printed;
	JoyCon *_joycon_left, *_joycon_right;
	VJoyController *_vjoy_left, * _vjoy_right;

};

