#pragma once
#include <cstdint>

#include "tonegen.h"
#include "noisegen.h"
#include "filt.h"
#include "circularbuffer.h"

class SMS;

constexpr auto tone_number = 3;
constexpr auto mixerout_attn = 0.25f;

class PSG
{
public:
	PSG();
	~PSG();

	enum VIDEOMODE
	{
		PAL = 0,
		NTSC = 1
	};

public:
	void ConnectBus(SMS* n) { sms = n; }
	bool read(uint8_t addr, uint8_t& data);
	bool write(uint8_t addr, uint8_t data);
	bool reset();
	bool clock();
	float GetSample();
	int GetSamplePerFrame();
	
private:
	//Pointer to SMS Object
	SMS* sms = nullptr;
	uint32_t m_nClockCounter;
	uint32_t	m_nSamplePerFrame;
		
	ToneGen		m_tone[tone_number];
	NoiseGen	m_noise;
	Filter*		m_lpfFilter;

	CircularBuffer m_audioBuffer{ 0x1000 };
};

