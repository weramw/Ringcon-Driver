#pragma once

#include <string>
#include <vector>
#include <array>
#include <stdint.h>

#include <hidapi.h>

#include <Eigen/Core>

#include "Calibration.h"

class JoyCon
{
public:
	static const int VENDOR_ID = 1406;
	static const int PRODUCT_ID_LEFT = 8198;
	static const int PRODUCT_ID_RIGHT = 8199;

	static int16_t uint16_to_int16(uint16_t a);

	enum TYPE {
		LEFT, RIGHT,UNKOWN
	};

	enum LIGHT {
		LIGHT_OFF=0x0,
		LIGHT_FLASH=0x10,
		LIGHT_SOLID=0x1
	};

	enum RUMBLE_INTENSITY {
		RUMBLE_HIGH = 1,
		RUMBLE_LOW = 2,
		RUMBLE_OFF = 3
	};

	JoyCon(hid_device_info* dev_info); //opens handle
	~JoyCon(); // closes handle

	// reads all available data from device
	void update(bool verbose=true);

	std::string getName() const;
	std::string getStringForType() const;

	void setPlayerLED(std::array<LIGHT, 4> values);
	void rumble(uint8_t frequency, RUMBLE_INTENSITY intensity);

protected:
	static const int _bluetooth_data_offset;
	static const uint8_t _mcu_crc8_table[256];
	static const uint8_t _ringmcu_crc8_table[256];

	uint8_t mcu_crc8_calc(uint8_t* buffer, uint8_t size) const;
	uint8_t ringmcu_crc8_calc(uint8_t* buffer, uint8_t size) const;

	// write data to device
	// returns True on success

	bool writeToDevice(const std::vector<uint8_t>& data);

	bool sendCommand(uint8_t command, const std::vector<uint8_t>& data);

	bool sendSubCommand(uint8_t command, uint8_t subcommand, const std::vector<uint8_t>& data);
	
	void parseData(const std::vector<uint8_t>& data);

	void parseDataWithIMU(const std::vector<uint8_t>& data);

	virtual void parseButtonsState(const std::vector<uint8_t>& data) = 0;
	virtual int getStickDataOffset() const = 0;
	virtual std::string getButtonsStateAsString() const = 0;

	void initialize();

protected:
	hid_device* _handle;
	Calibration* _calibration;

	wchar_t* _serial;
	std::string _name;
	TYPE _type;

	bool _ringconAttached;
	int _ringcon;

	int _read_timeout;

	uint8_t _global_count; // TODO: check what this does...

	uint16_t _stick_x;
	uint16_t _stick_y;
	//uint8_t _battery;

	Eigen::Vector3f _gyro;
	Eigen::Vector3f _accel;

};

