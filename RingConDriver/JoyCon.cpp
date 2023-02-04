#include "JoyCon.h"

#include <iostream>

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
	_global_count(0) // TODO: ???
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
	std::array<LIGHT, 4> light_modes = {SOLID, OFF, OFF, OFF}; // 1 to 4
	setPlayerLED(light_modes);
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

	for (size_t i = 0; i < 5; ++i){
		sendSubCommand(0x01, 0x30, data);
	}
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

	//parseData(data);
	if (verbose) {
		// TODO: print data
	}
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
		std::cout << "IMU" << std::endl;
		break;
	case 0x31:
		std::cout << "NFC" << std::endl;
		break;
	case 0x32:
		std::cout << "?" << std::endl;
		break;
	default:
		std::cout << "? data type: " << data[0] << std::endl;
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

//void send_subcommand(int command, int subcommand, uint8_t* data, int len) {
//	unsigned char buf[0x40];
//	memset(buf, 0, 0x40);
//
//	uint8_t rumble_base[9] = { (++global_count) & 0xF, 0x00, 0x01, 0x40, 0x40, 0x00, 0x01, 0x40, 0x40 };
//	memcpy(buf, rumble_base, 9);
//
//	if (global_count > 0xF) {
//		global_count = 0x0;
//	}
//
//	// set neutral rumble base only if the command is vibrate (0x01)
//	// if set when other commands are set, might cause the command to be misread and not executed
//	//if (subcommand == 0x01) {
//	//	uint8_t rumble_base[9] = { (++global_count) & 0xF, 0x00, 0x01, 0x40, 0x40, 0x00, 0x01, 0x40, 0x40 };
//	//	memcpy(buf + 10, rumble_base, 9);
//	//}
//
//	buf[9] = subcommand;
//	if (data && len != 0) {
//		memcpy(buf + 10, data, len);
//	}
//
//	send_command(command, buf, 10 + len);
//
//	if (data) {
//		memcpy(data, buf, 0x40); //TODO
//	}
//}


//void send_command(int command, uint8_t* data, int len) {
//	unsigned char buf[0x40];
//	memset(buf, 0, 0x40);
//
//	if (!bluetooth) {
//		buf[0x00] = 0x80;
//		buf[0x01] = 0x92;
//		buf[0x03] = 0x31;
//	}
//
//	buf[bluetooth ? 0x0 : 0x8] = command;
//	if (data != nullptr && len != 0) {
//		memcpy(buf + (bluetooth ? 0x1 : 0x9), data, len);
//	}
//
//	hid_exchange(this->handle, buf, len + (bluetooth ? 0x1 : 0x9));
//
//	if (data) {
//		memcpy(data, buf, 0x40);
//	}
//}


//void hid_exchange(hid_device* handle, unsigned char* buf, int len) {
//	if (!handle) return;
//
//	int res;
//
//	res = hid_write(handle, buf, len);
//
//	//if (res < 0) {
//	//	printf("Number of bytes written was < 0!\n");
//	//} else {
//	//	printf("%d bytes written.\n", res);
//	//}
//
//	//// set non-blocking:
//	//hid_set_nonblocking(handle, 1);
//
//	res = hid_read(handle, buf, 0x40);
//
//	//if (res < 1) {
//	//	printf("Number of bytes read was < 1!\n");
//	//} else {
//	//	printf("%d bytes read.\n", res);
//	//}
//}




