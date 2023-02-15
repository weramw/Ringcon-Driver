#include "JoyConRight.h"

JoyConRight::ButtonsState::ButtonsState() :
	Y(false),
	X(false),
	B(false),
	A(false),
	SR(false),
	SL(false),
	R(false),
	ZR(false),
	PLUS(false),
	STICK(false),
	HOME(false)
{
}

std::string JoyConRight::ButtonsState::toString() const
{
	std::stringstream ss;
	ss << "Y:\t" << (Y ? 1 : 0) << "\t";
	ss << "X:\t" << (X ? 1 : 0) << "\t";
	ss << "B:\t" << (B ? 1 : 0) << "\t";
	ss << "A:\t" << (A ? 1 : 0) << "\t";
	ss << "SR:\t" << (SR ? 1 : 0) << "\t";
	ss << "SL:\t" << (SL ? 1 : 0) << "\t";
	ss << "R:\t" << (R ? 1 : 0) << "\t";
	ss << "ZR:\t" << (ZR ? 1 : 0) << "\t";
	ss << "PLUS:\t" << (PLUS ? 1 : 0) << "\t";
	ss << "STICK:\t" << (STICK ? 1 : 0) << "\t";
	ss << "HOME:\t" << (HOME ? 1 : 0) << "\t";
	return ss.str();
}


JoyConRight::JoyConRight(hid_device_info* dev_info) :
	JoyCon(dev_info),
	_buttons_state()
{
}

void JoyConRight::parseButtonsState(const std::vector<uint8_t>& data)
{
	uint16_t btn_states = (data[_bluetooth_data_offset + 4] << 8) | (data[_bluetooth_data_offset + 3] & 0xFF);

	_buttons_state.Y = (btn_states & 1 << 0) ? true : false;
	_buttons_state.X = (btn_states & 1 << 1) ? true : false;
	_buttons_state.B = (btn_states & 1 << 2) ? true : false;
	_buttons_state.A = (btn_states & 1 << 3) ? true : false;
	_buttons_state.SR = (btn_states & 1 << 4) ? true : false;
	_buttons_state.SL = (btn_states & 1 << 5) ? true : false;
	_buttons_state.R = (btn_states & 1 << 6) ? true : false;
	_buttons_state.ZR = (btn_states & 1 << 7) ? true : false;
	_buttons_state.PLUS = (btn_states & 1 << 9) ? true : false;
	_buttons_state.STICK = (btn_states & 1 << 10) ? true : false;
	_buttons_state.HOME = (btn_states & 1 << 12) ? true : false;
}

int JoyConRight::getStickDataOffset() const
{
	return 9;
}

std::string JoyConRight::getButtonsStateAsString() const
{
	return _buttons_state.toString();
}

JoyConRight::ButtonsState JoyConRight::getButtonsState() const
{
	return _buttons_state;
}