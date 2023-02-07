#include "JoyCon.h"

#include <iostream>
//#include <algorithm>

#define NOMINMAX // prevent windows interfering with c++ min max
#include <Windows.h>

const int JoyCon::_bluetooth_data_offset = 0; // = 10 else

const uint8_t JoyCon::_mcu_crc8_table[256] = {
0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65, 0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2, 0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42, 0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C, 0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B, 0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};

const uint8_t JoyCon::_ringmcu_crc8_table[256] = {
0x00, 0x8D, 0x97, 0x1A, 0xA3, 0x2E, 0x34, 0xB9, 0xCB, 0x46, 0x5C, 0xD1, 0x68, 0xE5, 0xFF, 0x72,
0x1B, 0x96, 0x8C, 0x01, 0xB8, 0x35, 0x2F, 0xA2, 0xD0, 0x5D, 0x47, 0xCA, 0x73, 0xFE, 0xE4, 0x69,
0x36, 0xBB, 0xA1, 0x2C, 0x95, 0x18, 0x02, 0x8F, 0xFD, 0x70, 0x6A, 0xE7, 0x5E, 0xD3, 0xC9, 0x44,
0x2D, 0xA0, 0xBA, 0x37, 0x8E, 0x03, 0x19, 0x94, 0xE6, 0x6B, 0x71, 0xFC, 0x45, 0xC8, 0xD2, 0x5F,
0x6C, 0xE1, 0xFB, 0x76, 0xCF, 0x42, 0x58, 0xD5, 0xA7, 0x2A, 0x30, 0xBD, 0x04, 0x89, 0x93, 0x1E,
0x77, 0xFA, 0xE0, 0x6D, 0xD4, 0x59, 0x43, 0xCE, 0xBC, 0x31, 0x2B, 0xA6, 0x1F, 0x92, 0x88, 0x05,
0x5A, 0xD7, 0xCD, 0x40, 0xF9, 0x74, 0x6E, 0xE3, 0x91, 0x1C, 0x06, 0x8B, 0x32, 0xBF, 0xA5, 0x28,
0x41, 0xCC, 0xD6, 0x5B, 0xE2, 0x6F, 0x75, 0xF8, 0x8A, 0x07, 0x1D, 0x90, 0x29, 0xA4, 0xBE, 0x33,
0xD8, 0x55, 0x4F, 0xC2, 0x7B, 0xF6, 0xEC, 0x61, 0x13, 0x9E, 0x84, 0x09, 0xB0, 0x3D, 0x27, 0xAA,
0xC3, 0x4E, 0x54, 0xD9, 0x60, 0xED, 0xF7, 0x7A, 0x08, 0x85, 0x9F, 0x12, 0xAB, 0x26, 0x3C, 0xB1,
0xEE, 0x63, 0x79, 0xF4, 0x4D, 0xC0, 0xDA, 0x57, 0x25, 0xA8, 0xB2, 0x3F, 0x86, 0x0B, 0x11, 0x9C,
0xF5, 0x78, 0x62, 0xEF, 0x56, 0xDB, 0xC1, 0x4C, 0x3E, 0xB3, 0xA9, 0x24, 0x9D, 0x10, 0x0A, 0x87,
0xB4, 0x39, 0x23, 0xAE, 0x17, 0x9A, 0x80, 0x0D, 0x7F, 0xF2, 0xE8, 0x65, 0xDC, 0x51, 0x4B, 0xC6,
0xAF, 0x22, 0x38, 0xB5, 0x0C, 0x81, 0x9B, 0x16, 0x64, 0xE9, 0xF3, 0x7E, 0xC7, 0x4A, 0x50, 0xDD,
0x82, 0x0F, 0x15, 0x98, 0x21, 0xAC, 0xB6, 0x3B, 0x49, 0xC4, 0xDE, 0x53, 0xEA, 0x67, 0x7D, 0xF0,
0x99, 0x14, 0x0E, 0x83, 0x3A, 0xB7, 0xAD, 0x20, 0x52, 0xDF, 0xC5, 0x48, 0xF1, 0x7C, 0x66, 0xEB
};