//// input update packet:
//// 0x21 is just buttons, 0x30 includes gyro, 0x31 includes NFC (large packet size)
//if (packet[0] == 0x30 || packet[0] == 0x31 || packet[0] == 0x32) {
//
//	// offset for usb or bluetooth data:
//	/*int offset = settings.usingBluetooth ? 0 : 10;*/
//	int offset = jc->bluetooth ? 0 : 10;
//
//	uint8_t* btn_data = packet + offset + 3;
//
//	// get button states:
//	{
//		uint16_t states = 0;
//		uint16_t states2 = 0;
//
//		// Left JoyCon:
//		if (jc->left_right == 1) {
//			states = (btn_data[1] << 8) | (btn_data[2] & 0xFF);
//			// Right JoyCon:
//		}
//		else if (jc->left_right == 2) {
//			states = (btn_data[1] << 8) | (btn_data[0] & 0xFF);
//			// Pro Controller:
//		}
//		else if (jc->left_right == 3) {
//			states = (btn_data[1] << 8) | (btn_data[2] & 0xFF);
//			states2 = (btn_data[1] << 8) | (btn_data[0] & 0xFF);
//		}
//
//		jc->buttons = states;
//		// Pro Controller:
//		if (jc->left_right == 3) {
//			jc->buttons2 = states2;
//
//			// fix some non-sense the Pro Controller does
//			// clear nth bit
//			//num &= ~(1UL << n);
//			jc->buttons &= ~(1UL << 9);
//			jc->buttons &= ~(1UL << 10);
//			jc->buttons &= ~(1UL << 12);
//			jc->buttons &= ~(1UL << 14);
//
//			jc->buttons2 &= ~(1UL << 8);
//			jc->buttons2 &= ~(1UL << 11);
//			jc->buttons2 &= ~(1UL << 13);
//		}
//	}
//
//	// get stick data:
//	uint8_t* stick_data = packet + offset;
//	if (jc->left_right == 1) {
//		stick_data += 6;
//	}
//	else if (jc->left_right == 2) {
//		stick_data += 9;
//	}
//
//	uint16_t stick_x = stick_data[0] | ((stick_data[1] & 0xF) << 8);
//	uint16_t stick_y = (stick_data[1] >> 4) | (stick_data[2] << 4);
//	jc->stick.x = stick_x;
//	jc->stick.y = stick_y;
//
//	// use calibration data:
//	jc->CalcAnalogStick();
//
//	// pro controller:
//	if (jc->left_right == 3) {
//		stick_data += 6;
//		uint16_t stick_x = stick_data[0] | ((stick_data[1] & 0xF) << 8);
//		uint16_t stick_y = (stick_data[1] >> 4) | (stick_data[2] << 4);
//		jc->stick.x = (int)(unsigned int)stick_x;
//		jc->stick.y = (int)(unsigned int)stick_y;
//		stick_data += 3;
//		uint16_t stick_x2 = stick_data[0] | ((stick_data[1] & 0xF) << 8);
//		uint16_t stick_y2 = (stick_data[1] >> 4) | (stick_data[2] << 4);
//		jc->stick2.x = (int)(unsigned int)stick_x2;
//		jc->stick2.y = (int)(unsigned int)stick_y2;
//
//		// calibration data:
//		jc->CalcAnalogStick();
//	}
//
//	jc->battery = (stick_data[1] & 0xF0) >> 4;
//	//printf("JoyCon battery: %d\n", jc->battery);
//
//	// Accelerometer:
//	// Accelerometer data is absolute (m/s^2)
//	{
//
//		// get accelerometer X:
//		jc->accel.x = (float)(uint16_to_int16(packet[13] | (packet[14] << 8) & 0xFF00)) * jc->acc_cal_coeff[0];
//
//		// get accelerometer Y:
//		jc->accel.y = (float)(uint16_to_int16(packet[15] | (packet[16] << 8) & 0xFF00)) * jc->acc_cal_coeff[1];
//
//		// get accelerometer Z:
//		jc->accel.z = (float)(uint16_to_int16(packet[17] | (packet[18] << 8) & 0xFF00)) * jc->acc_cal_coeff[2];
//	}
//
//
//
//	// Gyroscope:
//	// Gyroscope data is relative (rads/s)
//	if (jc->left_right == 2 && ringconattached) {
//		{
//
//			// get roll:
//			jc->gyro.roll = (float)((uint16_to_int16(packet[35] | (packet[36] << 8) & 0xFF00)) - jc->sensor_cal[1][0]) * jc->gyro_cal_coeff[0]; //23 24 was working
//
//			// get pitch:
//			jc->gyro.pitch = (float)((uint16_to_int16(packet[31] | (packet[32] << 8) & 0xFF00)) - jc->sensor_cal[1][1]) * jc->gyro_cal_coeff[1]; // 19 20 was working
//
//			// get yaw:
//			jc->gyro.yaw = (float)((uint16_to_int16(packet[33] | (packet[34] << 8) & 0xFF00)) - jc->sensor_cal[1][2]) * jc->gyro_cal_coeff[2]; // 21 22 was working
//
//			// Note: All of the below orientations are from the point of view of the ringcon. May not line up with official terminology.
//			//13-14 Roll
//			//15-16 Pitch centred at horizontal
//			//17-18 Pitch centred at vertical
//			//19-20 Gyro pitch - Forward = +, Backward = -
//			//21-22 Gyro yaw (needed for running) - When running, stepping down = +, stepping up = -
//			//23-24 Gyro roll - Clockwise = +, Anticlockwise = -
//			//25-26 Roll anticlockwise +, clockwise -
//			//27-28 Pitch centred at horizontal - up = -, down = +
//			//29-30 Pitch centred at vertical - up = -, down = +
//			//31-32, 33-34, 35-36 arebouncing around but have something to do with the gyro. maybe i need a single byte?
//			//printf("%f      %f     %f", jc->gyro.roll, jc->gyro.yaw, jc->gyro.pitch);
//		}
//	}
//	else {
//
//		// get roll:
//		jc->gyro.roll = (float)((uint16_to_int16(packet[19] | (packet[20] << 8) & 0xFF00)) - jc->sensor_cal[1][0]) * jc->gyro_cal_coeff[0]; //23 24 was working, now not so much
//
//		// get pitch:
//		jc->gyro.pitch = (float)((uint16_to_int16(packet[21] | (packet[22] << 8) & 0xFF00)) - jc->sensor_cal[1][1]) * jc->gyro_cal_coeff[1]; // 19 20 was working
//
//		// get yaw:
//		jc->gyro.yaw = (float)((uint16_to_int16(packet[23] | (packet[24] << 8) & 0xFF00)) - jc->sensor_cal[1][2]) * jc->gyro_cal_coeff[2]; // 21 22 was working
//	}
//
//	// offsets:
//	{
//		jc->setGyroOffsets();
//
//		jc->gyro.roll -= jc->gyro.offset.roll;
//		jc->gyro.pitch -= jc->gyro.offset.pitch;
//		jc->gyro.yaw -= jc->gyro.offset.yaw;
//	}
//
//}
//
//
//
//
//
//
//// handle button combos:
//{
//	bool lightpress = false;
//	bool lightpull = false;
//	bool heavypress = false;
//	bool heavypull = false;
//
//	// right:
//	if (jc->left_right == 2 && ringconattached) {
//		//Ringcon logic - Default values - int prevringcon = 0x0A; int ringconcounter = 0;
//
//		Ringcon = packet[40];
//
//		if (Ringcon == 0x00) { //The Ringcon reading has started randomly putting zero in to the reading, I must not be initializing it properly. This is a hack to get around that.
//			Ringcon = prevRingcon;
//		}
//
//		Ringcon = Ringcon + settings.RingconFix;
//
//		if (Ringcon >= 100) {
//			Ringcon = Ringcon - 255;
//		}
//
//		if (Ringcon != prevRingcon) {
//			printf("%i\n", Ringcon);
//		}
//
//		if (settings.RingconFullRH) { //The sensor readings change if it is being held sideways
//			if (Ringcon == 0x0A || Ringcon == 0x09 || Ringcon == 0x08 || Ringcon == 0x07) { //Deadzone
//				ringconcounter = 0;
//			}
//
//			if (Ringcon == 0x01 || Ringcon == 0xFF || Ringcon == 0xFE) {
//				heavypress = false; //turn off heavy press, may damage Ringcon as it goes outside the flex range
//				//ringconcounter = -1;
//			}
//			if (Ringcon == 0x0D || Ringcon == 0x0E || Ringcon == 0x0F) {
//				heavypull = true;
//				ringconcounter = -1;
//			}
//			if (Ringcon >= 0x02 && Ringcon <= 0x06 && ringconcounter != -1) {
//				/*if (Ringcon < prevringcon && ringconcounter < 10) {
//					ringconcounter = 0;
//				}
//				else if (Ringcon == prevringcon && ringconcounter < 10) {
//					ringconcounter++;
//				}
//				else {*/
//				lightpress = true;
//				ringconcounter = 20;
//				//}
//			}
//			if (Ringcon <= 0x0C && Ringcon >= 0x0B && ringconcounter != -1) {
//				if (Ringcon > prevRingcon && ringconcounter < 10) {
//					ringconcounter = 0;
//				}
//				else if (Ringcon == prevRingcon && ringconcounter < 10) {
//					ringconcounter++;
//				}
//				else {
//					lightpull = true;
//					ringconcounter = 20;
//				}
//			}
//		}
//		else {
//			if (Ringcon == 0x0A || Ringcon == 0x09 || Ringcon == 0x08 || Ringcon == 0x0B) { //Deadzone
//				ringconcounter = 0;
//			}
//
//			if (Ringcon >= 0x11) {
//				heavypress = true;
//				ringconcounter = -1;
//			}
//			if (Ringcon <= 0x03 && Ringcon != 0x00) {
//				heavypull = true;
//				ringconcounter = -1;
//			}
//			if (Ringcon >= 0x0C && Ringcon <= 0x10 && ringconcounter != -1) {
//				if (Ringcon > prevRingcon && ringconcounter < 10) {
//					ringconcounter = 0;
//				}
//				else if (Ringcon == prevRingcon && ringconcounter < 10) {
//					ringconcounter++;
//				}
//				else {
//					lightpress = true;
//					ringconcounter = 20;
//				}
//			}
//			if (Ringcon <= 0x07 && Ringcon >= 0x04 && ringconcounter != -1) {
//				if (Ringcon < prevRingcon && ringconcounter < 10) {
//					ringconcounter = 0;
//				}
//				else if (Ringcon == prevRingcon && ringconcounter < 10) {
//					ringconcounter++;
//				}
//				else {
//					lightpull = true;
//					ringconcounter = 20;
//				}
//			}
//		}
//
//		prevRingcon = Ringcon;
//		//printf("%i \n\n", Ringcon);
//	}
//
//	// left:
//	if (jc->left_right == 1) {
//
//		// Determine whether the left joycon is telling us we are running
//		runningindex[runvalue % runarraylength] = jc->gyro.pitch;
//		runvalue++;
//		int sum = 0;
//		int average = 0;
//		for (int i = 0; i < runarraylength; i++) {
//			if (runningindex[i] >= 0) {
//				sum += (runningindex[i] * 2);
//			}
//			else {
//				sum -= (runningindex[i] * 2); //Too many zeros means the average will be 0 even when there are quite a lot of numbers with values. This seems to be a good value with arraylength at 50.
//			}
//		}
//
//		average = sum / runarraylength;
//
//		//printf("%i\n", average); //walk 0-1, jog 1-2, run 2-3, sprint 3-4
//		if (average > 0) {
//			running = true;
//			if (settings.Runpressesbutton) {
//				jc->buttons |= 1U << 4; //sr = run
//			}
//		}
//		else {
//			running = false;
//		}
//
//		//jc->btns.sl = (jc->buttons & (1 << 5)) ? 1 : 0; // set a bit: *ptr |= 1 << index;
//		//sprint button
//		if (average >= 3) { //sprint
//			jc->buttons |= 1U << 5; //sl = sprint
//		}
//		//int squatvalue = 0;
//		//printf("%f", jc->accel.z); //9.8 when horizontal. 0 when vertical. Goes to minus when facing down or backwards.
//		if (jc->accel.z > 6.0 && jc->accel.z < 12.0) {
//			squatvalue++;
//			if (squatvalue >= 20 && !settings.squatSlowsMouse) {
//				jc->buttons |= 1U << 8; //jc->btns.minus = (jc->buttons & (1 << 8)) ? 1 : 0;
//			}
//		}
//		else {
//			squatvalue = 0;
//		}
//
//		if (jc->accel.z > 2.0 && jc->accel.z < 12.0) {
//			squatting = true;
//		}
//		else {
//			squatting = false;
//		}
//
//		if (settings.squatSlowsMouse && !running) {
//			if (jc->accel.z <= 0.1) {
//				squatmousemult = 1;
//			}
//			else if (jc->accel.z >= 9.0) {
//				squatmousemult = 0.1;
//			}
//			else {
//				squatmousemult = 1 - (jc->accel.z * 0.1);
//			}
//		}
//		else {
//			squatmousemult = 1;
//		}
//
//		//Mouse buttons
//		if (settings.enableGyro && !settings.combineJoyCons) {
//			if (jc->buttons & (1 << 7) && !leftmousedown) { //ZL controls left mouse button
//				MC.LeftClickDown();
//				leftmousedown = true;
//			}
//			if (!(jc->buttons & (1 << 7)) && leftmousedown) { //ZL controls left mouse button
//				MC.LeftClickUp();
//				leftmousedown = false;
//			}
//			if (jc->buttons & (1 << 6) && !rightmousedown) { //L controls right mouse button
//				MC.RightClickDown();
//				rightmousedown = true;
//			}
//			if (!(jc->buttons & (1 << 6)) && rightmousedown) { //L controls right mouse button
//				MC.RightClickUp();
//				rightmousedown = false;
//			}
//		}
//
//		jc->btns.down = (jc->buttons & (1 << 0)) ? 1 : 0;
//		jc->btns.up = (jc->buttons & (1 << 1)) ? 1 : 0;
//		jc->btns.right = (jc->buttons & (1 << 2)) ? 1 : 0;
//		jc->btns.left = (jc->buttons & (1 << 3)) ? 1 : 0;
//		jc->btns.sr = (jc->buttons & (1 << 4)) ? 1 : 0;
//		jc->btns.sl = (jc->buttons & (1 << 5)) ? 1 : 0;
//		jc->btns.l = (jc->buttons & (1 << 6)) ? 1 : 0;
//		jc->btns.zl = (jc->buttons & (1 << 7)) ? 1 : 0;
//		jc->btns.minus = (jc->buttons & (1 << 8)) ? 1 : 0;
//		jc->btns.stick_button = (jc->buttons & (1 << 11)) ? 1 : 0;
//		jc->btns.capture = (jc->buttons & (1 << 13)) ? 1 : 0;
//	}
//
//	// right:
//	if (jc->left_right == 2) {
//
//		//Ringcon stuff
//
//		if (lightpress == true) {
//			jc->buttons |= 1U << 4;
//		}
//
//		if (heavypress == true) {
//			jc->buttons |= 1U << 7;
//		}
//
//		if (lightpull == true) {
//			jc->buttons |= 1U << 5;
//		}
//
//		if (heavypull == true) {
//			jc->buttons |= 1U << 6;
//		}
//
//		//Mouse buttons
//		//printf("%i\n", Ringcon);
//		if (settings.enableGyro) {
//			if ((jc->buttons & (1 << 7) || Ringcon >= 0x0C) && !leftmousedown) { //ZR controls left mouse button
//				MC.LeftClickDown();
//				leftmousedown = true;
//			}
//			if (!(jc->buttons & (1 << 7) || Ringcon >= 0x0C) && leftmousedown) {
//				MC.LeftClickUp();
//				leftmousedown = false;
//			}
//			if ((jc->buttons & (1 << 6) || lightpull || heavypull) && !rightmousedown) { //R controls right mouse button
//				MC.RightClickDown();
//				rightmousedown = true;
//			}
//			if (!(jc->buttons & (1 << 6) || lightpull || heavypull) && rightmousedown) {
//				MC.RightClickUp();
//				rightmousedown = false;
//			}
//		}
//
//		jc->btns.y = (jc->buttons & (1 << 0)) ? 1 : 0;
//		jc->btns.x = (jc->buttons & (1 << 1)) ? 1 : 0;
//		jc->btns.b = (jc->buttons & (1 << 2)) ? 1 : 0;
//		jc->btns.a = (jc->buttons & (1 << 3)) ? 1 : 0;
//		jc->btns.sr = (jc->buttons & (1 << 4)) ? 1 : 0;
//		jc->btns.sl = (jc->buttons & (1 << 5)) ? 1 : 0;
//		jc->btns.r = (jc->buttons & (1 << 6)) ? 1 : 0;
//		jc->btns.zr = (jc->buttons & (1 << 7)) ? 1 : 0;
//		jc->btns.plus = (jc->buttons & (1 << 9)) ? 1 : 0;
//		jc->btns.stick_button = (jc->buttons & (1 << 10)) ? 1 : 0;
//		jc->btns.home = (jc->buttons & (1 << 12)) ? 1 : 0;
//	}
//
//	// pro controller:
//	if (jc->left_right == 3) {
//
//		// left:
//		jc->btns.down = (jc->buttons & (1 << 0)) ? 1 : 0;
//		jc->btns.up = (jc->buttons & (1 << 1)) ? 1 : 0;
//		jc->btns.right = (jc->buttons & (1 << 2)) ? 1 : 0;
//		jc->btns.left = (jc->buttons & (1 << 3)) ? 1 : 0;
//		jc->btns.sr = (jc->buttons & (1 << 4)) ? 1 : 0;
//		jc->btns.sl = (jc->buttons & (1 << 5)) ? 1 : 0;
//		jc->btns.l = (jc->buttons & (1 << 6)) ? 1 : 0;
//		jc->btns.zl = (jc->buttons & (1 << 7)) ? 1 : 0;
//		jc->btns.minus = (jc->buttons & (1 << 8)) ? 1 : 0;
//		jc->btns.stick_button = (jc->buttons & (1 << 11)) ? 1 : 0;
//		jc->btns.capture = (jc->buttons & (1 << 13)) ? 1 : 0;
//
//		// right:
//		jc->btns.y = (jc->buttons2 & (1 << 0)) ? 1 : 0;
//		jc->btns.x = (jc->buttons2 & (1 << 1)) ? 1 : 0;
//		jc->btns.b = (jc->buttons2 & (1 << 2)) ? 1 : 0;
//		jc->btns.a = (jc->buttons2 & (1 << 3)) ? 1 : 0;
//		jc->btns.sr = (jc->buttons2 & (1 << 4)) ? 1 : 0;
//		jc->btns.sl = (jc->buttons2 & (1 << 5)) ? 1 : 0;
//		jc->btns.r = (jc->buttons2 & (1 << 6)) ? 1 : 0;
//		jc->btns.zr = (jc->buttons2 & (1 << 7)) ? 1 : 0;
//		jc->btns.plus = (jc->buttons2 & (1 << 9)) ? 1 : 0;
//		jc->btns.stick_button2 = (jc->buttons2 & (1 << 10)) ? 1 : 0;
//		jc->btns.home = (jc->buttons2 & (1 << 12)) ? 1 : 0;
//
//	}