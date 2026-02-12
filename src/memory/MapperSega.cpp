#include <loguru.hpp>
#include "mapper.h"
#include "mappersega.h"

MapperSega::MapperSega(uint8_t* cROM, uint8_t* cRAM)
{
	//Initialize POinter to Cartridge ROM & RAM
	pROM = cROM;
	pRAM = cRAM;

	//Reset Mapper Control Registers
	mapper_control[0] = 0x00;	//FFFCh - Cartridge RAM Mapper Control
	mapper_control[1] = 0x00;	//FFFDh - Mapper Slot 0 Control
	mapper_control[2] = 0x01;	//FFFEh - Mapper Slot 1 Control
	mapper_control[3] = 0x02;	//FFFFh - Mapper Slot 2 Control
}

MapperSega::~MapperSega()
{
}

bool MapperSega::readMap(uint16_t addr, uint8_t& data)
{
	uint32_t mapped_addr;

	//Map Address according to Mapper Control Register
	if (addr >= 0x0000 && addr <= 0x03ff)
	{
		//First 1K is never Mapped, contains interrupt routines
		data = pROM[addr];
	}
	else if (addr >= 0x0400 && addr <= 0x3fff)
	{
		//ROM Slot 0
		mapped_addr = addr + mapper_control[1] * 0x4000;
		data = pROM[mapped_addr];

	}
	else if (addr >= 0x4000 && addr <= 0x7fff)
	{
		//ROM Slot 1
		mapped_addr = (addr - 0x4000) + (mapper_control[2] * 0x4000);
		data = pROM[mapped_addr];
	}
	else if (addr >= 0x8000 && addr <= 0xbfff)
	{
		//Check if reads from ROM Slot 2 or RAM Slot 2
		if ((mapper_control[0] & 0x08) == 1)
		{
			//Check Cartridge RAM Bank Selection
			if ((mapper_control[0] & 0x04) == 0)
			{
				//Slot 2 is First Bank of 16K
				data = pRAM[addr & 0x3fff];
			}
			else
			{
				//Slot 2 is Second Bank of 16K
				data = pRAM[0x4000 + (addr & 0x3fff)];
			}
		}
		else
		{
			//Read from Cartridge ROM
			mapped_addr = (addr - 0x8000) + mapper_control[3] * 0x4000;
			data = pROM[mapped_addr];
		}
	}
	else if (addr >= 0xc000 && addr <= 0xffff)
	{
		//Check if RAM Slot 3 is enabled
		if ((mapper_control[0] & 0x10) == 1)
		{
			//Check Cartridge RAM Bank Selection
			if ((mapper_control[0] & 0x04) == 0)
			{
				//Slot 3 is Second Bank of 16K
				data = pRAM[0x4000 + (addr & 0x3fff)];
			}
			else
			{
				//Slot 3 is First Bank of 16K
				data = pRAM[addr & 0x3fff];
			}
		}
	}

	return true;
}

bool MapperSega::writeMap(uint16_t addr, uint8_t data)
{
	if (addr >= 0x8000 && addr <= 0xbfff)
	{
		//Check if RAM Slot 2 is enabled - (0x8000 - 0xbfff)
		if ((mapper_control[0] & 0x08) == 1)
		{
			//Check Cartridge RAM Bank Selection
			if ((mapper_control[0] & 0x04) == 0)
			{
				//Slot 2 is First Bank of 16K
				pRAM[addr & 0x3fff] = data;
			}
			else
			{
				//Slot 2 is Second Bank of 16K
				pRAM[0x4000 + (addr & 0x3fff)] = data;
			}
		}
	}
	else if (addr >= 0xc000 && addr <= 0xffff)
	{
		//Check if RAM Slot 3 is enabled - (0xc000 - 0xffff)
		if ((mapper_control[0] & 0x10) == 1)
		{
			//Check Cartridge RAM Bank Selection
			if ((mapper_control[0] & 0x04) == 0)
			{
				//Slot 3 is Second Bank of 16K
				pRAM[0x4000 + (addr & 0x3fff)] = data;
			}
			else
			{
				//Slot 3 is First Bank of 16K
				pRAM[addr & 0x3fff] = data;
			}
		}
	}

	//Write to Mapper Control Registers mapped in memory at:
	//FFFCh - Cartridge RAM Mapper Control
	//FFFDh - Mapper Slot 0 Control
	//FFFEh - Mapper Slot 1 Control
	//FFFFh - Mapper Slot 2 Control
	if (addr >= 0xfffc && addr <= 0xffff)
	{
		switch (addr)
		{
		case 0xfffc: mapper_control[0] = data; break;
		case 0xfffd: mapper_control[1] = data; break;
		case 0xfffe: mapper_control[2] = data; break;
		case 0xffff: mapper_control[3] = data; break;
		}
		LOG_F(1, "MPR - write: fffc (%d), fffd (%d), fffe (%d), ffff (%d)", mapper_control[0], mapper_control[1], mapper_control[2], mapper_control[3]);
	}

	return true;
}
