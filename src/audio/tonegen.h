#pragma once
#include <cstdint>

struct ToneGen
{
	uint16_t nFrequency = 0;
	uint16_t nCounter = 0;
	
	float fSample = 0.0f;
	float fAttenuation = 0.0f;

	bool clock();
};