int16_t JoyCon::uint16_to_int16(uint16_t a) {
	int16_t b;
	char* aPointer = (char*)&a, * bPointer = (char*)&b;
	memcpy(bPointer, aPointer, sizeof(a));
	return b;
}

uint8_t JoyCon::mcu_crc8_calc(uint8_t* buffer, uint8_t size) const { //CTCAER
	uint8_t crc8 = 0x0;

	for (int i = 0; i < size; ++i) {
		crc8 = _mcu_crc8_table[(uint8_t)(crc8 ^ buffer[i])];
	}
	return crc8;
}

uint8_t JoyCon::ringmcu_crc8_calc(uint8_t* buffer, uint8_t size) const {
	uint8_t crc8 = 0x0;

	for (int i = 0; i < size; ++i) {
		crc8 = _ringmcu_crc8_table[(uint8_t)(crc8 ^ buffer[i])];
	}
	return crc8;
}

JoyCon::JoyCon(hid_device_info* dev_info) :
	_handle(nullptr),
	_serial(nullptr),
	_name("UNKOWN"),
	_type(UNKOWN),
	_ringconAttached(false),
	_ringcon(0x0A), //Ringcon data. Packet[40]. Fully pulled = 0x00, rest = 0x0a, fully pushed = 0x14.
	_read_timeout(3),
	_global_count(0), // TODO: ???
	_accel(Eigen::Vector3f::Zero()),
	_gyro(Eigen::Vector3f::Zero())
{	
	_serial = _wcsdup(dev_info->serial_number);

	if (dev_info->product_id == PRODUCT_ID_LEFT) {
		_name = "Joy-Con (L)";
		_type = LEFT;
		printf("Found %s: %ls %s\n", _name.c_str(), _serial, dev_info->path);
	}
	else if (dev_info->product_id == PRODUCT_ID_RIGHT) {
		_name = "Joy-Con (R)";
		_type = RIGHT;
		printf("Found %s: %ls %s\n", _name.c_str(), _serial, dev_info->path);
	}

	_handle = hid_open_path(dev_info->path);
	if (_handle == nullptr) {
		printf("Could not open %s with serial %ls\n", _name.c_str(), _serial); // , strerror_s(errno));
		throw;
	}

	printf("Successfully opened device: %s\n", _name.c_str());

	// init device
	initialize();
}

JoyCon::~JoyCon()
{
	if (_handle != nullptr) {
		hid_close(_handle);
		printf("Closed device: %s\n", _name.c_str());
	}
}

std::string JoyCon::getName() const
{
	return _name;
}

std::string JoyCon::getStringForType() const
{
	std::string str_type = "UNKOWN";
	if (_type == LEFT) {
		str_type = "LEFT";
	}
	else if (_type == RIGHT) {
		str_type = "RIGHT";
	}
	return str_type;
}

void JoyCon::initialize()
{
	std::vector<uint8_t> buffer = { 1 }; // Enabled

	// Enable vibration
	std::cout << "Enabling Vibration..." << std::endl;
	sendSubCommand(0x1, 0x48, buffer);

	// Enable IMU data
	std::cout << "Enabling IMU data..." << std::endl;
	sendSubCommand(0x01, 0x40, buffer);

	// Set input report mode (to push at 60hz)
	std::cout << "Set input report mode to 0x30..." << std::endl;
	buffer[0] = 0x30; //REPORT_MODE_NPAD_STANDARD
	sendSubCommand(0x01, 0x03, buffer);

	// TODO:
	//GetCalibrationData();

	std::cout << "Setting LEDs..." << std::endl;
	std::array<LIGHT, 4> light_modes = {LIGHT_SOLID, LIGHT_OFF, LIGHT_OFF, LIGHT_OFF}; // 1 to 4
	setPlayerLED(light_modes);

	// Small Rumble:
	std::cout << "Vibrating JoyCon..." << std::endl;
	rumble(100, RUMBLE_HIGH);
	Sleep(20);
	rumble(1, RUMBLE_OFF);
}

