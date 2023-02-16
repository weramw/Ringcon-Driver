#include "JoyData.h"

JoyData::JoyData() :
	stick(Eigen::Vector2f::Zero()),
	accel(Eigen::Vector3f::Zero()),
	gyro(Eigen::Vector3f::Zero()),
	ring_flex(-1)
{
	for (int i = 0; i < 11; ++i) {
		buttons[i] = false;
	}
}

