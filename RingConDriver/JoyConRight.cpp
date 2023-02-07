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

//	if (_hasRingCon) {
		// get roll:
//			jc->gyro.roll = (float)((uint16_to_int16(packet[35] | (packet[36] << 8) & 0xFF00)) - jc->sensor_cal[1][0]) * jc->gyro_cal_coeff[0]; //23 24 was working
//
//			// get pitch:
//			jc->gyro.pitch = (float)((uint16_to_int16(packet[31] | (packet[32] << 8) & 0xFF00)) - jc->sensor_cal[1][1]) * jc->gyro_cal_coeff[1]; // 19 20 was working
//
//			// get yaw:
//			jc->gyro.yaw = (float)((uint16_to_int16(packet[33] | (packet[34] << 8) & 0xFF00)) - jc->sensor_cal[1][2]) * jc->gyro_cal_coeff[2]; // 21 22 was working
//
//			// Note: All of the below orientations are from the point of view of the ringcon. May not line up with official terminology.
//			//13-14 Roll
//			//15-16 Pitch centred at horizontal
//			//17-18 Pitch centred at vertical
//			//19-20 Gyro pitch - Forward = +, Backward = -
//			//21-22 Gyro yaw (needed for running) - When running, stepping down = +, stepping up = -
//			//23-24 Gyro roll - Clockwise = +, Anticlockwise = -
//			//25-26 Roll anticlockwise +, clockwise -
//			//27-28 Pitch centred at horizontal - up = -, down = +
//			//29-30 Pitch centred at vertical - up = -, down = +
//			//31-32, 33-34, 35-36 arebouncing around but have something to do with the gyro. maybe i need a single byte?
//			//printf("%f      %f     %f", jc->gyro.roll, jc->gyro.yaw, jc->gyro.pitch);
//	}
//

