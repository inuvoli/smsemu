#pragma once
#include <cstdint>

#include "emuconst.h"

enum class ControllerButton : uint8_t
{
	UP = 0x0,
	DOWN = 0x1,
	LEFT = 0x2,
	RIGHT = 0x3,
	S1 = 0x4,
	S2 = 0x5,
};

class SMS;

class Controller
{
public:
	Controller();
	~Controller();

public:
	void ConnectBus(SMS* n) { sms = n; }
	bool read(uint8_t addr, uint8_t& data);
	bool write(uint8_t addr, uint8_t data);
	void SetRegion(ConsoleRegion cntRegion);
	void setButtonState(int controllerIndex, ControllerButton button, bool pressed);

public:
	uint8_t regDC;          //$dc |P2 DN|P2 UP|P1 S2|P1 S1|P1 RT|P1 LF|P1 DN|P1 UP|
	uint8_t regDD;			//$dd |P2 TH|P1 TH|  X  |RESET|P2 S2|P2 S1|P2 RT|P2 LF|
	uint8_t reg3F;			//$3f |P2 TRIG OUT LVL|P2 TH OUT LVL|P1 TRIG OUT LVL|P1 TH OUT LVL|P2 TRIG DATA DIR|P2 TH DATA DIR|P1 TRIG DATA DIR|P1 TH DATA DIR|

private:
	//Pointer to BUS Object
	SMS* sms = nullptr;

	//Define Region (JP, US od EU)
	ConsoleRegion controllerRegion;
};

