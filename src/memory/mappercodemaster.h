#pragma once
#include "mapper.h"

class MapperCodemaster : public Mapper
{
public:
	MapperCodemaster(uint8_t* cROM, uint8_t* cRAM);
	~MapperCodemaster();

	//Virtual Members, defined by derived class for each Mapper implementation
	bool readMap(uint16_t addr, uint8_t& data) override;
	bool writeMap(uint16_t addr, uint8_t data) override;
};