//// handle button combos:
//{
//	bool lightpress = false;
//	bool lightpull = false;
//	bool heavypress = false;
//	bool heavypull = false;
//
//	// right:
//	if (jc->left_right == 2 && ringconattached) {
//		//Ringcon logic - Default values - int prevringcon = 0x0A; int ringconcounter = 0;
//
//		Ringcon = packet[40];
//
//		if (Ringcon == 0x00) { //The Ringcon reading has started randomly putting zero in to the reading, I must not be initializing it properly. This is a hack to get around that.
//			Ringcon = prevRingcon;
//		}
//
//		Ringcon = Ringcon + settings.RingconFix;
//
//		if (Ringcon >= 100) {
//			Ringcon = Ringcon - 255;
//		}
//
//		if (Ringcon != prevRingcon) {
//			printf("%i\n", Ringcon);
//		}
//
//		if (settings.RingconFullRH) { //The sensor readings change if it is being held sideways
//			if (Ringcon == 0x0A || Ringcon == 0x09 || Ringcon == 0x08 || Ringcon == 0x07) { //Deadzone
//				ringconcounter = 0;
//			}
//
//			if (Ringcon == 0x01 || Ringcon == 0xFF || Ringcon == 0xFE) {
//				heavypress = false; //turn off heavy press, may damage Ringcon as it goes outside the flex range
//				//ringconcounter = -1;
//			}
//			if (Ringcon == 0x0D || Ringcon == 0x0E || Ringcon == 0x0F) {
//				heavypull = true;
//				ringconcounter = -1;
//			}
//			if (Ringcon >= 0x02 && Ringcon <= 0x06 && ringconcounter != -1) {
//				/*if (Ringcon < prevringcon && ringconcounter < 10) {
//					ringconcounter = 0;
//				}
//				else if (Ringcon == prevringcon && ringconcounter < 10) {
//					ringconcounter++;
//				}
//				else {*/
//				lightpress = true;
//				ringconcounter = 20;
//				//}
//			}
//			if (Ringcon <= 0x0C && Ringcon >= 0x0B && ringconcounter != -1) {
//				if (Ringcon > prevRingcon && ringconcounter < 10) {
//					ringconcounter = 0;
//				}
//				else if (Ringcon == prevRingcon && ringconcounter < 10) {
//					ringconcounter++;
//				}
//				else {
//					lightpull = true;
//					ringconcounter = 20;
//				}
//			}
//		}
//		else {
//			if (Ringcon == 0x0A || Ringcon == 0x09 || Ringcon == 0x08 || Ringcon == 0x0B) { //Deadzone
//				ringconcounter = 0;
//			}
//
//			if (Ringcon >= 0x11) {
//				heavypress = true;
//				ringconcounter = -1;
//			}
//			if (Ringcon <= 0x03 && Ringcon != 0x00) {
//				heavypull = true;
//				ringconcounter = -1;
//			}
//			if (Ringcon >= 0x0C && Ringcon <= 0x10 && ringconcounter != -1) {
//				if (Ringcon > prevRingcon && ringconcounter < 10) {
//					ringconcounter = 0;
//				}
//				else if (Ringcon == prevRingcon && ringconcounter < 10) {
//					ringconcounter++;
//				}
//				else {
//					lightpress = true;
//					ringconcounter = 20;
//				}
//			}
//			if (Ringcon <= 0x07 && Ringcon >= 0x04 && ringconcounter != -1) {
//				if (Ringcon < prevRingcon && ringconcounter < 10) {
//					ringconcounter = 0;
//				}
//				else if (Ringcon == prevRingcon && ringconcounter < 10) {
//					ringconcounter++;
//				}
//				else {
//					lightpull = true;
//					ringconcounter = 20;
//				}
//			}
//		}
//
//		prevRingcon = Ringcon;
//		//printf("%i \n\n", Ringcon);
//	}
//
//	// left:
//	if (jc->left_right == 1) {
//
//		// Determine whether the left joycon is telling us we are running
//		runningindex[runvalue % runarraylength] = jc->gyro.pitch;
//		runvalue++;
//		int sum = 0;
//		int average = 0;
//		for (int i = 0; i < runarraylength; i++) {
//			if (runningindex[i] >= 0) {
//				sum += (runningindex[i] * 2);
//			}
//			else {
//				sum -= (runningindex[i] * 2); //Too many zeros means the average will be 0 even when there are quite a lot of numbers with values. This seems to be a good value with arraylength at 50.
//			}
//		}
//
//		average = sum / runarraylength;
//
//		//printf("%i\n", average); //walk 0-1, jog 1-2, run 2-3, sprint 3-4
//		if (average > 0) {
//			running = true;
//			if (settings.Runpressesbutton) {
//				jc->buttons |= 1U << 4; //sr = run
//			}
//		}
//		else {
//			running = false;
//		}
//
//		//jc->btns.sl = (jc->buttons & (1 << 5)) ? 1 : 0; // set a bit: *ptr |= 1 << index;
//		//sprint button
//		if (average >= 3) { //sprint
//			jc->buttons |= 1U << 5; //sl = sprint
//		}
//		//int squatvalue = 0;
//		//printf("%f", jc->accel.z); //9.8 when horizontal. 0 when vertical. Goes to minus when facing down or backwards.
//		if (jc->accel.z > 6.0 && jc->accel.z < 12.0) {
//			squatvalue++;
//			if (squatvalue >= 20 && !settings.squatSlowsMouse) {
//				jc->buttons |= 1U << 8; //jc->btns.minus = (jc->buttons & (1 << 8)) ? 1 : 0;
//			}
//		}
//		else {
//			squatvalue = 0;
//		}
//
//		if (jc->accel.z > 2.0 && jc->accel.z < 12.0) {
//			squatting = true;
//		}
//		else {
//			squatting = false;
//		}
//
//		if (settings.squatSlowsMouse && !running) {
//			if (jc->accel.z <= 0.1) {
//				squatmousemult = 1;
//			}
//			else if (jc->accel.z >= 9.0) {
//				squatmousemult = 0.1;
//			}
//			else {
//				squatmousemult = 1 - (jc->accel.z * 0.1);
//			}
//		}
//		else {
//			squatmousemult = 1;
//		}
//
//
//
//	// right:
//	if (jc->left_right == 2) {
//
//		//Ringcon stuff
//
//		if (lightpress == true) {
//			jc->buttons |= 1U << 4;
//		}
//
//		if (heavypress == true) {
//			jc->buttons |= 1U << 7;
//		}
//
//		if (lightpull == true) {
//			jc->buttons |= 1U << 5;
//		}
//
//		if (heavypull == true) {
//			jc->buttons |= 1U << 6;
//		}
//
//		//Mouse buttons
//		//printf("%i\n", Ringcon);
//		if (settings.enableGyro) {
//			if ((jc->buttons & (1 << 7) || Ringcon >= 0x0C) && !leftmousedown) { //ZR controls left mouse button
//				MC.LeftClickDown();
//				leftmousedown = true;
//			}
//			if (!(jc->buttons & (1 << 7) || Ringcon >= 0x0C) && leftmousedown) {
//				MC.LeftClickUp();
//				leftmousedown = false;
//			}
//			if ((jc->buttons & (1 << 6) || lightpull || heavypull) && !rightmousedown) { //R controls right mouse button
//				MC.RightClickDown();
//				rightmousedown = true;
//			}
//			if (!(jc->buttons & (1 << 6) || lightpull || heavypull) && rightmousedown) {
//				MC.RightClickUp();
//				rightmousedown = false;
//			}
//		}
//
//	}
