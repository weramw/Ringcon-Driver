#pragma once

#include <string>
#include <vector>
#include <array>
#include <stdint.h>
#include <thread>
#include <mutex>

#include <hidapi.h>

#include <Eigen/Core>

#include "IOHelper.h"
#include "Calibration.h"

#include "JoyData.h"

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

	// needs to be called after construction!
	virtual void initialize();

	// call this to start and stop listening to data from device
	void startReceiving();
	void stopReceiving();

	// prints joycon stats
	// return no lines printed
	virtual int printStats() const;

	std::string getName() const;
	std::string getStringForType() const;

	Eigen::Vector2f getStick() const;
	Eigen::Vector3f getAccel() const;
	Eigen::Vector3f getGyro() const;


	Eigen::Vector2i getStickRaw() const;
	Eigen::Vector3i getAccelRaw() const;
	Eigen::Vector3i getGyroRaw() const;

	virtual JoyData getData() const;

	void setPlayerLED(std::array<LIGHT, 4> values);
	void rumble(uint8_t frequency, RUMBLE_INTENSITY intensity);

protected:
	static const int _bluetooth_data_offset;

	// init functions
	void enableVibration();
	void enableIMU();
	void setStandardReportMode();
	void getCalibrationData();
	void setStandardPlayerLEDs();
	void smallRumble();

	// write data to device
	// returns True on success
	bool writeToDevice(const std::vector<uint8_t>& data);

	bool sendCommand(uint8_t command, const std::vector<uint8_t>& data);

	bool sendSubCommand(uint8_t command, uint8_t subcommand, const std::vector<uint8_t>& data);
	
	bool parseData(const std::vector<uint8_t>& data);

	virtual void parseDataWithIMU(const std::vector<uint8_t>& data);

	virtual void parseButtonsState(const std::vector<uint8_t>& data) = 0;
	virtual int getStickDataOffset() const = 0;
	virtual std::string getButtonsStateAsString() const = 0;

	virtual void receiveData();

protected:
	hid_device* _handle;
	Calibration* _calibration;

	wchar_t* _serial;
	std::string _name;
	TYPE _type;

	std::thread *_receive_data_thread;
	std::atomic<bool> _stop_receiving;
	mutable std::mutex _mtx_data;
	uint8_t _timing_byte; // same as timing byte, response order for hid device

	Eigen::Vector2i _stick;
	Eigen::Vector3i _gyro;
	Eigen::Vector3i _accel;

	IOHelper _io_helper;
};

