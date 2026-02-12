#pragma once
#include <cstdint>
#include <array>

#include "z80a.h"
#include "bios.h"
#include "mapper.h"
#include "cartridge.h"
#include "psg.h"
#include "vdp.h"
#include "controller.h"
#include "memorymanager.h"
#include "debugconsole.h"
#include "emuconst.h"
#include "commandline.h"

class SMS
{
public:
	SMS(ConsoleRegion region, ConsoleMapper mapper, std::string filename);
	~SMS();
	
	//Connected Devices
	Z80A cpu;									//Z80A CPU
	VDP vdp;									//VDP Video Device Processor
	PSG psg;									//PSG Programmable Sound Generator
	Controller cnt;								//Controller A & B
	DebugConsole dbg;							//Debug Console
	MemoryManager mem;							//Memory Manager
	
	std::map<uint16_t, std::string> mapAsm;		//Disassembler

	std::shared_ptr<Bios> bios;					//Bios ROM - Always in Page 0, 0000h - 1fffh 
	std::shared_ptr<Cartridge> cart;			//Cartridge ROM, 48K from 0000h - bfffh 							
	std::array<uint8_t, 8 * 1024> ram;			//System 8K RAM (C000h - DFFFh, Mirrored at E000h - FFFFh)
	uint64_t masterclock_cycles;
	float frameDuration;
	
private:
	bool bCartInserted;

public:
	//Read & Write from Memory (Emulate MREQ active low)
	uint8_t readMem(uint16_t addr);
	void writeMem(uint16_t addr, uint8_t data);

	//Read & Write from I/O Ports (Emulate IORQ active low)
	uint8_t readIO(uint8_t addr);
	void writeIO(uint8_t addr, uint8_t data);

	bool SetRegion(ConsoleRegion region);
	bool InsertCartridge();
	bool RemoveCartridge();
	bool NewFrame();
	bool reset();
	bool reset(uint16_t org);
	bool clock();
	float GetFrameDuration() const { return frameDuration; }
};

