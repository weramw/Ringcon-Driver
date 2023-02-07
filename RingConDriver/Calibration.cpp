#include "Calibration.h"

#include <iostream>

#include "ByteConversions.h"

Calibration::Calibration(hid_device* handle, bool is_left) :
	_is_left(is_left),
	_timing_byte(0x0),
	_stick_x(Eigen::Vector3i::Zero()),
	_stick_y(Eigen::Vector3i::Zero()),
	_imu(Eigen::Matrix<int, 2, 3>::Zero()),
	_accel(Eigen::Vector3f::Zero()),
	_gyro(Eigen::Vector3f::Zero())
{
	std::cout << "Getting calibration data..." << std::endl;

	getStickData(handle);
	getIMUData(handle);

	std::cout << "Got Calibration data." << std::endl;
}

void Calibration::getSpiData(hid_device* handle, uint32_t offset, uint8_t read_len, std::vector<uint8_t>& data)
{
	int max_try = 100;
	int count = 0;
	int result = -1;
	data.clear();

	std::vector<uint8_t> buffer_result;


	while (count < max_try) {
		count++;
		buffer_result.resize(256, 0);
		std::vector<uint8_t> buffer(256, 0);
		auto hdr = (brcm_hdr*)(&(buffer[0]));
		auto pkt = (brcm_cmd_01*)(hdr + 1);

		hdr->cmd = 1;
		hdr->rumble[0] = _timing_byte;

		buffer[1] = _timing_byte;

		_timing_byte++;
		if (_timing_byte > 0xF) {
			_timing_byte = 0x0;
		}

		pkt->subcmd = 0x10;
		pkt->offset = offset;
		pkt->size = read_len;

		for (int i = 11; i < 22; ++i) {
			buffer[i] = buffer[i + 3];
		}
		//std::cout << "Writing: ";
		//for (size_t i = 0; i < buffer.size(); ++i) {
		//	std::cout << int(buffer[i]) << " ";
		//}
		//std::cout << std::endl;
		result = hid_write(handle, &(buffer[0]), sizeof(*hdr) + sizeof(*pkt));

		result = hid_read(handle, &(buffer_result[0]), 256);

		if ((*(uint16_t*)&buffer_result[0xD] == 0x1090) && (*(uint32_t*)&buffer_result[0xF] == offset)) {
			break;
		}
	}
	if (count == max_try) {
		return;
	}
	if (result >= 0x14 + read_len) {
		//std::cout << "Got data" << std::endl;
		data.insert(data.end(), buffer_result.begin() + 0x14, buffer_result.begin() + 0x14 + read_len);
	}
}

void Calibration::getStickData(hid_device* handle)
{
	if (handle == nullptr) {
		std::cout << "No device to get calibration data from!" << std::endl;
		return;
	}

	//std::vector<uint8_t> stick_model, stick_model_2, factory_data, user_data;
	std::vector<uint8_t> factory_data, user_data;

	// TODO: check data was read successfully!
	//getSpiData(handle, 0x6086, 0x12, stick_model);
	//getSpiData(handle, 0x6098, 0x12, stick_model_2);
	//stick_model.insert(stick_model.end(), stick_model_2.begin(), stick_model_2.end());

	getSpiData(handle, 0x603D, 0x12, factory_data);
	//std::cout << "Factory data size: " << factory_data.size() << "(" << int(0x12) << ")" << std::endl;
	if (factory_data.size() != 0x12) {
		std::cout << "ERROR reading factory data!" << std::endl;
		return;
	}
	getSpiData(handle, 0x8010, 0x16, user_data);
	if (user_data.size() != 0x16) {
		std::cout << "ERROR reading user data!" << std::endl;
		return;
	}
	std::vector<uint8_t> sorted_calib_data;
	
	// get correct calib data
	int offset = 0;
	std::vector<uint8_t>* data_to_use = &factory_data;
	if (_is_left) {
		if ((user_data[offset] | user_data[offset + 1] << 8) == 0xA1B2) {
			offset += 2;
			data_to_use = &user_data;
			std::cout << "Getting user calib!" << std::endl;
		}
		sorted_calib_data.insert(sorted_calib_data.end(), data_to_use->begin() + offset + 3, data_to_use->begin() + offset + 9); // last 6 entries
		sorted_calib_data.insert(sorted_calib_data.end(), data_to_use->begin() + offset, data_to_use->begin() + offset + 3); // first 3 entries
	}
	else {
		offset = 9;
		if ((user_data[offset + 2] | user_data[offset + 3] << 8) == 0xA1B2) {
			offset += 2;
			data_to_use = &user_data;
		}
		sorted_calib_data.insert(sorted_calib_data.end(), data_to_use->begin() + offset, data_to_use->begin() + offset + 9); // all 9 entries
	}
	parseStickData(sorted_calib_data);
	
	std::cout << (_is_left ? "LEFT" : "RIGHT") << ": Got stick calibration X " << _stick_x.transpose() << ", Y " << _stick_y.transpose() << std::endl;
}

