#include "JoyCon.h"

#include <iostream>

#define NOMINMAX // prevent windows interfering with c++ min max
#include <Windows.h>

#include "ByteConversions.h"

const int JoyCon::_bluetooth_data_offset = 0; // = 10 else

JoyCon::JoyCon(hid_device_info* dev_info) :
	_handle(nullptr),
	_calibration(nullptr),
	_serial(nullptr),
	_name("UNKOWN"),
	_type(UNKOWN),
	_read_timeout(3),
	_global_count(0), // TODO: ???
	//_stick_x(0),
	//_stick_y(0),
	//_battery(0),
	_stick(Eigen::Vector2i::Zero()),
	_accel(Eigen::Vector3i::Zero()),
	_gyro(Eigen::Vector3i::Zero()),
	_io_helper(4,4)
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

	if (_type == UNKOWN) {
		return;
	}

	_handle = hid_open_path(dev_info->path);
	if (_handle == nullptr) {
		printf("Could not open %s with serial %ls\n", _name.c_str(), _serial); // , strerror_s(errno));
		throw;
	}

	printf("Successfully opened device: %s\n", _name.c_str());

	if (_type == UNKOWN) {
		// do not initialize or load calibration if not left or right JoyCon
		return;
	}
}

JoyCon::~JoyCon()
{
	if (_handle != nullptr) {
		hid_close(_handle);
		printf("Closed device: %s\n", _name.c_str());
	}
	if (_calibration != nullptr) {
		delete _calibration;
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

Eigen::Vector2f JoyCon::getStick() const
{
	return _calibration->stick(_stick);
}

Eigen::Vector3f JoyCon::getAccel() const
{
	return _calibration->accel(_accel);
}

Eigen::Vector3f JoyCon::getGyro() const
{
	return _calibration->gyro(_gyro);
}

Eigen::Vector2i JoyCon::getStickRaw() const
{
	return _stick;
}
Eigen::Vector3i JoyCon::getAccelRaw() const
{
	return _accel;
}
Eigen::Vector3i JoyCon::getGyroRaw() const
{
	return _gyro;
}

JoyData JoyCon::getData() const
{
	JoyData data;

	data.stick = getStick();
	data.accel = getAccel();
	data.gyro = getGyro();

	return data;
}

void JoyCon::initialize()
{
	// enable stuff
	enableVibration();
	enableIMU();

	// set report mode
	setStandardReportMode();

	// calib
	getCalibrationData();

	// signal init done
	setStandardPlayerLEDs();
	smallRumble();
}

void JoyCon::enableVibration()
{
	std::vector<uint8_t> buffer = { 1 }; // Enabled
	std::cout << "Enabling Vibration..." << std::endl;
	sendSubCommand(0x1, 0x48, buffer);
}

void JoyCon::enableIMU()
{
	std::vector<uint8_t> buffer = { 1 }; // Enabled
	std::cout << "Enabling IMU data..." << std::endl;
	sendSubCommand(0x01, 0x40, buffer);
}

void JoyCon::setStandardReportMode()
{
	std::vector<uint8_t> buffer = { 1 }; // Enabled
	std::cout << "Set input report mode to 0x30..." << std::endl;
	buffer[0] = 0x30; //REPORT_MODE_NPAD_STANDARD
	sendSubCommand(0x01, 0x03, buffer);
}

void JoyCon::getCalibrationData()
{
	bool is_left = (_type == LEFT);
	_calibration = new Calibration(_handle, is_left);
}

void JoyCon::setStandardPlayerLEDs()
{
	std::cout << "Setting LEDs..." << std::endl;
	std::array<LIGHT, 4> light_modes = { LIGHT_SOLID, LIGHT_OFF, LIGHT_OFF, LIGHT_OFF }; // 1 to 4
	setPlayerLED(light_modes);
}

void JoyCon::smallRumble()
{
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
	//std::cout << _name << ": before parse - data[0] = " << int(data[0]) << std::endl;

	bool success = parseData(data);
	//std::cout << _name << ": after parse - data[0] = " << int(data[0]) << std::endl;
	if (verbose && success) {
		//if (_type == LEFT) {
		//	std::cout << "l" << std::flush;
		//} else if (_type == RIGHT) {
		//	std::cout << "r" << std::flush;
		//}
		//else {
		//	std::cout << "x" << std::flush;
		//}
		// TODO: print data
		//std::cout << _name << " - Accel: " << _accel.transpose() << std::endl;
		//std::cout << _name << " - Gyro: " << _gyro.transpose() << std::endl;
		//if (_type == RIGHT) {
		//	std::cout << _name << ": " << getButtonsStateAsString() << std::endl;
		//}
		
		//std::cout << _name << ": data[0] = " << int(data[0]) << std::endl; 
		//std::cout << _name << ": X,Y \t" << toString(_stick) << "\t\t(" << toString(stick_cal) << ")" << std::endl;
		//std::cout << _name << ": accel \t" << toString(_accel) << "\t(" << toString(accel_cal) << ")" << std::endl;
		//std::cout << _name << ": gyro \t" << toString(_gyro) << "\t(" << toString(gyro_cal) << ")" << std::endl;
		//std::cout << _name << ": gyro packets: " << int(data[19]) << " " << int(data[20]) << " | " << int(data[21]) << " " << int(data[22]) << " | " << int(data[23]) << " " << int(data[24]) << std::endl;
	}
}

int JoyCon::printStats() const
{
	Eigen::Vector2f stick_cal = getStick();
	Eigen::Vector3f accel_cal = getAccel();
	Eigen::Vector3f gyro_cal = getGyro();

	std::cout << _name << std::endl;
	std::cout << "stick \t\t" << _io_helper.toString(stick_cal) << "\t\t (" << _io_helper.toString(_stick) << ")" << std::endl;
	std::cout << "accel \t\t" << _io_helper.toString(accel_cal) << "\t (" << _io_helper.toString(_accel) << ")" << std::endl;
	std::cout << "gyro \t\t" << _io_helper.toString(gyro_cal) << "\t (" << _io_helper.toString(_gyro) << ")" << std::endl;
	return 4;
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

bool JoyCon::parseData(const std::vector<uint8_t>& data)
{
	// TOOD: parse data

	//std::cout << " in parse data[0]: " << int(data[0]) << std::endl;
	bool success = false;
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
		parseDataWithIMU(data);
		success = true;
		break;
	default:
		//std::cout << "? data type: " << data[0] << std::endl;
		std::cout << "?(" << data[0] << ")" << std::flush;
	}
	return success;
}

void JoyCon::parseDataWithIMU(const std::vector<uint8_t>& data)
{
	parseButtonsState(data);

	int stick_data_offset = _bluetooth_data_offset + getStickDataOffset();
	
	// raw stick
	_stick.x() = byte_conversions::combine_2bytesXLive(data[stick_data_offset + 0], data[stick_data_offset + 1]);
	_stick.y() = byte_conversions::combine_2bytesYLive(data[stick_data_offset + 1], data[stick_data_offset + 2]); // (data[stick_data_offset + 1] >> 4) | (data[stick_data_offset + 2] << 4);
	//_battery = (data[stick_data_offset + 1] & 0xF0) >> 4;

	// raw accel
	_accel.x() = byte_conversions::combine_2bytesIMULive(data[13], data[14]);
	_accel.y() = byte_conversions::combine_2bytesIMULive(data[15], data[16]);
	_accel.z() = byte_conversions::combine_2bytesIMULive(data[17], data[18]);

	// raw gyro
	_gyro.x() = byte_conversions::combine_2bytesIMULive(data[19], data[20]);
	_gyro.y() = byte_conversions::combine_2bytesIMULive(data[21], data[22]);
	_gyro.z() = byte_conversions::combine_2bytesIMULive(data[23], data[24]);
}