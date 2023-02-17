#include "RingCon.h"

#include <iostream>

#include "ByteConversions.h"

uint8_t RingCon::_mcu_crc8_table[256] = {
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

uint8_t RingCon::_ringmcu_crc8_table[256] = {
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

uint8_t RingCon::_mcu_crc8_calc(uint8_t* buffer, uint8_t size) { //CTCAER
	uint8_t crc8 = 0x0;

	for (int i = 0; i < size; ++i) {
		crc8 = _mcu_crc8_table[(uint8_t)(crc8 ^ *(buffer+i))];
	}
	return crc8;
}

uint8_t RingCon::_ringmcu_crc8_calc(uint8_t* buffer, uint8_t size) {
	uint8_t crc8 = 0x0;

	for (int i = 0; i < size; ++i) {
		crc8 = _ringmcu_crc8_table[(uint8_t)(crc8 ^ *(buffer+i))];
	}
	return crc8;
}

std::vector<uint8_t> RingCon::getDefaultCommand(uint8_t command, bool set_rumble_base)
{
	std::vector<uint8_t> buffer(10, 0);
	buffer[0] = command;
	buffer[1] = _timing_byte & 0xF;
	_timing_byte++;
	if (_timing_byte > 0xF) {
		_timing_byte = 0x0;
	}

	if (set_rumble_base) {
		buffer[2] = 0x00;
		buffer[3] = 0x01;
		buffer[4] = 0x40;
		buffer[5] = 0x40;
		buffer[6] = 0x00;
		buffer[7] = 0x01;
		buffer[8] = 0x40;
		buffer[9] = 0x40;
	}
	return buffer;
}

RingCon::RingCon(hid_device_info* dev_info) :
	JoyConRight(dev_info),
	_hasRingCon(false),
	_timing_byte(0x0),
	_gyro_ring(Eigen::Vector3i::Zero()),
	_flex(10)
{
}

void RingCon::initialize()
{
	JoyConRight::initialize();

	// try setting up RingCon
	if (!enableMCU()) {
		return;
	}
	if (!enableMCUPolling()) {
		return;
	}
	if (!setMCUMode()) {
		return;
	}
	if (getExtData()) {
		std::cout << "Initializing RingCon..." << std::endl;
		if (enableIMU() && setExtFormatConfig() && enableExtPolling() && setExtConfig()) {
			std::cout << "done" << std::endl;
			_hasRingCon = true;
		}
		else {
			std::cout << "ERROR initializing RingCon" << std::endl;
		}
	}
	else {
		JoyCon::enableIMU();
		std::cout << "Did not initialize RingCon, maybe already initialized?" << std::endl;
		_hasRingCon = true; // HACK, check when parsing data that _flex has sane range, if not disable RingCon
	}

	if (_hasRingCon) {
		_name = "RingCon";
	}
}

int RingCon::printStats() const
{
	int lines_printed = JoyCon::printStats();

	if (!_hasRingCon) {		
		return lines_printed;
	}

	Eigen::Vector3f gyro_ring_cal = getGyroRing();

	std::cout << "ring gyro\t" << _io_helper.toString(gyro_ring_cal) << "\t (" << _io_helper.toString(getGyroRingRaw()) << ")" << std::endl;
	std::cout << "ring press\t" << _io_helper.intToString(getFlex()) << std::endl;

	return lines_printed+2;
}

int RingCon::getFlex() const
{
	_mtx_data.lock();
	int res = _flex;
	_mtx_data.unlock();
	return res;
}

Eigen::Vector3f RingCon::getGyroRing() const
{ 
	// TODO: maybe calib needs to be changed since other axes have different order....
	return _calibration->gyro(_gyro_ring);
}

Eigen::Vector3i RingCon::getGyroRingRaw() const
{
	_mtx_data.lock();
	Eigen::Vector3i res = _gyro_ring;
	_mtx_data.unlock();
	return res;
}

JoyData RingCon::getData() const
{
	JoyData data = JoyConRight::getData();
	if (_hasRingCon) {
		data.ring_flex = _flex;
	}
	return data;
}

bool RingCon::enableMCU()
{
	std::vector<uint8_t> data = { 0x22, 0x1 }; // MUC, ENABLE
	std::cout << "Enabling MUC data..." << std::endl;

	std::vector<std::pair<int, uint8_t>> check_data = {
		{13, 128}, {14, 34}
	};
	bool success = communicateWithDevice(data, check_data, 1, true);
	if (!success) {
		std::cout << "ERROR: enableMCU!" << std::endl;
		return false;
	}
	std::cout << "...done." << std::endl;
	return true;
}

bool RingCon::enableMCUPolling()
{
	std::vector<uint8_t> data(39, 0);
	data[0] = 0x21;
	data[1] = 0x21;
	data[2] = 0x00;
	data[3] = 0x03; // was index 10-13
	// 4 - 37 => 34x 0
	data[38] = _mcu_crc8_calc(&(data[2]), 36); // buf[48] = mcu_crc8_calc(buf + 12, 36);
	
	std::cout << "Enabling MCU polling..." << std::endl;

	std::vector<std::pair<int, uint8_t>> check_data = {
		{0, 0x21}, 
		{15, 0x01},
		{22, 0x03}
	};

	bool success = communicateWithDevice(data, check_data, 1, false);
	if (!success) {
		std::cout << "ERROR: enableMCUpolling!" << std::endl;
		return false;
	}
	std::cout << "...done." << std::endl;
	return true;
}

bool RingCon::setMCUMode()
{
	std::vector<uint8_t> data(39, 0);
	data[0] = 0x21;
	data[1] = 0x21;
	data[2] = 0x01;
	data[3] = 0x01; // was index 10-13
	// 4 - 37 => 34x 0
	data[38] = _mcu_crc8_calc(&(data[2]), 36); // buf[48] = mcu_crc8_calc(buf + 12, 36);

	std::cout << "Set MCU mode..." << std::endl;

	std::vector<std::pair<int, uint8_t>> check_data = {
		{0, 0x21},
		{15, 0x09},
		{17, 0x01}
	};

	bool success = communicateWithDevice(data, check_data, 1, false);
	if (!success) {
		std::cout << "ERROR: setMCUMode!" << std::endl;
		return false;
	}
	std::cout << "...done." << std::endl;
	return true;
}

bool RingCon::getExtData()
{
	std::vector<uint8_t> data(39, 0);	
	data[0] = 0x59;
	std::cout << "Getting ext data..." << std::endl;

	std::vector<std::pair<int, uint8_t>> check_data = {
		{0, 0x21},
		{14, 0x59},
		{16, 0x20}
	};

	bool success = communicateWithDevice(data, check_data, 1, false);
	if (!success) {
		std::cout << "ERROR: getExtData!" << std::endl;
		return false;
	}
	std::cout << "...done." << std::endl;
	return true;
}

bool RingCon::enableIMU()
{
	std::vector<uint8_t> data(39, 0);
	data[0] = 0x40;
	data[1] = 0x03;
	std::cout << "Enabling IMU..." << std::endl;

	std::vector<std::pair<int, uint8_t>> check_data = {
		{0, 0x21},
		{14, 0x40},		
	};

	bool success = communicateWithDevice(data, check_data, 1, true);
	if (!success) {
		std::cout << "ERROR: enableIMU!" << std::endl;
		return false;
	}

	check_data.clear();
	data[1] = 0x02;
	communicateWithDevice(data, check_data, 1, true);
	data[1] = 0x01;
	communicateWithDevice(data, check_data, 1, true);

	std::cout << "...done." << std::endl;
	return true;
}

bool RingCon::setExtFormatConfig()
{
	std::vector<uint8_t> data(39, 0);
	data[0] = 0x5C;
	data[1] = 0x06;
	data[2] = 0x03;
	data[3] = 0x25;
	data[4] = 0x06;
	data[9] = 0x1C;
	data[10] = 0x16;
	data[11] = 237;// 0xCD;//+
	data[12] = 52;// 0xA6;//+
	data[13] = 54;// 0x2B;//+ These change but appear to be connected with the last values
	data[17] = 10;//timestampbuffer[0];// 27-32 = timestamp
	data[18] = 100;//timestampbuffer[1];
	data[19] = 11;//timestampbuffer[2];
	data[20] = 230;//timestampbuffer[3];
	data[21] = 169;//timestampbuffer[4];
	data[22] = 34;//timestampbuffer[5];
	data[23] = 0x00;//timestampbuffer[6];
	data[24] = 0x00;//timestampbuffer[7]; 
	data[25] = 0x04;
	data[33] = 0x90;
	data[34] = 0xA8;
	data[35] = 225;// 0xC1;//+
	data[36] = 52;// 0x34;// 0xA6;//+
	data[37] = 54;// 0x36;// 0x2B;//+ These numbers have something to do with the input report from 40 3 */

	std::cout << "Setting ext format config..." << std::endl;

	std::vector<std::pair<int, uint8_t>> check_data = {
		{0, 0x21},
		{14, 0x5C} // Ringcon config set
	};

	bool success = communicateWithDevice(data, check_data, 1, false);
	if (!success) {
		std::cout << "ERROR: setExtFormatConfig!" << std::endl;
		return false;
	}
	std::cout << "...done." << std::endl;
	return true;
}

bool RingCon::enableExtPolling()
{
	std::vector<uint8_t> data(39, 0);
	data[0] = 0x5A;
	data[1] = 0x04;
	data[2] = 0x01;
	data[3] = 0x01;
	data[4] = 0x02;

	std::cout << "Enable ext polling..." << std::endl;

	std::vector<std::pair<int, uint8_t>> check_data = {
		{0, 0x21},
		{14, 0x5A} // Mcu mode is ringcon polling
	};

	bool success = communicateWithDevice(data, check_data, 1, false);
	if (!success) {
		std::cout << "ERROR: enableExtPolling!" << std::endl;
		return false;
	}
	std::cout << "...done." << std::endl;
	return true;
}

bool RingCon::setExtConfig()
{
	std::vector<uint8_t> data(39, 0);
	data[0] = 0x58;
	data[1] = 0x04;
	data[2] = 0x04;
	data[3] = 0x12;
	data[4] = 0x02;

	std::cout << "Set ext config..." << std::endl;

	std::vector<std::pair<int, uint8_t>> check_data = {
		{0, 0x21},
		{14, 0x58} // Mcu mode is ringcon polling
	};

	bool success = communicateWithDevice(data, check_data, 1, false);
	if (!success) {
		std::cout << "ERROR: setExtConfig!" << std::endl;
		return false;
	}
	std::cout << "...done." << std::endl;
	return true;

}

bool RingCon::communicateWithDevice(const std::vector<uint8_t>& data, const std::vector<std::pair<int, uint8_t>>& check_data, uint8_t command, bool set_rumble_base, int read_length)
{
	int max_try = 10;
	int write_count = 0;
	int result = -1;
	
	while (write_count < max_try) {
		write_count++;
		std::vector<uint8_t> buffer = getDefaultCommand(command, set_rumble_base);
		buffer.insert(buffer.end(), data.begin(), data.end());

		std::cout << "Writing: ";
		for (size_t i = 0; i < buffer.size(); ++i) {
			std::cout << int(buffer[i]) << " ";
		}
		std::cout << std::endl;
		result = hid_write(_handle, &(buffer[0]), buffer.size());

		int read_count = 0;
		while (read_count < max_try) {
			read_count++;
			std::vector<uint8_t> buffer_result(read_length, 0); // data to be read
			result = hid_read(_handle, &(buffer_result[0]), read_length);

			if (result < 0) {
				continue; // no successful read
			}

			bool is_good = true;
			for (const auto& check : check_data) {
				//std::cout << "Checking " << check.first << ": " << int(buffer_result[check.first]) << " vs " << int(check.second) << std::endl;
				if (buffer_result[check.first] != check.second) {
					is_good = false;
					break;
				}
			}
			if (is_good) {
				return true; // success
			}
		}
	}
	return false; // max tries
}

void RingCon::parseDataWithIMU(const std::vector<uint8_t>& data)
{
	JoyCon::parseDataWithIMU(data);

	if (!_hasRingCon) {
		return;
	}

	// raw gyro from ring
	_gyro_ring.x() = byte_conversions::combine_2bytesIMULive(data[35], data[36]);
	_gyro_ring.y() = byte_conversions::combine_2bytesIMULive(data[31], data[32]);
	_gyro_ring.z() = byte_conversions::combine_2bytesIMULive(data[33], data[34]);
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

	_flex = int(data[40]); // fully pulled = 0x00, rest = 0x0a, fully pushed = 0x14. (0 - 10 - 20)

	// checking for sane value range
	// HACK to check if RingCon is properly initialized
	// if not, disable RingCon
	if (_flex < 0 || _flex > 20) {
		_hasRingCon = false;
	}
}