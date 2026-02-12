#include "sms.h"

SMS::SMS(ConsoleRegion region, ConsoleMapper mapper, std::string filename)
{
	//Start with no Cartridge inserted, run from BIOS
	bCartInserted = false;

	//Init MasterClock
	masterclock_cycles = 0;

	//Connect to CPU
	cpu.ConnectBus(this);

	//Connect to VDP
	vdp.ConnectBus(this);

	//Connect to PSG
	psg.ConnectBus(this);

	//Connect to Controller
	cnt.ConnectBus(this);

	//Connect to Controller
	dbg.ConnectBus(this);

	//Load Bios ROM into memory
	//bios = std::make_shared<Bios>("./ROM/bios13fx.bin");
	//LoadBios();

	//Load Cartridge ROM
	cart = std::make_shared<Cartridge>(filename, mapper);
	InsertCartridge();

	//Set Sega Master System Region
	SetRegion(region);

	//Disassemble code
	mapAsm = cpu.disassemble(0x0000, 0xbfff);
}

SMS::~SMS ()
{

}

bool SMS::SetRegion(ConsoleRegion region)
{
	
	//Set Controller Region
	cnt.SetRegion(region);

	//Set VDP Region Configuration
	switch(region)
	{
		case ConsoleRegion::JP:
			vdp.SetVideoStandard(VDP::NTSC);
			frameDuration = 1.0f / 60.0f;
			break;
		case ConsoleRegion::US:
			vdp.SetVideoStandard(VDP::NTSC);
			frameDuration = 1.0f / 60.0f;
			break;
		case ConsoleRegion::EU:
			vdp.SetVideoStandard(VDP::PAL);
			frameDuration = 1.0f / 50.0f;	
			break;
	}
	
	return true;
}
uint8_t SMS::readMem(uint16_t addr)
{
	uint8_t data = 0x00;

	//Read from Cartridge ROM Slot 0, 1 and 2 or Cartridge RAM Slot 2
	//Alteranively read from Sytem BIOS ROM
	if (addr >= 0x0000 && addr <= 0xbfff)
	{
		if (bCartInserted)
			cart->read(addr, data);
		else
			bios->read(addr, data);
		return data;
	}

	//Read from 8K System RAM if RAM is Enabled (Active Low!!!)
	//Else read from Cartridge RAM at 0xc000 - 0xffff. Bit 4 Must be Set on Register 0xfffc
	if (addr >= 0xc000 && addr <= 0xffff)
	{
		if (mem.SystemRamEnabled())
		{
			//Read from System 8K Memory (Mirroring address)
			return ram[addr & 0x1fff];
		}
		else
		{
			//Read from Cartridge RAM Slot 3
			if (bCartInserted)
				cart->read(addr, data);
			return data;
		}
	}

	return data;
}

void SMS::writeMem(uint16_t addr, uint8_t data)
{
	//Write to Cartridge RAM Slot 2 (0x8000 - 0xbfff)
	//Bit 3 Must be Set on Register 0xfffc to override Cartridge Slot 2
	if (addr >= 0x8000 && addr <= 0xbfff)
	{
		if (bCartInserted)
			cart->write(addr, data);
	}

	//Write to 8K System RAM or Cartridge RAM Slot 3
	if (addr >= 0xc000 && addr <= 0xffff)
	{
		if (mem.SystemRamEnabled())
		{
			//Write to System 8K Memory (Mirroring address)
			ram[addr & 0x1fff] = data;
		}
		else
		{
			//Write to Cartridge RAM Slot 3
			//Bit 4 Must be Set on Register 0xfffc
			if (bCartInserted)
				cart->write(addr, data);
		}
	}

	//Write to Cartridge Mapper Control Register
	//Note that data is written on Mapper Registers and RAM too
	if (addr >= 0xfffc && addr <= 0xffff)
	{
		if (bCartInserted)
			cart->write(addr, data);
	}
}

