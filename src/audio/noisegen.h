#pragma once
#include <cstdint>

struct NoiseGen
{
	uint8_t	nType = 0;
	uint8_t nFrequency = 0;
	uint16_t nTone2Freq = 0;
	uint16_t nCounter = 0;
	uint16_t nShiftReg  = 0x8000;
	uint16_t nTapBit = 0x0009;

	float fSample = 0.0f;
	float fAttenuation = 0.0f;

	bool clock();
	int parity(uint16_t data);
};