void JoyCon::setPlayerLED(std::array<LIGHT, 4> values)
{
	uint8_t light_mode = 0x0;
	for (int i = 0; i < 4; ++i) {
		light_mode |= static_cast<uint8_t>(values[i]) * (1 << i);
	}
	//std::cout << "Light_mode: " << int(light_mode) << std::endl;

	std::vector<uint8_t> data = { light_mode };

	//std::cout << "data: " << int(data[0]) << std::endl;

	sendSubCommand(0x01, 0x30, data);
}

void JoyCon::rumble(uint8_t frequency, RUMBLE_INTENSITY intensity)
{
	int offset = _type == LEFT ? 0 : 4;
	int intensity_int = static_cast<int>(intensity);
	
	std::vector<uint8_t> data(9,0);

	data[1 + offset] = frequency;
	data[1 + offset + intensity_int] = 1;

	hid_set_nonblocking(_handle, 1);

	sendCommand(0x10, data);
	
	hid_set_nonblocking(_handle, 0);
}

void JoyCon::update(bool verbose)
{
	if (_handle == nullptr) {
		return;
	}

	std::vector<uint8_t> data(65, 0); // 64+1 
	int len_data = hid_read_timeout(_handle, &(data[0]), 64, _read_timeout); // 10ms timeout
	if (len_data <= 0) {
		//std::cout << getStringForType() << ": No data to read!" << std::endl;
		return;
	}
	if (len_data != 64) {
		std::cout << getStringForType() << ": Got invalid len_data " << len_data << " -> IGNORE!" << std::endl;
		return;
	}

	//std::cout << "Got " << len_data << " bytes of data." << std::endl;

	parseData(data);
	if (verbose) {
		// TODO: print data
		//std::cout << _name << " - Accel: " << _accel.transpose() << std::endl;
		//std::cout << _name << " - Gyro: " << _gyro.transpose() << std::endl;
		//if (_type == RIGHT) {
		//	std::cout << _name << ": " << getButtonsStateAsString() << std::endl;
		//}
	}
}

bool JoyCon::writeToDevice(const std::vector<unsigned char>& data)
{
	if (_handle == nullptr) {
		return false;
	}
	if (data.empty()) {
		return true;
	}

	std::cout << "Writing data with length " << data.size() << " to device: " << std::endl;
	for (const auto& d : data) {
		std::cout << int(d) << " ";
	}
	std::cout << std::endl;

	int bytes_written = hid_write(_handle, &(data[0]), data.size());
	//std::cout << "Wrote " << bytes_written << " bytes." << std::endl;
	
	std::vector<unsigned char> response(65, 0);
	int bytes_read = hid_read(_handle, &(response[0]), 64);

	if (bytes_written <  0 || bytes_read < 0) {
		std::cout << "ERROR writing to device." << std::endl;
		return false;
	}
	return true; // success
}


bool JoyCon::sendCommand(uint8_t command, const std::vector<unsigned char>& data)
{
	std::vector<uint8_t> buffer = {command};
	// set command (report ID?)

	//if (!bluetooth) {
	//  buffer.resize(9, 0);
	//	buffer[0] = 0x80;
	//	buffer[1] = 0x92;
	//	buffer[3] = 0x31;
	//	buffer[8] = command;
	//}
	
	// add data
	buffer.insert(buffer.end(), data.begin(), data.end());
	return writeToDevice(buffer);
}

