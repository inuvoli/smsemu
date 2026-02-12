#include "memorymanager.h"
#include "sms.h"

MemoryManager::MemoryManager()
{
	reg.b = 0x00;
}

MemoryManager::~MemoryManager()
{
}

bool MemoryManager::read(uint8_t addr, uint8_t& data)
{
	data = reg.b;
	return true;
}

bool MemoryManager::write(uint8_t addr, uint8_t data)
{
	reg.b = data;
	return true;
}

bool MemoryManager::SystemRamEnabled()
{
	if (reg.ram_en == 0)
		return true;
	else
		return false;
}
