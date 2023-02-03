#include "Driver.h"

#include <iostream>

Driver::Driver()
{

}


void Driver::connect()
{
	std::cout << "Connecting...\n";
	std::cout << "Looking for JoyCons with vendor id " << JOYCON::VENDOR << std::endl;

	// Enumerate and print the HID devices on the system
	struct hid_device_info* devs, *cur_dev;

	//int res = hid_init(); // prob not required, no multi-thread
	devs = hid_enumerate(JOYCON::VENDOR, 0x0);
	cur_dev = devs;
	while (cur_dev) {
		switch (cur_dev->product_id) {
		case JOYCON::L_BT:
			std::cout << "Found left Joycon" << std::endl;
			break;
		case JOYCON::R_BT:
			std::cout << "Found right Joycon" << std::endl;
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
	std::cout << "Disconnect!\n";

}


void Driver::update()
{
	std::cout << "." << std::flush;

}
