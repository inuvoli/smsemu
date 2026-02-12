#include "mapper.h"
#include "mappercodemaster.h"

MapperCodemaster::MapperCodemaster(uint8_t* cROM, uint8_t* cRAM)
{
}

MapperCodemaster::~MapperCodemaster()
{
}

bool MapperCodemaster::readMap(uint16_t addr, uint8_t& data)
{
	return false;
}

bool MapperCodemaster::writeMap(uint16_t addr, uint8_t data)
{
	return false;
}
