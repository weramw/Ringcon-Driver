#pragma once

#include <vector>

#include <Eigen/Core>

#include <hidapi.h>

class Calibration
{
public:
	Calibration(hid_device* handle, bool is_left);

	void stick(uint16_t x, uint16_t y, float& res_x, float& res_y) const;
	void accel(const Eigen::Vector3f& accel) const;
	void gyro(const Eigen::Vector3f gyro) const;

	void setGyroOffsets(const Eigen::Vector3f& gyro);

protected:	

	struct brcm_hdr {
		uint8_t cmd;
		uint8_t rumble[9];
	};

	struct brcm_cmd_01 {
		uint8_t subcmd;
		uint32_t offset;
		uint8_t size;
	};

	void getSpiData(hid_device* handle, uint32_t offset, uint8_t read_len, std::vector<uint8_t>& data);

	void getStickData(hid_device* handle);	
	void parseStickData(const std::vector<uint8_t>& data);

	//void getIMUData(hid_device* handle);
	
protected:
	bool _is_left;
	int _timing_byte;

	//bool _has_user_stick_l, _has_user_stick_r, _has_user_imu;
	
	float _x, _y;
	Eigen::Vector3f _accel_coeff, _gyro_coeff;

	uint16_t _factory_imu_calm[0xC];
	uint16_t _user_imu_calm[0xC];
	int16_t _imu[0x2][0x3];
	//uint16_t _stick_x[0x3];
	//uint16_t _stick_y[0x3];

	Eigen::Vector3i _stick_x, _stick_y; // min, center, max
};

