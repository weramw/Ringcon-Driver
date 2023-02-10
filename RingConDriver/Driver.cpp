#include "Driver.h"

#include <iostream>

#include "JoyConLeft.h"
#include "JoyConRight.h"

Driver::Driver() :
	_lines_printed(0),
	_joycon_left(nullptr),
	_joycon_right(nullptr)
{

}

bool Driver::is_connected() const
{
	return (_joycon_left != nullptr) && (_joycon_right != nullptr);
	//return (_joycon_left != nullptr);
}

void Driver::connect()
{
	std::cout << "Connecting...\n";
	std::cout << "Looking for JoyCons with vendor id " << JoyCon::VENDOR_ID << std::endl;

	// Enumerate and print the HID devices on the system
	struct hid_device_info* devs, *cur_dev;

	//int res = hid_init(); // prob not required, no multi-thread
	devs = hid_enumerate(JoyCon::VENDOR_ID, 0x0);
	cur_dev = devs;
	while (cur_dev) {
		switch (cur_dev->product_id) {
		case JoyCon::PRODUCT_ID_LEFT:
			if (_joycon_left == nullptr) {
				std::cout << "Found left Joycon" << std::endl;
				_joycon_left = new JoyConLeft(cur_dev);
			}
			break;
		case JoyCon::PRODUCT_ID_RIGHT:
			if (_joycon_right == nullptr) {
				std::cout << "Found right Joycon" << std::endl;
				_joycon_right = new JoyConRight(cur_dev);
			}
			break;
		default:
			std::cout << "Found other controller with product_id: " << cur_dev->product_id << std::endl;
		}
		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);
}


void Driver::disconnect()
{
	std::cout << "Disconnecting..." << std::endl;
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
}
