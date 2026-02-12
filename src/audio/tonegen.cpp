#include "tonegen.h"

bool ToneGen::clock()
{
	if (nFrequency <= 1)
	{
		fSample = 1.0f;
	}
	else
	{
		nCounter--;

		if (nCounter == 0)
		{
			nCounter = nFrequency;
			(fSample == 0.0f) ? fSample = 1.0f : fSample = 0.0f;
		}
	}
	
	return true;
}