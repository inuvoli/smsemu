#include <loguru.hpp>
#include "controller.h"
#include "sms.h"

Controller::Controller()
{
	//Port are Active LOW!
	regDC = 0xff;
	regDD = 0xff;
	reg3F = 0xff;
}

Controller::~Controller()
{
}

void Controller::SetRegion(ConsoleRegion cntRegion)
{
	controllerRegion = cntRegion;
}

void Controller::setButtonState(int controllerIndex, ControllerButton button, bool pressed)
{
	switch (button)
	{
		case ControllerButton::UP:
			switch (controllerIndex)
			{
			case 0:
				regDC = (regDC & ~0x01) + (pressed ? 0 : 0x01);
				LOG_F(2, "CONTROLLER - Controller 1 UP %s", pressed ? "Pressed" : "Released");
				break;
			case 1:
				regDC = (regDC & ~0x40) + (pressed ? 0 : 0x40);
				LOG_F(2, "CONTROLLER - Controller 2 UP %s", pressed ? "Pressed" : "Released");
				break;
			}
			break;

		case ControllerButton::DOWN:
			switch (controllerIndex)
			{
			case 0:
				regDC = (regDC & ~0x02) + (pressed ? 0 : 0x02);
				LOG_F(2, "CONTROLLER - Controller 1 DOWN %s", pressed ? "Pressed" : "Released");
				break;
			case 1:
				regDC = (regDC & ~0x80) + (pressed ? 0 : 0x80);
				LOG_F(2, "CONTROLLER - Controller 2 DOWN %s", pressed ? "Pressed" : "Released");
				break;
			}
			break;

		case ControllerButton::LEFT:
			switch (controllerIndex)
			{
			case 0:
				regDC = (regDC & ~0x04) + (pressed ? 0 : 0x04);
				LOG_F(2, "CONTROLLER - Controller 1 LEFT %s", pressed ? "Pressed" : "Released");
				break;
			case 1:
				regDD = (regDD & ~0x01) + (pressed ? 0 : 0x01);
				LOG_F(2, "CONTROLLER - Controller 2 LEFT %s", pressed ? "Pressed" : "Released");
				break;
			}
			break;

		case ControllerButton::RIGHT:
			switch (controllerIndex)
			{
			case 0:
				regDC = (regDC & ~0x08) + (pressed ? 0 : 0x08);
				LOG_F(2, "CONTROLLER - Controller 1 RIGHT %s", pressed ? "Pressed" : "Released");
				break;
			case 1:
				regDD = (regDD & ~0x02) + (pressed ? 0 : 0x02);
				LOG_F(2, "CONTROLLER - Controller 2 RIGHT %s", pressed ? "Pressed" : "Released");
				break;
			}
			break;

		case ControllerButton::S1:
			switch (controllerIndex)
			{
			case 0:
				regDC = (regDC & ~0x10) + (pressed ? 0 : 0x10);
				LOG_F(2, "CONTROLLER - Controller 1 S1 %s", pressed ? "Pressed" : "Released");
				break;
			case 1:
				regDD = (regDD & ~0x04) + (pressed ? 0 : 0x04);
				LOG_F(2, "CONTROLLER - Controller 2 S1 %s", pressed ? "Pressed" : "Released");
				break;
			}
			break;

		case ControllerButton::S2:
			switch (controllerIndex)
			{
			case 0:
				regDC = (regDC & ~0x20) + (pressed ? 0 : 0x20);
				LOG_F(2, "CONTROLLER - Controller 1 S2 %s", pressed ? "Pressed" : "Released");
				break;
			case 1:
				regDD = (regDD & ~0x08) + (pressed ? 0 : 0x08);
				LOG_F(2, "CONTROLLER - Controller 2 S2 %s", pressed ? "Pressed" : "Released");
				break;
			}
			break;

		default:
			break;
	}
}

bool Controller::read(uint8_t addr, uint8_t& data)
{
	switch (addr)
	{
	case 0x01: data = reg3F; break;
	case 0xc0: data = regDC; break;
	case 0xc1: data = regDD; break;
	}
	
	return true;
}

bool Controller::write(uint8_t addr, uint8_t data)
{
	uint8_t out_bit;

	//Nationalization Control
	reg3F = data;

	//Set PortB according to Regional Settings
	switch (controllerRegion)
	{
		case ConsoleRegion::JP:
			//SMS is Japanese
			out_bit = (reg3F & 0x80) + ((reg3F & 0x20) << 1);
			out_bit = ~out_bit;
			out_bit &= 0xc0;
			regDD = (regDD & 0x3f) + out_bit;
			break;
		case ConsoleRegion::US:
		case ConsoleRegion::EU:
			//SMS is American or European
			out_bit = (reg3F & 0x80) + ((reg3F & 0x20) << 1);
			out_bit &= 0xc0;
			regDD = (regDD & 0x3f) + out_bit;
			break;
	}

	LOG_F(1, "CNT - write: addr (0x%02x), data (0x%02x)", addr, data);

	return true;
}
