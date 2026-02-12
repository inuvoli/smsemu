#include "mapper.h"

Mapper::Mapper()
{
	pROM = NULL;
	pRAM = NULL;
}

//Mapper::Mapper(uint8_t* cROM, uint8_t* cRAM)
//{
//	//Connect Cartrige ROM and RAM to Mapper
//	pROM = cROM;
//	pRAM = cRAM;
//}

Mapper::~Mapper()
{
}
