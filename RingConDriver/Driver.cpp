#include "Driver.h"

#include <iostream>

#include "JoyConLeft.h"
#include "JoyConRight.h"
#include "RingCon.h"

Driver::Driver() :
	_lines_printed(0),
	_joycon_left(nullptr),
	_joycon_right(nullptr),
	_vjoy_left(nullptr),
	_vjoy_right(nullptr)
{

}

bool Driver::is_connected() const
{
	return is_connected_joy_cons() && is_connected_v_joy_controller();
}

void Driver::connect()
{
	std::cout << "Connecting..." << std::endl;
	connectJoyCons();
	connectVJoyController();
	std::cout << "Connected!" << std::endl;
}


void Driver::disconnect()
{
	std::cout << "Disconnecting..." << std::endl;
	disconnectJoyCons();
	disconnectVJoyController();
	std::cout << "Disconnected!" << std::endl;
}


void Driver::update()
{
	if (_joycon_right != nullptr) {
		_joycon_right->update();
	}
	if (_joycon_left != nullptr) {
		_joycon_left->update();
	}

	//std::cout << "." << std::flush;
	std::cout << "\x1b[" << _lines_printed << "A"; // move up n lines
	std::cout << "\x1b[" << _lines_printed << "M"; // delete last n lines
	
	_lines_printed = 0;
	if (_joycon_left != nullptr) {
		_lines_printed += _joycon_left->printStats();
	}
	if (_joycon_right != nullptr) {
		_lines_printed += _joycon_right->printStats();
	}

	// send joyCon data
	if (_vjoy_left != nullptr) {
	    _lines_printed += _vjoy_left->sendData(makeData(_joycon_left));
	}
	if (_vjoy_right != nullptr) {
		_lines_printed += _vjoy_right->sendData(makeData(_joycon_right));
	}
}

JoyData Driver::makeData(JoyCon* joycon) const
{
	JoyData data;
	if (joycon == nullptr) {
		return data;
	}

	data = joycon->getData();
	return data;
}

bool Driver::is_connected_joy_cons() const
{
	return (_joycon_left != nullptr) && (_joycon_right != nullptr);
	//return (_joycon_left != nullptr);
}

bool Driver::is_connected_v_joy_controller() const
{
	return (_vjoy_left != nullptr) && (_vjoy_right != nullptr);
}

void Driver::connectJoyCons()
{
	std::cout << "Looking for JoyCons with vendor id " << JoyCon::VENDOR_ID << std::endl;

	// Enumerate and print the HID devices on the system
	struct hid_device_info* devs, * cur_dev;

	//int res = hid_init(); // prob not required, no multi-thread
	devs = hid_enumerate(JoyCon::VENDOR_ID, 0x0);
	cur_dev = devs;
	while (cur_dev) {
		switch (cur_dev->product_id) {
		case JoyCon::PRODUCT_ID_LEFT:
			if (_joycon_left == nullptr) {
				std::cout << "Found left Joycon" << std::endl;
				_joycon_left = new JoyConLeft(cur_dev);
				_joycon_left->initialize();
			}
			break;
		case JoyCon::PRODUCT_ID_RIGHT:
			if (_joycon_right == nullptr) {
				std::cout << "Found right Joycon" << std::endl;
				//_joycon_right = new JoyConRight(cur_dev);
				//_joycon_right->initialize();
				_joycon_right = new RingCon(cur_dev);
				_joycon_right->initialize();
			}
			break;
		default:
			std::cout << "Found other controller with product_id: " << cur_dev->product_id << std::endl;
		}
		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);
}

void Driver::connectVJoyController()
{
	UINT device_id_left = 1;
	UINT device_id_right = 2;

	if (_vjoy_left == nullptr) {
		_vjoy_left = new VJoyController(device_id_left);
		if (!_vjoy_left->initialize()) {
			delete _vjoy_left;
		}
	}
	if (_vjoy_right == nullptr) {
		_vjoy_right = new VJoyController(device_id_right);
		if (!_vjoy_right->initialize()) {
			delete _vjoy_right;
		}
	}
}

void Driver::disconnectJoyCons()
{
	if (_joycon_left != nullptr) {
		delete _joycon_left;
		std::cout << "closed left JoyCon..." << std::flush;
	}
	else {
		std::cout << "No left JoyCon connected...";
	}
	if (_joycon_right != nullptr) {
		delete _joycon_right;
		std::cout << "closed right JoyCon..." << std::flush;
	}
	else {
		std::cout << "No right JoyCon connected...";
	}
}

void Driver::disconnectVJoyController()
{
	if (_vjoy_left != nullptr) {
		delete _vjoy_left;
		std::cout << "closed left vJoyController..." << std::flush;
	}
	else {
		std::cout << "No left vJoyController connected...";
	}
	if (_vjoy_right != nullptr) {
		delete _vjoy_right;
		std::cout << "closed right vJoyController..." << std::flush;
	}
	else {
		std::cout << "No right vJoyController connected...";
	}
}

