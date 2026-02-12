#include "bios.h"

Bios::Bios(const std::string& sFileName)
{
	//Read ROM Image
	std::ifstream ifs;
	ifs.open(sFileName, std::ifstream::binary);
	if (ifs.is_open())
	{
		bROM.resize(48 * 1024);
		ifs.read((char*)bROM.data(), bROM.size());
	}
	ifs.close();
}

Bios::Bios(const std::string& sFileName, uint16_t org)
{
	//Read ROM Image
	std::ifstream ifs;
	ifs.open(sFileName, std::ifstream::binary);
	if (ifs.is_open())
	{
		ifs.seekg(0, ifs.end);
		uint32_t romsize = ifs.tellg();
		ifs.seekg(0, ifs.beg);

		bROM.resize(romsize);
		ifs.read((char*)bROM.data(), bROM.size());

		for (uint32_t i = 0; i < org; i++)
			bROM.insert(bROM.begin(), 0x00);

		for (uint32_t j = 0; j < (0xc000 - org - romsize); j++)
			bROM.insert(bROM.end(), 0x00);

	}
	ifs.close();
}

Bios::~Bios()
{
}

bool Bios::read(uint16_t addr, uint8_t& data)
{
	data = bROM[addr];

	return true;
}

bool Bios::write(uint16_t addr, uint8_t data)
{
	bROM[addr] = data;

	return true;
}
