#include "ByteConversions.h"

#include <cstring>

namespace byte_conversions
{
	int16_t uint16_to_int16(uint16_t a) {
		int16_t b;
		char* aPointer = (char*)&a, * bPointer = (char*)&b;
		memcpy(bPointer, aPointer, sizeof(a));
		return b;
	}

	int combine_2bytesX(uint8_t a, uint8_t b)
	{
		return static_cast<int>(uint16_to_int16((a << 8) & 0xF00 | b));
	}

	int combine_2bytesY(uint8_t a, uint8_t b)
	{
		return static_cast<int>(uint16_to_int16((a << 4) | (b >> 4)));
	}

	int combine_2bytesIMU(uint8_t a, uint8_t b)
	{
		return static_cast<int>(uint16_to_int16(a | b << 8));
	}

	int combine_2bytesXLive(uint8_t a, uint8_t b)
	{ 
		return static_cast<int>(uint16_to_int16(a | (b & 0xF) << 8));
	}

	int combine_2bytesYLive(uint8_t a, uint8_t b)
	{
		return static_cast<int>(uint16_to_int16((a >> 4) | (b << 4)));
	}

	int combine_2bytesIMULive(uint8_t a, uint8_t b)
	{
		return static_cast<int>(uint16_to_int16(a | (b << 8) & 0xFF00));
	}
}