#include "psg.h"
#include "sms.h"

float attn_table[16] = { 1.0000f, 0.7943f, 0.6310f, 0.5012f, 0.3981f, 0.3162f, 0.2512f,
						0.1995f, 0.1585f, 0.1259f, 0.1000f, 0.0794f, 0.0631f, 0.0501f,
						0.0398f, 0.000f };

PSG::PSG()
{
	m_nClockCounter = 0;
	m_nSamplePerFrame = 0;
	m_lpfFilter = new Filter(LPF, 51, (double)(44100 / 1000), 20);
}

PSG::~PSG()
{
	delete m_lpfFilter;
}

bool PSG::read(uint8_t addr, uint8_t& data)
{
	return true;
}

bool PSG::write(uint8_t addr, uint8_t data)
{
	static int nLatchedChannel;
	static int nLatchedMode;
	

	if (data >= 0x80)		
	{
		//bit7 = 1
		nLatchedChannel = (data >> 5) & 0x03;
		nLatchedMode = (data >> 4) & 0x01;

		uint8_t reg_addr = (data >> 4) & 0x07;
		switch (reg_addr)
		{
		case 0b000:
			m_tone[0].nFrequency = (m_tone[0].nFrequency & 0xfff0) | (data & 0x0f);
			break;
		case 0b001:
			m_tone[0].fAttenuation = attn_table[data & 0x0f];
			break;
		case 0b010:
			m_tone[1].nFrequency = (m_tone[1].nFrequency & 0xfff0) | (data & 0x0f);
			break;
		case 0b011:
			m_tone[1].fAttenuation = attn_table[data & 0x0f];
			break;
		case 0b100:
			m_tone[2].nFrequency = (m_tone[2].nFrequency & 0xfff0) | (data & 0x0f);
			m_noise.nTone2Freq = m_tone[2].nFrequency;
			break;
		case 0b101:
			m_tone[2].fAttenuation = attn_table[data & 0x0f];
		case 0b110:
			m_noise.nType = (data >> 2) & 0x01;
			m_noise.nFrequency = data & 0x03;
			if (m_noise.nType == 0)
				m_noise.nShiftReg = 0x8000;
			break;
		case 0b111:
			m_noise.fAttenuation = attn_table[data & 0x0f];
			break;
		}
	}
	else					
	{
		//bit7 = 0
		if (nLatchedMode == 1)	
		{
			//Volume
			switch (nLatchedChannel)
			{
			case 0: m_tone[0].fAttenuation = attn_table[data & 0x0f]; break;
			case 1: m_tone[1].fAttenuation = attn_table[data & 0x0f]; break;
			case 2: m_tone[2].fAttenuation = attn_table[data & 0x0f]; break;
			case 3: m_noise.fAttenuation = attn_table[data & 0x0f]; break;
			}
		}
		else	
		{
			//Tone
			switch (nLatchedChannel)
			{
			case 0: m_tone[0].nFrequency = (m_tone[0].nFrequency & 0x000f) | (data & 0x3f) << 4; break;
			case 1: m_tone[1].nFrequency = (m_tone[1].nFrequency & 0x000f) | (data & 0x3f) << 4; break;
			case 2:
				m_tone[2].nFrequency = (m_tone[2].nFrequency & 0x000f) | (data & 0x3f) << 4;
				m_noise.nTone2Freq = m_tone[2].nFrequency;
				break;
			case 3:
				m_noise.nType = (data >> 2) & 0x01;
				m_noise.nFrequency = data & 0x03;
				if (m_noise.nType == 0)
					m_noise.nShiftReg = 0x8000;
				break;
			}
		}
	}
	return true;
}

bool PSG::reset()
{
	m_nClockCounter = 0;	
	return true;
}

bool PSG::clock()
{
	float fSampleIn, fSampleOut;
			
	if ((m_nClockCounter % 16) == 0)
	{
		m_tone[0].clock();
		m_tone[1].clock();
		m_tone[2].clock();
		m_noise.clock();
	}

	//Sequencers samples frequency rate is:
	//PAL:  223012,5 Samples per Second
	//NTSC: 224010,0 Samples per Second
	//
	//In order to play audio on a PC it has to be converted to 44100 Samples per Second
	if ((m_nClockCounter % 81) == 0)
	{
		fSampleIn = m_tone[0].fSample * m_tone[0].fAttenuation +
			m_tone[1].fSample * m_tone[1].fAttenuation +
			m_tone[2].fSample * m_tone[2].fAttenuation +
			m_noise.fSample * m_noise.fAttenuation;

		fSampleOut = m_lpfFilter->do_sample((double)fSampleIn);
		m_audioBuffer.PutSample(fSampleOut * mixerout_attn);

		m_nSamplePerFrame++;
	}

	m_nClockCounter++;
	return true;
}

float PSG::GetSample()
{
	return m_audioBuffer.GetSample();
}

int PSG::GetSamplePerFrame()
{
	int nResult = m_nSamplePerFrame;
	m_nSamplePerFrame = 0;

	return nResult;
}
