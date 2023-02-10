#include "IOHelper.h"

#include <sstream>
#include <iomanip>

IOHelper::IOHelper(int width, int precision) :
_width(0),
_precision(0)
{
	setWidth(width);
	setPrecision(precision);
}

void IOHelper::setWidth(int width)
{
	_width = width + 1; // account for sign
}
void IOHelper::setPrecision(int precision)
{
	_precision = precision;
}

std::string IOHelper::intToString(int i) const
{
	std::stringstream ss;
	ss << std::setfill(' ') << std::setw(_width) << i;
	return ss.str();
}

std::string IOHelper::toString(const Eigen::Vector2i& vec) const
{
	std::stringstream ss;
	ss << intToString(vec.x()) << " " << intToString(vec.y());
	return ss.str();
}

std::string IOHelper::toString(const Eigen::Vector3i& vec) const
{
	std::stringstream ss;
	ss << intToString(vec.x()) << " " << intToString(vec.y()) << " " << intToString(vec.z());
	return ss.str();
}

std::string IOHelper::floatToString(float f) const
{
	std::stringstream ss;
	ss << std::fixed << std::setprecision(_precision) << std::setfill(' ') << std::setw(_width+_precision) << f;
	return ss.str();
}

std::string IOHelper::toString(const Eigen::Vector2f& vec) const
{
	std::stringstream ss;
	ss << floatToString(vec.x()) << " " << floatToString(vec.y());
	return ss.str();
}

std::string IOHelper::toString(const Eigen::Vector3f& vec) const
{
	std::stringstream ss;
	ss << floatToString(vec.x()) << " " << floatToString(vec.y()) << " " << floatToString(vec.z());
	return ss.str();
}