#pragma once

#include <stdint.h>

namespace byte_conversions
{
	int16_t uint16_to_int16(uint16_t a);

	// calib
	int combine_2bytesX(uint8_t a, uint8_t b);
	int combine_2bytesY(uint8_t a, uint8_t b);
	int combine_2bytesIMU(uint8_t a, uint8_t b);

	// joycon
	int combine_2bytesXLive(uint8_t a, uint8_t b);
	int combine_2bytesYLive(uint8_t a, uint8_t b);
	int combine_2bytesIMULive(uint8_t a, uint8_t b);
}