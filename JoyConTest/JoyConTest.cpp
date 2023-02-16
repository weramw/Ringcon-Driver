// vJoyClient.cpp : Simple feeder application with a FFB demo
//


// Monitor Force Feedback (FFB) vJoy device
#include "stdafx.h"
//#include "Devioctl.h"
#include "public.h"
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "vjoyinterface.h"
#include "Math.h"

// Default device ID (Used when ID not specified)
#define DEV_ID		1

// Prototypes
void  CALLBACK FfbFunction(PVOID data);
void  CALLBACK FfbFunction1(PVOID cb, PVOID data);

BOOL PacketType2Str(FFBPType Type, LPTSTR Str);
BOOL EffectType2Str(FFBEType Ctrl, LPTSTR Str);
BOOL DevCtrl2Str(FFB_CTRL Type, LPTSTR Str);
BOOL EffectOpStr(FFBOP Op, LPTSTR Str);
int  Polar2Deg(BYTE Polar);
int  Byte2Percent(BYTE InByte);
int TwosCompByte2Int(BYTE in);


int ffb_direction = 0;
int ffb_strenght = 0;
int serial_result = 0;


JOYSTICK_POSITION_V2 iReport; // The structure that holds the full position data

int
__cdecl
_tmain(int argc, _TCHAR* argv[])
{
	int stat = 0;
	UINT DevID = DEV_ID;
	USHORT X = 0;
	USHORT Y = 0;
	USHORT Z = 0;
	LONG   Btns = 0;


	PVOID pPositionMessage;
	UINT	IoCode = LOAD_POSITIONS;
	UINT	IoSize = sizeof(JOYSTICK_POSITION);
	// HID_DEVICE_ATTRIBUTES attrib;
	BYTE id = 1;
	UINT iInterface = 1;

	// Define the effect names
	static FFBEType FfbEffect = (FFBEType)-1;
	LPCTSTR FfbEffectName[] =
	{ _T("NONE"), _T("Constant Force"), _T("Ramp"), _T("Square"), _T("Sine"), _T("Triangle"), _T("Sawtooth Up"),\
	_T("Sawtooth Down"), _T("Spring"), _T("Damper"), _T("Inertia"), _T("Friction"), _T("Custom Force") };

	// Set the target Joystick - get it from the command-line 
	if (argc > 1)
		DevID = _tstoi(argv[1]);


	// Get the driver attributes (Vendor ID, Product ID, Version Number)
	if (!vJoyEnabled())
	{
		_tprintf(_T("Function vJoyEnabled Failed - make sure that vJoy is installed and enabled\n"));
		int dummy = getchar();
		stat = -2;
		RelinquishVJD(DevID);
		return 0;
	}
	else
	{
		wprintf(L"Vendor: %s\nProduct :%s\nVersion Number:%s\n", static_cast<TCHAR*> (GetvJoyManufacturerString()), static_cast<TCHAR*>(GetvJoyProductString()), static_cast<TCHAR*>(GetvJoySerialNumberString()));
	};

	// Get the status of the vJoy device before trying to acquire it
	VjdStat status = GetVJDStatus(DevID);

	switch (status)
	{
	case VJD_STAT_OWN:
		_tprintf(_T("vJoy device %d is already owned by this feeder\n"), DevID);
		break;
	case VJD_STAT_FREE:
		_tprintf(_T("vJoy device %d is free\n"), DevID);
		break;
	case VJD_STAT_BUSY:
		_tprintf(_T("vJoy device %d is already owned by another feeder\nCannot continue\n"), DevID);
		return -3;
	case VJD_STAT_MISS:
		_tprintf(_T("vJoy device %d is not installed or disabled\nCannot continue\n"), DevID);
		return -4;
	default:
		_tprintf(_T("vJoy device %d general error\nCannot continue\n"), DevID);
		return -1;
	};

	// Acquire the vJoy device
	if (!AcquireVJD(DevID))
	{
		_tprintf(_T("Failed to acquire vJoy device number %d.\n"), DevID);
		int dummy = getchar();
		stat = -1;
		RelinquishVJD(DevID);
		return 0;
	}
	else
		_tprintf(_T("Acquired device number %d - OK\n"), DevID);



	// Start FFB
#if 1
	//BOOL Ffbstarted = FfbStart(DevID);
	//if (!Ffbstarted)
	//{
	//	_tprintf(_T("Failed to start FFB on vJoy device number %d.\n"), DevID);
	//	int dummy = getchar();
	//	stat = -3;
	//	goto Exit;
	//}
	//else
	//	_tprintf(_T("Started FFB on vJoy device number %d - OK\n"), DevID);

#endif // 1

	// Register Generic callback function
	// At this point you instruct the Receptor which callback function to call with every FFB packet it receives
	// It is the role of the designer to register the right FFB callback function
	FfbRegisterGenCB(FfbFunction1, NULL);




	// Start endless loop
	// The loop injects position data to the vJoy device
	// If it fails it let's the user try again
	//
	// FFB Note:
	// All FFB activity is performed in a separate thread created when registered the callback function   
	while (1)
	{

		// Set destenition vJoy device
		id = (BYTE)DevID;
		iReport.bDevice = id;

		// Set position data of 3 first axes
		if (Z > 35000) Z = 0;
		Z += 200;
		iReport.wAxisZ = Z;
		iReport.wAxisX = 32000 - Z;
		iReport.wAxisY = Z / 2 + 7000;

		// Set position data of first 8 buttons
		Btns = 1 << (Z / 4000);
		iReport.lButtons = Btns;

		// Send position data to vJoy device
		pPositionMessage = (PVOID)(&iReport);
		if (!UpdateVJD(DevID, pPositionMessage))
		{
			printf("Feeding vJoy device number %d failed - try to enable device then press enter\n", DevID);
			//getchar();
			AcquireVJD(DevID);
		}
		Sleep(2);
	}
}


