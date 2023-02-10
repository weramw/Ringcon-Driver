#pragma once

#include <string>
#include <Eigen/Core>

class IOHelper
{
public:
	IOHelper(int width=4, int precision=4);

	void setWidth(int width);
	void setPrecision(int precision);

	std::string intToString(int i) const;
	std::string toString(const Eigen::Vector2i& vec) const;
	std::string toString(const Eigen::Vector3i& vec) const;

	std::string floatToString(float f) const;
	std::string toString(const Eigen::Vector2f& vec) const;
	std::string toString(const Eigen::Vector3f& vec) const;

protected:
	int _width, _precision;
};