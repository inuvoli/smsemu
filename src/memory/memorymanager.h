#pragma once
#include<cstdint>

class SMS;

class MemoryManager
{
public:
	MemoryManager();
	~MemoryManager();

public:
	void ConnectBus(SMS* n) { sms = n; }
	bool read(uint8_t addr, uint8_t& data);
	bool write(uint8_t addr, uint8_t data);

	bool SystemRamEnabled();

public:
	union {
		uint8_t b;
		struct {
			uint8_t : 2;			// Unused
			uint8_t io_en : 1;		// I/O Enable
			uint8_t bios_en : 1;	// BIOS ROM Enable
			uint8_t ram_en : 1;		// RAM Enable
			uint8_t card_en : 1;	// Card Slot Enable
			uint8_t cart_en : 1;	// Cartridge Slot Enable
			uint8_t exp_en : 1;		// Expansion Slot Enable
		};
	} reg;

private:
	//Pointer to BUS Object
	SMS* sms = nullptr;

	
};