void Calibration::parseStickData(const std::vector<uint8_t>& data)
{
	if (data.size() != 9) {
		std::cout << "Cannot parse stick data! 9 data values required!" << std::endl;
		return;
	}
	_stick_x.y() = byte_conversions::combine_2bytesX(data[1], data[0]); // center
	_stick_x.x() = _stick_x.y() - byte_conversions::combine_2bytesX(data[4], data[3]); // min
	_stick_x.z() = _stick_x.y() + byte_conversions::combine_2bytesX(data[7], data[6]); // max

	_stick_y.y() = byte_conversions::combine_2bytesY(data[2], data[1]); // center
	_stick_y.x() = _stick_y.y() - byte_conversions::combine_2bytesY(data[5], data[4]); // min
	_stick_y.z() = _stick_y.y() + byte_conversions::combine_2bytesY(data[8], data[7]); // max
}

float convert_accel(int value)
{
	return float(1.0 / double(16384 - value) * 4.0 * 9.8);
}

float convert_gyro(int value)
{
	return float(936.0 / double(13371 - value) * 0.01745329251994);
}

void Calibration::getIMUData(hid_device* handle)
{
	std::vector<uint8_t> factory_data, user_data;// , model;
	//get_spi_data(0x6080, 0x6, sensor_model);
	
	getSpiData(handle, 0x6020, 0x18, factory_data);
	//std::cout << "Factory data size: " << factory_data.size() << "(" << int(0x12) << ")" << std::endl;
	if (factory_data.size() != 0x18) {
		std::cout << "ERROR reading factory data!" << std::endl;
		return;
	}
	getSpiData(handle, 0x8026, 0x1A, user_data);
	if (user_data.size() != 0x1A) {
		std::cout << "ERROR reading user data!" << std::endl;
		return;
	}

	int offset = 0;
	std::vector<uint8_t>* data_to_use = &factory_data;
	if (byte_conversions::combine_2bytesIMU(user_data[0], user_data[1]) == 0xA1B2) {
		int offset = 2;
		data_to_use = &user_data;
	}
	// accel origin
	_imu(0, 0) = byte_conversions::combine_2bytesIMU(data_to_use->at(offset + 0), data_to_use->at(offset + 1));
	_imu(0, 1) = byte_conversions::combine_2bytesIMU(data_to_use->at(offset + 2), data_to_use->at(offset + 3));
	_imu(0, 2) = byte_conversions::combine_2bytesIMU(data_to_use->at(offset + 4), data_to_use->at(offset + 5));

	// gyro origin
	_imu(1, 0) = byte_conversions::combine_2bytesIMU(data_to_use->at(offset + 12), data_to_use->at(offset + 13));
	_imu(1, 1) = byte_conversions::combine_2bytesIMU(data_to_use->at(offset + 14), data_to_use->at(offset + 15));
	_imu(1, 2) = byte_conversions::combine_2bytesIMU(data_to_use->at(offset + 16), data_to_use->at(offset + 17));

	// Use SPI calibration and convert them to SI acc unit
	_accel.x() = convert_accel(_imu(0, 0));
	_accel.y() = convert_accel(_imu(0, 1));
	_accel.z() = convert_accel(_imu(0, 2));

	// Use SPI calibration and convert them to SI gyro unit
	_gyro.x() = convert_gyro(_imu(1, 0));
	_gyro.y() = convert_gyro(_imu(1, 1));
	_gyro.z() = convert_gyro(_imu(1, 2));

	std::cout << (_is_left ? "LEFT" : "RIGHT") << ": Got IMU calibration ACCEL " << _imu.block(0, 0, 1, 3) << "( " << _accel.transpose() << ")" << std::endl;
	std::cout << (_is_left ? "LEFT" : "RIGHT") << ": Got IMU calibration GYRO " << _imu.block(1, 0, 1, 3) << "( " << _gyro.transpose() << ")" << std::endl;
}


