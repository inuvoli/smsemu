#pragma once
#include <cstdint>

class SMS;

class DebugConsole
{
public:
	DebugConsole();
	~DebugConsole();

public:
	void ConnectBus(SMS* n) { sms = n; }
	bool read(uint8_t addr, uint8_t& data);
	bool write(uint8_t addr, uint8_t data);

private:
	//Pointer to BUS Object
	SMS* sms = nullptr;
};