bool JoyCon::sendSubCommand(uint8_t command, uint8_t subcommand, const std::vector<uint8_t>& data)
{
	std::vector<uint8_t> buffer;	
	std::vector<uint8_t> rumble_base = { uint8_t(++_global_count & 0xF), 0x00, 0x01, 0x40, 0x40, 0x00, 0x01, 0x40, 0x40 };

	buffer.insert(buffer.end(), rumble_base.begin(), rumble_base.end()); // set index 0-8 of buffer
	
	if (_global_count > 0xF) {
		_global_count = 0x0;
	}

	// set neutral rumble base only if the command is vibrate (0x01)
	// if set when other commands are set, might cause the command to be misread and not executed
	//if (subcommand == 0x01) {
	//	uint8_t rumble_base[9] = { (++global_count) & 0xF, 0x00, 0x01, 0x40, 0x40, 0x00, 0x01, 0x40, 0x40 };
	//	memcpy(buf + 10, rumble_base, 9);
	//}

	buffer.push_back(subcommand); // set index 9

	// fill remaining indices with data
	buffer.insert(buffer.end(), data.begin(), data.end());
	return sendCommand(command, buffer);
}

void JoyCon::parseData(const std::vector<uint8_t>& data)
{
	// TOOD: parse data

	std::cout << "data[0]: " << int(data[0]) << std::endl;

	switch (data[0]) {
	case 0x3F:
		std::cout << "BT button pressed" << std::endl;
		break;
	case 0x21:
		std::cout << "buttons presses only" << std::endl;
		break;
	case 0x30:
		//std::cout << "IMU" << std::endl;
		//break;
	case 0x31:
		//std::cout << "NFC" << std::endl;
		//break;
	case 0x32:
		//std::cout << "?" << std::endl;
		//break;
		parseDataWithIMU(data);
	default:
		std::cout << "? data type: " << data[0] << std::endl;
	}
}

void JoyCon::parseDataWithIMU(const std::vector<uint8_t>& data)
{
	int stick_data_offset = _bluetooth_data_offset + getStickDataOffset();
	
	uint16_t stick_x = data[stick_data_offset + 0] | ((data[stick_data_offset + 1] & 0xF) << 8);
	uint16_t stick_y = (data[stick_data_offset + 1] >> 4) | (data[stick_data_offset + 2] << 4);
	uint8_t	battery = (data[stick_data_offset + 1] & 0xF0) >> 4;
	
	parseButtonsState(data);

	//	jc->CalcAnalogStick(); // apply calib

	// accel (m/s^2):
	_accel.x() = static_cast<float>(uint16_to_int16(data[13] | (data[14] << 8) & 0xFF00)); // *jc->acc_cal_coeff[0];
	_accel.y() = static_cast<float>(uint16_to_int16(data[15] | (data[16] << 8) & 0xFF00)); // *jc->acc_cal_coeff[1];
	_accel.z() = static_cast<float>(uint16_to_int16(data[17] | (data[18] << 8) & 0xFF00));  //* jc->acc_cal_coeff[2];

	// gyro (rad/s)
	_gyro.x() = static_cast<float>((uint16_to_int16(data[19] | (data[20] << 8) & 0xFF00))); // - jc->sensor_cal[1][0]) * jc->gyro_cal_coeff[0]; //23 24 was working, now not so much
	_gyro.y() = static_cast<float>((uint16_to_int16(data[21] | (data[22] << 8) & 0xFF00))); // -jc->sensor_cal[1][1])* jc->gyro_cal_coeff[1]; // 19 20 was working
	_gyro.z() = static_cast<float>((uint16_to_int16(data[23] | (data[24] << 8) & 0xFF00))); // - jc->sensor_cal[1][2]) * jc->gyro_cal_coeff[2]; // 21 22 was working

	// gyro offsets
	//jc->setGyroOffsets();
	//
	//		jc->gyro.roll -= jc->gyro.offset.roll;
	//		jc->gyro.pitch -= jc->gyro.offset.pitch;
	//		jc->gyro.yaw -= jc->gyro.offset.yaw;

	

}

