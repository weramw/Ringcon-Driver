#include "VJoyController.h"

#include <iostream>


VJoyController::VJoyController(UINT device_id) :
	_device_id(device_id),
	_vendor_id(),
	_product_id(),
	_status(0),
	_X(0),
	_Y(0),
	_Z(0),
	_Btns(0)
{
}

VJoyController::~VJoyController()
{
	if (_status >= 0) {
		sendData(JoyData()); // send default values before freeing
	}
	RelinquishVJD(_device_id);
	_status = 2; // freed device
}

bool VJoyController::initialize()
{
	if (!vJoyEnabled()){
		std::cout << "Function vJoyEnabled Failed - make sure that vJoy is installed and enabled"<< std::endl;
		int dummy = getchar();
		_status = -2;
		RelinquishVJD(_device_id);
		return false;
	}

	wprintf(L"Vendor: %s\nProduct :%s\nVersion Number:%s\n", static_cast<TCHAR*> (GetvJoyManufacturerString()), static_cast<TCHAR*>(GetvJoyProductString()), static_cast<TCHAR*>(GetvJoySerialNumberString()));

	_vendor_id = static_cast<TCHAR*>(GetvJoyManufacturerString());
	_product_id = static_cast<TCHAR*>(GetvJoyProductString()); //GetvJoySerialNumberString()

	std::cout << "Trying to acquire device with VID " << _vendor_id << " and PID " << _product_id << "..." << std::endl;

	// Get the status of the vJoy device before trying to acquire it
	VjdStat status = GetVJDStatus(_device_id);

	switch (status){
	case VJD_STAT_OWN:
		std::cout << "vJoy device " << _device_id << " is already owned by this feeder" << std::endl;
		_status = 0;
		break;
	case VJD_STAT_FREE:
		std::cout << "vJoy device " << _device_id << " is free" << std::endl;
		_status = 1;
		break;
	case VJD_STAT_BUSY:
		std::cout << "vJoy device " << _device_id << " is already owned by another feeder -> Cannot continue!" << std::endl;
		_status = -3;
		break;
	case VJD_STAT_MISS:
		std::cout << "vJoy device " << _device_id << " is not installed or disabled -> Cannot continue!" << std::endl;
		_status = -4;
		break;
	default:
		std::cout << "vJoy device " << _device_id << " general error -> Cannot continue!" << std::endl;
		_status = -1;
	};

	if (status < 0) {
		return false;
	}

	if (status == 0) {
		// we already own the device... nothing to do?
		return true;
	}

	
	// device is free and we need to acquire it	
	// Acquire the vJoy device
	if (!AcquireVJD(_device_id)) {
		std::cout << "Failed to acquire vJoy device number " << _device_id << " -> Cannot continue!" << std::endl;
		int dummy = getchar();
		_status = -1;
		RelinquishVJD(_device_id);
		return false;
	}

	std::cout << "Acquired device number " << _device_id << " - OK!" << std::endl;
	return true;
}

int VJoyController::sendData(const JoyData& data)
{
	JOYSTICK_POSITION_V2 iReport = makeReport(data); // The structure that holds the full position data

	// Send position data to vJoy device
	PVOID pPositionMessage = (PVOID)(&iReport);
	if (!UpdateVJD(_device_id, pPositionMessage)) {
		std::cout << "Feeding vJoy device number " << _device_id << " failed - try to enable device then press enter" << std::endl;
		AcquireVJD(_device_id);
		return 1;
	}
	return 0;
}

USHORT getAxisValue(float axis, float min_axis, float max_axis)
{
	float scale = 1.f / (max_axis - min_axis);
	axis = max(min_axis, min(max_axis, axis)); // clip
	axis = scale*(axis-min_axis); // normalize
	int axis_val = int(std::floor(axis * 32768.f));
	return USHORT(axis_val);
}

USHORT getStickValue(float axis)
{
	return getAxisValue(axis, -1.f, 1.f);
}

USHORT getAccelValue(float axis)
{
	return getAxisValue(axis, -20.f, 20.f);
}

USHORT getGyroValue(float axis)
{
	return getAxisValue(axis, -12.f, 12.f);
}


JOYSTICK_POSITION_V2 VJoyController::makeReport(const JoyData& data)
{
	// TODO: actually use data

	// Set destination vJoy device
	JOYSTICK_POSITION_V2 iReport;
	iReport.bDevice = (BYTE)_device_id;

	// axes
	iReport.wAxisX = getAccelValue(data.accel.x());
	iReport.wAxisY = getAccelValue(data.accel.y());
	iReport.wAxisZ = getAccelValue(data.accel.z());

	iReport.wAxisXRot = getGyroValue(data.gyro.x());
	iReport.wAxisYRot = getGyroValue(data.gyro.y());
	iReport.wAxisZRot = getGyroValue(data.gyro.z());

	iReport.wSlider = getStickValue(data.stick.x());
	iReport.wDial = getStickValue(data.stick.y());

	// ring flex
	if (data.ring_flex >= 0) {
		//iReport.wThrottle = data.ring_flex;
		iReport.wAxisX = getAxisValue(float(data.ring_flex), 0.f, 20.f);
	}

	// buttons
	LONG btns = 0;
	for (int i = 0; i < 11; ++i) {
		btns |= ((data.buttons[i] ? 0x1 : 0x0) << i);
	}
	iReport.lButtons = btns;

	return iReport;
}