#include <loguru.hpp>
#include "debugconsole.h"
#include "sms.h"

DebugConsole::DebugConsole()
{
}

DebugConsole::~DebugConsole()
{
}

bool DebugConsole::read(uint8_t addr, uint8_t& data)
{
	return false;
}

bool DebugConsole::write(uint8_t addr, uint8_t data)
{
	LOG_F(2, "DBG - write: addr(%.2x) data(%.2x) char(%c)\n", addr, data, data);
	if (addr == 0xfd)
		LOG_F(2, "DBG - write: char(%c)\n", data);

	return true;
}