// Generic callback function
void CALLBACK FfbFunction(PVOID data)
{
	FFB_DATA* FfbData = (FFB_DATA*)data;
	int size = FfbData->size;
	_tprintf(_T("\nFFB Size %d\n"), size);

	_tprintf(_T("Cmd:%08.8X "), FfbData->cmd);
	_tprintf(_T("ID:%02.2X "), FfbData->data[0]);
	_tprintf(_T("Size:%02.2d "), static_cast<int>(FfbData->size - 8));
	_tprintf(_T(" - "));
	for (UINT i = 0; i < FfbData->size - 8; i++)
		_tprintf(_T(" %02.2X"), (UINT)FfbData->data);
	_tprintf(_T("\n"));
}

void CALLBACK FfbFunction1(PVOID data, PVOID userdata)
{
	// Packet Header
	_tprintf(_T("\n ============= FFB Packet size Size %d =============\n"), static_cast<int>(((FFB_DATA*)data)->size));

	/////// Packet Device ID, and Type Block Index (if exists)
#pragma region Packet Device ID, and Type Block Index
	int DeviceID, BlockIndex;
	FFBPType	Type;
	TCHAR	TypeStr[100];

	if (ERROR_SUCCESS == Ffb_h_DeviceID((FFB_DATA*)data, &DeviceID))
		_tprintf(_T("\n > Device ID: %d"), DeviceID);
	if (ERROR_SUCCESS == Ffb_h_Type((FFB_DATA*)data, &Type))
	{
		if (!PacketType2Str(Type, TypeStr))
			_tprintf(_T("\n > Packet Type: %d"), Type);
		else
			_tprintf(_T("\n > Packet Type: %s"), TypeStr);

	}
	if (ERROR_SUCCESS == Ffb_h_EBI((FFB_DATA*)data, &BlockIndex))
		_tprintf(_T("\n > Effect Block Index: %d"), BlockIndex);
#pragma endregion


	/////// Effect Report
#pragma region Effect Report
	//FFB_EFF_REPORT Effect;
	//if (ERROR_SUCCESS == Ffb_h_Eff_Report((FFB_DATA*)data, &Effect))
	//{
	//	if (!EffectType2Str(Effect.EffectType, TypeStr))
	//		_tprintf(_T("\n >> Effect Report: %02x"), Effect.EffectType);
	//	else
	//		_tprintf(_T("\n >> Effect Report: %s"), TypeStr);

	//	if (Effect.Polar)
	//	{
	//		_tprintf(_T("\n >> Direction: %d deg (%02x)"), Polar2Deg(Effect.Direction), Effect.Direction);


	//	}
	//	else
	//	{
	//		_tprintf(_T("\n >> X Direction: %02x"), Effect.DirX);
	//		_tprintf(_T("\n >> Y Direction: %02x"), Effect.DirY);
	//	};

	//	if (Effect.Duration == 0xFFFF)
	//		_tprintf(_T("\n >> Duration: Infinit"));
	//	else
	//		_tprintf(_T("\n >> Duration: %d MilliSec"), static_cast<int>(Effect.Duration));

	//	if (Effect.TrigerRpt == 0xFFFF)
	//		_tprintf(_T("\n >> Trigger Repeat: Infinit"));
	//	else
	//		_tprintf(_T("\n >> Trigger Repeat: %d"), static_cast<int>(Effect.TrigerRpt));

	//	if (Effect.SamplePrd == 0xFFFF)
	//		_tprintf(_T("\n >> Sample Period: Infinit"));
	//	else
	//		_tprintf(_T("\n >> Sample Period: %d"), static_cast<int>(Effect.SamplePrd));


	//	_tprintf(_T("\n >> Gain: %d%%"), Byte2Percent(Effect.Gain));

	//};
#pragma endregion
#pragma region PID Device Control
	FFB_CTRL	Control;
	TCHAR	CtrlStr[100];
	if (ERROR_SUCCESS == Ffb_h_DevCtrl((FFB_DATA*)data, &Control) && DevCtrl2Str(Control, CtrlStr))
		_tprintf(_T("\n >> PID Device Control: %s"), CtrlStr);

#pragma endregion
#pragma region Effect Operation
	FFB_EFF_OP	Operation;
	TCHAR	EffOpStr[100];
	if (ERROR_SUCCESS == Ffb_h_EffOp((FFB_DATA*)data, &Operation) && EffectOpStr(Operation.EffectOp, EffOpStr))
	{
		_tprintf(_T("\n >> Effect Operation: %s"), EffOpStr);
		if (Operation.LoopCount == 0xFF)
			_tprintf(_T("\n >> Loop until stopped"));
		else
			_tprintf(_T("\n >> Loop %d times"), static_cast<int>(Operation.LoopCount));

	};
#pragma endregion
#pragma region Global Device Gain
	BYTE Gain;
	if (ERROR_SUCCESS == Ffb_h_DevGain((FFB_DATA*)data, &Gain))
		_tprintf(_T("\n >> Global Device Gain: %d"), Byte2Percent(Gain));

#pragma endregion
#pragma region Condition
	FFB_EFF_COND Condition;
	if (ERROR_SUCCESS == Ffb_h_Eff_Cond((FFB_DATA*)data, &Condition))
	{
		if (Condition.isY)
			_tprintf(_T("\n >> Y Axis"));
		else
			_tprintf(_T("\n >> X Axis"));
		_tprintf(_T("\n >> Center Point Offset: %d"), TwosCompByte2Int(Condition.CenterPointOffset) * 10000 / 127);
		_tprintf(_T("\n >> Positive Coefficient: %d"), TwosCompByte2Int(Condition.PosCoeff) * 10000 / 127);
		_tprintf(_T("\n >> Negative Coefficient: %d"), TwosCompByte2Int(Condition.NegCoeff) * 10000 / 127);
		_tprintf(_T("\n >> Positive Saturation: %d"), Condition.PosSatur * 10000 / 255);
		_tprintf(_T("\n >> Negative Saturation: %d"), Condition.NegSatur * 10000 / 255);
		_tprintf(_T("\n >> Dead Band: %d"), Condition.DeadBand * 10000 / 255);
	}
#pragma endregion
#pragma region Envelope
	FFB_EFF_ENVLP Envelope;
	if (ERROR_SUCCESS == Ffb_h_Eff_Envlp((FFB_DATA*)data, &Envelope))
	{
		_tprintf(_T("\n >> Attack Level: %d"), Envelope.AttackLevel * 10000 / 255);
		_tprintf(_T("\n >> Fade Level: %d"), Envelope.FadeLevel * 10000 / 255);
		_tprintf(_T("\n >> Attack Time: %d"), static_cast<int>(Envelope.AttackTime));
		_tprintf(_T("\n >> Fade Time: %d"), static_cast<int>(Envelope.FadeTime));
	};

#pragma endregion
#pragma region Periodic
	FFB_EFF_PERIOD EffPrd;
	if (ERROR_SUCCESS == Ffb_h_Eff_Period((FFB_DATA*)data, &EffPrd))
	{
		_tprintf(_T("\n >> Magnitude: %d"), EffPrd.Magnitude * 10000 / 255);
		_tprintf(_T("\n >> Offset: %d"), TwosCompByte2Int(EffPrd.Offset) * 10000 / 127);
		_tprintf(_T("\n >> Phase: %d"), EffPrd.Phase * 3600 / 255);
		_tprintf(_T("\n >> Period: %d"), static_cast<int>(EffPrd.Period));
	};
#pragma endregion

#pragma region Effect Type
	FFBEType EffectType;
	if (ERROR_SUCCESS == Ffb_h_EffNew((FFB_DATA*)data, &EffectType))
	{
		if (EffectType2Str(EffectType, TypeStr))
			_tprintf(_T("\n >> Effect Type: %s"), TypeStr);
		else
			_tprintf(_T("\n >> Effect Type: Unknown"));
	}

#pragma endregion

#pragma region Ramp Effect
	FFB_EFF_RAMP RampEffect;
	if (ERROR_SUCCESS == Ffb_h_Eff_Ramp((FFB_DATA*)data, &RampEffect))
	{
		_tprintf(_T("\n >> Ramp Start: %d"), TwosCompByte2Int(RampEffect.Start) * 10000 / 127);
		_tprintf(_T("\n >> Ramp End: %d"), TwosCompByte2Int(RampEffect.End) * 10000 / 127);
	};

#pragma endregion

	_tprintf(_T("\n"));
	FfbFunction(data);
	_tprintf(_T("\n ====================================================\n"));

}


