#pragma once


#include <stdio.h>
#include <tchar.h>

#include <windows.h>
#include <basetyps.h>
#include <cfgmgr32.h>
#include <Setupapi.h>
#include <strsafe.h>
#include <Newdev.h>
#include <INITGUID.H>

#include <malloc.h>
#include <string>
#include <stdlib.h>
//#include "Math.h"

#include <vJoy/public.h>
#include <vJoy/vjoyinterface.h>

#include "JoyData.h"

class VJoyController
{
public:
	VJoyController(UINT device_id=1);
	~VJoyController();

	// returns true on success
	bool initialize();

	int sendData(const JoyData& data);

protected:
	JOYSTICK_POSITION_V2 makeReport(const JoyData& data);

protected:
	UINT _device_id;

	int _status;

	std::string _vendor_id, _product_id;

	USHORT _X, _Y, _Z;
	LONG   _Btns;
};

