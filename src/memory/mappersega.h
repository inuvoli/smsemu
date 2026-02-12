#pragma once
#include "mapper.h"

class MapperSega : public Mapper
{
public:
	MapperSega(uint8_t* cROM, uint8_t* cRAM);
	~MapperSega();

	//Virtual Members, defined by derived class for each Mapper implementation
	bool readMap(uint16_t addr, uint8_t& data) override;
	bool writeMap(uint16_t addr, uint8_t data) override;

public:
	//Mapper Registers
	std::array<uint8_t, 4> mapper_control;
};