// Convert Packet type to String
BOOL PacketType2Str(FFBPType Type, LPTSTR OutStr)
{
	BOOL stat = TRUE;
	LPTSTR Str = LPTSTR("");

	switch (Type)
	{
	case PT_EFFREP:
		Str = LPTSTR("Effect Report");
		break;
	case PT_ENVREP:
		Str = LPTSTR("Envelope Report");
		break;
	case PT_CONDREP:
		Str = LPTSTR("Condition Report");
		break;
	case PT_PRIDREP:
		Str = LPTSTR("Periodic Report");
		break;
	case PT_CONSTREP:
		Str = LPTSTR("Constant Force Report");
		break;
	case PT_RAMPREP:
		Str = LPTSTR("Ramp Force Report");
		break;
	case PT_CSTMREP:
		Str = LPTSTR("Custom Force Data Report");
		break;
	case PT_SMPLREP:
		Str = LPTSTR("Download Force Sample");
		break;
	case PT_EFOPREP:
		Str = LPTSTR("Effect Operation Report");
		break;
	case PT_BLKFRREP:
		Str = LPTSTR("PID Block Free Report");
		break;
	case PT_CTRLREP:
		Str = LPTSTR("PID Device Contro");
		break;
	case PT_GAINREP:
		Str = LPTSTR("Device Gain Report");
		break;
	case PT_SETCREP:
		Str = LPTSTR("Set Custom Force Report");
		break;
	case PT_NEWEFREP:
		Str = LPTSTR("Create New Effect Report");
		break;
	case PT_BLKLDREP:
		Str = LPTSTR("Block Load Report");
		break;
	case PT_POOLREP:
		Str = LPTSTR("PID Pool Report");
		break;
	default:
		stat = FALSE;
		break;
	}

	if (stat)
		_tcscpy_s(OutStr, 100, Str);

	return stat;
}

