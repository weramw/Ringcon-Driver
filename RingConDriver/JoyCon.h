#pragma once

#include <string>
#include <stdint.h>

#include <hidapi.h>

class JoyCon
{
public:
	static const int VENDOR_ID = 1406;
	static const int PRODUCT_ID_LEFT = 8198;
	static const int PRODUCT_ID_RIGHT = 8199;

	enum TYPE {
		LEFT, RIGHT,UNKOWN
	};

	JoyCon(hid_device_info* dev_info); //opens handle
	~JoyCon(); // closes handle

	// reads all available data from device
	void update();

protected:
	static const uint8_t _mcu_crc8_table[256];
	static const uint8_t _ringmcu_crc8_table[256];

	uint8_t mcu_crc8_calc(uint8_t* buffer, uint8_t size) const;
	uint8_t ringmcu_crc8_calc(uint8_t* buffer, uint8_t size) const;

protected:
	hid_device* _handle;

	wchar_t* _serial;
	std::string _name;
	TYPE _type;

	bool _ringconAttached;
	int _ringcon;
};

