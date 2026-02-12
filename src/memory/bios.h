#pragma once
#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include <array>

class Bios
{
public:
	Bios(const std::string& sFileName);
	Bios(const std::string& sFileName, uint16_t org);
	~Bios();

private:
	//BIOS ROM Memory
	std::vector<uint8_t> bROM;

public:
	bool read(uint16_t addr, uint8_t& data);
	bool write(uint16_t addr, uint8_t data);


};