// Convert Effect type to String
BOOL EffectType2Str(FFBEType Type, LPTSTR OutStr)
{
	BOOL stat = TRUE;
	LPTSTR Str = LPTSTR("");

	switch (Type)
	{
	case ET_NONE:
		stat = FALSE;
		break;
	case ET_CONST:
		Str = LPTSTR("Constant Force");
		break;
	case ET_RAMP:
		Str = LPTSTR("Ramp");
		break;
	case ET_SQR:
		Str = LPTSTR("Square");
		break;
	case ET_SINE:
		Str = LPTSTR("Sine");
		break;
	case ET_TRNGL:
		Str = LPTSTR("Triangle");
		break;
	case ET_STUP:
		Str = LPTSTR("Sawtooth Up");
		break;
	case ET_STDN:
		Str = LPTSTR("Sawtooth Down");
		break;
	case ET_SPRNG:
		Str = LPTSTR("Spring");
		break;
	case ET_DMPR:
		Str = LPTSTR("Damper");
		break;
	case ET_INRT:
		Str = LPTSTR("Inertia");
		break;
	case ET_FRCTN:
		Str = LPTSTR("Friction");
		break;
	case ET_CSTM:
		Str = LPTSTR("Custom Force");
		break;
	default:
		stat = FALSE;
		break;
	};

	if (stat)
		_tcscpy_s(OutStr, 100, Str);

	return stat;
}

