#include "JoyConLeft.h"

#include <sstream>

JoyConLeft::ButtonsState::ButtonsState() :
	DOWN(false),
    UP(false),
	RIGHT(false),
	LEFT(false),
	SR(false),
	SL(false),
	L(false),
	ZL(false),
	MINUS(false),
	STICK(false),
	CAPTURE(false)
{
}

std::string JoyConLeft::ButtonsState::toString() const
{
	std::stringstream ss;
	ss << "DOWN:\t" << (DOWN ? 1 : 0) << "\t";
	ss << "UP:\t" << (UP ? 1 : 0) << "\t";
	ss << "RIGHT:\t" << (RIGHT ? 1 : 0) << "\t";
	ss << "LEFT:\t" << (LEFT ? 1 : 0) << "\t";
	ss << "SR:\t" << (SR ? 1 : 0) << "\t";
	ss << "SL:\t" << (SL ? 1 : 0) << "\t";
	ss << "L:\t" << (L ? 1 : 0) << "\t";
	ss << "ZL:\t" << (ZL ? 1 : 0) << "\t";
	ss << "MINUS:\t" << (MINUS ? 1 : 0) << "\t";
	ss << "STICK:\t" << (STICK ? 1 : 0) << "\t";
	ss << "CAPTURE:\t" << (CAPTURE ? 1 : 0) << "\t";
	return ss.str();
}

JoyConLeft::JoyConLeft(hid_device_info* dev_info) :
	JoyCon(dev_info),
	_buttons_state()
{
}

void JoyConLeft::parseButtonsState(const std::vector<uint8_t>& data)
{
	uint16_t btn_states = (data[_bluetooth_data_offset + 4] << 8) | (data[_bluetooth_data_offset + 5] & 0xFF);

	_buttons_state.DOWN = (btn_states & 1 << 0) ? true : false;
	_buttons_state.UP = (btn_states & 1 << 1) ? true : false;
	_buttons_state.RIGHT = (btn_states & 1 << 2) ? true : false;
	_buttons_state.LEFT = (btn_states & 1 << 3) ? true : false;
	_buttons_state.SR = (btn_states & 1 << 4) ? true : false;
	_buttons_state.SL = (btn_states & 1 << 5) ? true : false;
	_buttons_state.L = (btn_states & 1 << 6) ? true : false;
	_buttons_state.ZL = (btn_states & 1 << 7) ? true : false;
	_buttons_state.MINUS = (btn_states & 1 << 8) ? true : false;
	_buttons_state.STICK = (btn_states & 1 << 11) ? true : false;
	_buttons_state.CAPTURE = (btn_states & 1 << 13) ? true : false;
}

int JoyConLeft::getStickDataOffset() const
{
	return 6;
}

std::string JoyConLeft::getButtonsStateAsString() const
{
	return _buttons_state.toString();
}

JoyConLeft::ButtonsState JoyConLeft::getButtonsState() const
{
	return _buttons_state;
}