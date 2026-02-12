#pragma once
#include <cstdint>
#include <array>

class BitplaneShifter
{
public:
	BitplaneShifter(uint8_t nBplNumber);
	~BitplaneShifter();

public:
	bool LoadBitplane(uint8_t bitplane, uint8_t data);
	uint8_t GetPixelColor();

private:
	uint8_t* m_pBplArray = NULL;
	uint8_t  m_nBplNumber = 0;
};
