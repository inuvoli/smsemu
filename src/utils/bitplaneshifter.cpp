#include "bitplaneshifter.h"

////////////////////////////////////////////////////////////////////////////////
//
//                      Bitplane Shifter Class Implementation
//
////////////////////////////////////////////////////////////////////////////////
BitplaneShifter::BitplaneShifter(uint8_t nBplNumber)
{
	m_pBplArray = new uint8_t[nBplNumber];
	m_nBplNumber = nBplNumber;
}

BitplaneShifter::~BitplaneShifter()
{
	delete[] m_pBplArray;
}

bool BitplaneShifter::LoadBitplane(uint8_t bitplane, uint8_t data)
{
	if (m_pBplArray != NULL && bitplane <= m_nBplNumber)
	{
		m_pBplArray[bitplane] = data;
		return true;
	}
	else
	{
		return false;
	}
}

uint8_t BitplaneShifter::GetPixelColor()
{
	uint8_t color = 0;
	
	for (int i = 0; i <= m_nBplNumber; i++)
	{
		color += (m_pBplArray[i] >> 7) * (1 << i);
		m_pBplArray[i] <<= 1;
	}

	return color;
}
