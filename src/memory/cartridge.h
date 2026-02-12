#pragma once
#include <cstdint>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <array>
#include <memory>

#include "emuconst.h"
#include "mapper.h"
#include "mappersega.h"
#include "mappercodemaster.h"

class Cartridge
{
public:
	Cartridge(const std::string& filename, ConsoleMapper mapper);
	~Cartridge();

private:
	//Cartridge ROM & RAM Memory
	uint8_t* cROM;
	uint8_t* cRAM;

	//Memory Mapper
	std::shared_ptr<Mapper> pMapper;

	//ROM Header
	std::array<uint8_t, 11> SegaString = { };
	uint16_t checksum;
	uint16_t serialnumber;
	uint8_t version;
	uint8_t sizecode;

public:
	bool read(uint16_t addr, uint8_t &data);
	bool write(uint16_t addr, uint8_t data);
};