int clamp(int a, int min, int max) {
	return std::max(min, std::min(a, max));	
}

float normalize(int val, const Eigen::Vector3i& range)
{
	val = clamp(val, range.x(), range.z());
	if (val >= range.y()) {
		// above center -> +
		return float(val - range.y()) / float(range.z() - range.y());
	}
	// on or below center
	return float(val - range.y()) / float(range.y() - range.x());
}

float deadZone(float value, float dead_zone) 
{	
	float length = std::fabs(value);
	if (length < dead_zone) {
		// value in inner dead zone
		return 0.f;
	}

	// scale such that output length is in the range [0.0f, 1.0f]
	float legal_range = 1.0f - dead_zone;
	float normalized_length = std::min(1.f, std::max(0.f, (length - dead_zone) / legal_range));	
	if (value >= 0.f) {
		return normalized_length;
	}
	return -normalized_length;
}

Eigen::Vector2f Calibration::stick(const Eigen::Vector2i& stick) const
{
	Eigen::Vector2f res(Eigen::Vector2f::Zero());
	// convert to float based on calibration and valid ranges per x and y axis
	res.x() = normalize(stick.x(), _stick_x);
	res.y() = normalize(stick.y(), _stick_y);

	// TOOD: make joystick calibration config!
	res.x() = deadZone(res.x(), 0.15f);
	res.y() = deadZone(res.y(), 0.15f);
	return res;
}

Eigen::Vector3f Calibration::accel(const Eigen::Vector3i& accel) const
{
	Eigen::Vector3f res(Eigen::Vector3f::Zero());
	res.x() = _accel.x() * accel.x();
	res.y() = _accel.y() * accel.y();
	res.z() = _accel.z() * accel.z();
	return res;
}

Eigen::Vector3f Calibration::gyro(const Eigen::Vector3i& gyro) const
{
	Eigen::Vector3f res(Eigen::Vector3f::Zero());
	res.x() = (_gyro.x() - _imu(1,0)) * gyro.x();
	res.y() = (_gyro.y() - _imu(1,1)) * gyro.y();
	res.z() = (_gyro.z() - _imu(1,2)) * gyro.z();
	return res;
}

void Calibration::setGyroOffsets(const Eigen::Vector3f& gyro)
{
	float thresh = 0.1f;
	if (std::abs(gyro.x()) > thresh || std::abs(gyro.y()) > thresh || std::abs(gyro.z()) > thresh) {
		return;
	}

	// TODO?
	//this->gyro.offset.n += 1;
	//this->gyro.offset.roll = this->gyro.offset.roll + ((this->gyro.roll - this->gyro.offset.roll) / this->gyro.offset.n);
	//this->gyro.offset.pitch = this->gyro.offset.pitch + ((this->gyro.pitch - this->gyro.offset.pitch) / this->gyro.offset.n);
	//this->gyro.offset.yaw = this->gyro.offset.yaw + ((this->gyro.yaw - this->gyro.offset.yaw) / this->gyro.offset.n);
}