uint8_t SMS::readIO(uint8_t addr)
{
	uint8_t data = 0x00;
	uint8_t mirror_addr = addr & 0xc1;

	//Only bit 7, 6 and 0 are used causing Port Mirroring
	switch (mirror_addr)
	{
	case 0x00:
		//Memory Enables - All Mirror of 0x3e
		mem.read(mirror_addr, data);
		break;
	case 0x40:
		//V Counter - All Mirror of 0x7e
		vdp.read(mirror_addr, data);
		break;
	case 0x41:
		//H Counter - All Mirror of 0x7f
		vdp.read(mirror_addr, data);
		break;
	case 0x80:
		//VDP Data Port - All Mirror of 0xbe
		vdp.read(mirror_addr, data);
		break;
	case 0x81:
		//VDP Control Port - All Mirror of 0xbf
		vdp.read(mirror_addr, data);
		break;
	case 0xc0:
		//Joystick Port - All Mirror of 0xdc
		cnt.read(mirror_addr, data);
		break;
	case 0xc1:
		//Joystick Port - All Mirror of 0xdd
		cnt.read(mirror_addr, data);
		break;
	}

	return data;
}

void SMS::writeIO(uint8_t addr, uint8_t data)
{
	uint8_t mirror_addr = addr & 0xc1;

	//Only bit 7, 6 and 0 are used causing Port Mirroring
	switch (mirror_addr)
	{
	case 0x00:
		//Memory Enables - All Mirror of 0x3e
		mem.write(mirror_addr, data);
		break;
	case 0x01:
		//Joystick Port Control - All Mirror of 0x3f
		cnt.write(mirror_addr, data);
		break;
	case 0x40:
		//PSG - SN76480 - All Mirror of 0x7e
		psg.write(mirror_addr, data);
		break;
	case 0x41:
		//PSG - SN76480 - All Mirror of 0x7f 
		psg.write(mirror_addr, data);
		break;
	case 0x80:
		//VDP Data Port - All Mirror of 0xbe
		vdp.write(mirror_addr, data);
		break;
	case 0x81:
		//VDP Control Port - All Mirror of 0xbf
		vdp.write(mirror_addr, data);
		break;
	}

	return;
}

bool SMS::InsertCartridge()
{
	bCartInserted = true;
	
	return true;
}

bool SMS::RemoveCartridge()
{
	bCartInserted = false;
	
	return true;
}

bool SMS::NewFrame()
{
	do
	{
		clock();
	} while (!vdp.bFrameComplete);
	vdp.bFrameComplete = false;
		
	return true;
}

bool SMS::reset()
{
	masterclock_cycles = 0;

	vdp.reset();
	cpu.reset();
	psg.reset();

	return true;
}

bool SMS::reset(uint16_t org)
{
	masterclock_cycles = 0;
	
	vdp.reset();
	cpu.reset();
	psg.reset();
		 
	cpu.pc.w = org;

	return true;
}

bool SMS::clock()
{
	////////////////////////////////////////////////////////////////////////////////////////////
	// Master Clock is the actual clock of the VDP
	// NTSC = 10.738635 MHz
	// PAL  = 10.640685 MHz
	//
	// The VDP is clocked at 1/2 the master clock frequency
	// The CPU is clocked at 1/3 the master clock frequency
	// The PSG is clocked at 1/3 the master clock frequency
	//
	// Realtime simulation depends on the Frame per second which are different for NTSC and PAL
	//
	// NTSC = ~60 frame per second
	// PAL  = ~50 frame per second
	///////////////////////////////////////////////////////////////////////////////////////////

	(masterclock_cycles % 2) ? 0 : vdp.clock();
	(masterclock_cycles % 3) ? 0 : cpu.clock();
	(masterclock_cycles % 3) ? 0 : psg.clock();
	
	masterclock_cycles++;

	return true;
}
