#include "noisegen.h"

bool NoiseGen::clock()
{
	static bool bShift;
	
	nCounter--;

	if (nCounter == 0)
	{
		switch (nFrequency)
		{
		case 0: nCounter = 0x10; break;
		case 1: nCounter = 0x20; break;
		case 2: nCounter = 0x40; break;
		case 3: nCounter = nTone2Freq; break;
		}
		bShift = !bShift;
	}

	if (bShift)	
	{
		if (nType == 1)	
		{
			//White Noise
			nShiftReg = (nShiftReg >> 1) | (parity(nShiftReg & nTapBit) << 15);
		}
		else           
		{
			//Periodic Noise
			nShiftReg = (nShiftReg >> 1) | ((nShiftReg & 0x0001) << 15);
		}

		fSample = (float)(nShiftReg & 0x0001);
	}
	return true;
}

int NoiseGen::parity(uint16_t data)
{
	//Check Parity on one Word in Even

	//return 0 for even number of 1's
	//return 1 for odd number of 1's
	data ^= data >> 4;
	data &= 0xf;
	return !((0x6996 >> data) & 1);
}
