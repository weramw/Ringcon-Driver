#pragma once

#include <Eigen/Core>

struct JoyData {
	Eigen::Vector2f stick; // in range [-1,1]
	Eigen::Vector3f accel, gyro; // in m/s^2 and rad/s respectively
	int ring_flex; // in range [0,20]
	bool buttons[11];
	JoyData();
};