// Convert PID Device Control to String
BOOL DevCtrl2Str(FFB_CTRL Ctrl, LPTSTR OutStr)
{
	BOOL stat = TRUE;
	LPTSTR Str = LPTSTR("");

	switch (Ctrl)
	{
	case CTRL_ENACT:
		Str = LPTSTR("Enable Actuators");
		break;
	case CTRL_DISACT:
		Str = LPTSTR("Disable Actuators");
		break;
	case CTRL_STOPALL:
		Str = LPTSTR("Stop All Effects");
		break;
	case CTRL_DEVRST:
		Str = LPTSTR("Device Reset");
		break;
	case CTRL_DEVPAUSE:
		Str = LPTSTR("Device Pause");
		break;
	case CTRL_DEVCONT:
		Str = LPTSTR("Device Continue");
		break;
	default:
		stat = FALSE;
		break;
	}
	if (stat)
		_tcscpy_s(OutStr, 100, Str);

	return stat;
}

// Convert Effect operation to string
BOOL EffectOpStr(FFBOP Op, LPTSTR OutStr)
{
	BOOL stat = TRUE;
	LPTSTR Str = LPTSTR("");

	switch (Op)
	{
	case EFF_START:
		Str = LPTSTR("Effect Start");
		break;
	case EFF_SOLO:
		Str = LPTSTR("Effect Solo Start");
		break;
	case EFF_STOP:
		Str = LPTSTR("Effect Stop");
		break;
	default:
		stat = FALSE;
		break;
	}

	if (stat)
		_tcscpy_s(OutStr, 100, Str);

	return stat;
}

// Polar values (0x00-0xFF) to Degrees (0-360)
int Polar2Deg(BYTE Polar)
{
	return ((UINT)Polar * 360) / 255;
}

// Convert range 0x00-0xFF to 0%-100%
int Byte2Percent(BYTE InByte)
{
	return ((UINT)InByte * 100) / 255;
}

// Convert One-Byte 2's complement input to integer
int TwosCompByte2Int(BYTE in)
{
	int tmp;
	BYTE inv = ~in;
	BOOL isNeg = in >> 7;
	if (isNeg)
	{
		tmp = (int)(inv);
		tmp = -1 * tmp;
		return tmp;
	}
	else
		return (int)in;
}
