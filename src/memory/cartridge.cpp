#include <loguru.hpp>
#include "cartridge.h"

Cartridge::Cartridge(const std::string& filename, ConsoleMapper mapper)
{
	uint32_t romsize;
	uint16_t header_addr;
		
	//Load ROM Image
	std::ifstream ifs;
	ifs.open(filename, std::ifstream::binary);
	if (ifs.is_open())
	{
		ifs.seekg(0, ifs.end);
		romsize = ifs.tellg();
		ifs.seekg(0, ifs.beg);
		
		uint8_t rompages = (romsize / 16384);
		if (rompages < 3)
			rompages = 3;

		//cROM.resize(rompages * 16384);
		//ifs.read((char*)cROM.data(), cROM.size());
		cROM = new uint8_t[rompages * 16384];
		ifs.read((char*)cROM, rompages * 16384);
		ifs.close();
		LOG_F(INFO, "CRT - Game File Loaded: %s, Size: %d bytes, Pages: %d", filename.c_str(), romsize, rompages);
	}
	else
	{
		LOG_F(ERROR, "Failed to load ROM: %s", filename.c_str());
	}
	
	//Load Header Data
	//Check ROM Size to determine the Header Position
	// 8k  --> Header at 0x1ff0
	// 16k --> Header at 0x3ff0
	// 32k --> Header at 0x7ff0
	switch (romsize)
	{
	case 0x2000: header_addr = 0x1ff0; break;
	case 0x4000: header_addr = 0x3ff0; break;
	default:	 header_addr = 0x7ff0;
	}

	//Read Header info from ROM
	for (int i=0; i < 10; i++)
		SegaString[i] = cROM[header_addr + i];

	checksum = cROM[header_addr + 0x000a] + (cROM[header_addr + 0x000b] << 8);
	serialnumber = cROM[header_addr + 0x000c] + (cROM[header_addr + 0x000d] << 8);
	version = cROM[header_addr + 0x000e];
	sizecode = cROM[header_addr + 0x000f];

	//Initialize RAM Memory, assume 32K for now
	//cRAM.resize(1024 * 32);
	cRAM = new uint8_t[1024 * 32];

	//Initialize Memory Mapper
	switch (mapper)
	{
	case ConsoleMapper::SEGA:		pMapper = std::make_shared<MapperSega>(cROM, cRAM); break;
	case ConsoleMapper::CODEMASTER:	pMapper = std::make_shared<MapperCodemaster>(cROM, cRAM); break;
	}
}

Cartridge::~Cartridge()
{
	delete[] cROM;
	delete[] cRAM;
}

bool Cartridge::read(uint16_t addr, uint8_t& data)
{
	pMapper->readMap(addr, data);

	return true;
}

bool Cartridge::write(uint16_t addr, uint8_t data)
{
	pMapper->writeMap(addr, data);

	return true;
}

