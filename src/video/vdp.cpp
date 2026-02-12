#include "vdp.h"
#include "sms.h"


VDP::VDP()
{
	//Init Internal Status
	HCount = 0;
	VCount = 0;
	nFrameCounter = 0;
	nCycleCounter = 0;
	
	//Init Internat Registers
	reg0.b = 0x36;		//Mode Control #1
	reg1.b = 0xa0;		//Mode Control #2
	reg2 = 0xff;		//Screen Map Table Base
	reg3 = 0xff;		//Color Table BAse Address - Always 0xff
	reg4 = 0xff;		//Background Pattern Generator Base Address - Always 0xff
	reg5 = 0xff;		//Sprite Attributes Table Base Address
	reg6 = 0xfb;		//Sprite Pattern Generator Base Address
	reg7 = 0x00;		//Overscan Color
	reg8 = 0x00;		//Background X Scroll
	reg9 = 0x00;		//Background Y Scroll
	reg10 = 0xff;		//Line Counter for Line Interrupt

	status.b = 0x00;		//Status Register

	//Init Control Port & Data Port Variables 
	command_word = 0;
	code_reg = 0;
	addr_reg = 0;
	read_buf = 0x00;
		
	bFrameComplete = false;
	bFirstByteRecv = false;

	raster_counter = 0;
	active_period = 192;
	additional_scan = 0;

	pRenderBuffer = nullptr;
	pFrameBuffer = nullptr;
	pCharTable[0] = nullptr;
	pCharTable[1] = nullptr;

	//Autogenerate all 64 Color Palette Scaling RGB from 2bit to 8bit
	//Color Value Stored in CRAM is used as an Index for the Palette.
	//Note that in CRAM color are stores as 8 bit 00bbggrr
	//TODO - Need to be checked for Color accuracy
	for (uint8_t c = 0; c < 64; c++)
	{
		palScreen[c] = _rgba_to_int(((c >> 0) & 0x03) * 0x55, ((c >> 2) & 0x03) * 0x55, ((c >> 4) & 0x03) * 0x55, 0xff);
	}
}

VDP::~VDP()
{
	delete pRenderBuffer;
	delete pFrameBuffer;
	delete pCharTable[0];
	delete pCharTable[1];

}

////////////////////////////////////////////////////////////////////////////////
//
//                        Helper Functions
//
////////////////////////////////////////////////////////////////////////////////
//Swap bits in a unsigned char, used with Horizontal Mirroring 
inline uint8_t VDP::_bitswap_uint8(uint8_t x)
{
	return (x * 0x0202020202ULL & 0x010884422010ULL) % 1023;
}

inline uint32_t VDP::_rgba_to_int(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	uint32_t color = 0;

	color = (b << 0) | (g << 8) | (r << 16) | (a << 24);

	return color;
}

////////////////////////////////////////////////////////////////////////////////
//
//                      External IO Interface Functions
//
////////////////////////////////////////////////////////////////////////////////
bool VDP::read(uint8_t addr, uint8_t& data)
{
	//Get VCount according to Video Mode and Resolution 
	auto getVCountByte = [&]()
	{
		uint8_t delta = scanline_number - 256U;

		if (video_std == 0)
		{
			if (active_period == 192)
				return (VCount > 0xf2) ? (uint8_t)(VCount - delta) : (uint8_t)VCount;
			if (active_period == 224)
				return (VCount > 0x102) ? (uint8_t)(VCount - delta) : (uint8_t)VCount;
			if (active_period == 240)
				return (VCount > 0x10a) ? (uint8_t)(VCount - delta) : (uint8_t)VCount;
		}

		if (video_std == 1)
		{
			if (active_period == 192)
				return (VCount > 0xda) ? (uint8_t)(VCount - delta) : (uint8_t)VCount;
			if (active_period == 224)
				return (VCount > 0xea) ? (uint8_t)(VCount - delta) : (uint8_t)VCount;
		}

		return (uint8_t)0;
	};

	//Get HCount from counter
	auto getHCountByte = [&]()
	{
		//return (HCount <= 0xff) ? (uint8_t)HCount : 0xff;
		return (HCount & 0x1fe) >> 1;
	};

	switch (addr)
	{
	case 0x40:
		data = getVCountByte();;
		break;

	case 0x41:
		data = getHCountByte();
		break;

	case 0x80:
		readDataPort(data);
		break;

	case 0x81:
		readControlPort(data);
		break;
	}
		
	return true;
}

bool VDP::write(uint8_t addr, uint8_t data)
{
	switch(addr)
	{
	case 0x80:
		writeDataPort(data);
		break;

	case 0x81:
		writeControlPort(data);
	}

	return true;
}

bool VDP::reset()
{
	//Init Internal Status
	HCount = 0;
	VCount = 0;
	nFrameCounter = 0;
	nCycleCounter = 0;

	//Init Internat Registers
	reg0.b = 0x36;		//Mode Control #1
	reg1.b = 0xa0;		//Mode Control #2
	reg2 = 0xff;		//Screen Map Table Base
	reg3 = 0xff;		//Color Table BAse Address - Always 0xff
	reg4 = 0xff;		//Background Pattern Generator Base Address - Always 0xff
	reg5 = 0xff;		//Sprite Attrributes Table Base Address
	reg6 = 0xfb;		//Sprite Pattern Generator Base Address
	reg7 = 0x00;		//Overscan Color
	reg8 = 0x00;		//Background X Scroll
	reg9 = 0x00;		//Background Y Scroll
	reg10 = 0xff;		//Line Counter for Line Interrupt

	status.b = 0x00;		//Status Register

	//Init Control Port & Data Port Variables 
	command_word = 0;
	code_reg = 0;
	addr_reg = 0;
	read_buf = 0x00;

	bFrameComplete = false;
	bFirstByteRecv = false;

	raster_counter = 0;
	active_period = 192;

	return true;
}

bool VDP::clock()
{
	//Check whenever tha Raster Counter expire and Throw a Line Interrupt
	//Raster Counter is loaded with Reg10 outside Active Area or when
	//it's value underflow to 0xff. 
	auto checkRasterCounter = [&]()
	{
		if (VCount <= active_period)
		{
			//Decrement raster_counter at the begining of the scanline
			if (HCount == 0)
			{
				raster_counter--;
			}

			if (raster_counter == 0xff)
			{
				if (HCount == scanline_lenght - 1)
				{
					raster_counter = reg10;
				}

				return true;
			}
		}
		else
		{
			raster_counter = reg10;
		}
		return false;
	};
		
	//Assert CPU Maskable Interrupt INT if Line Interrupt is enabled  
	auto assertLineIrq = [&]()
	{
		if (reg0.ie1 == 1)
			sms->cpu.irq();

		return true;
	};

	//Assert CPU Maskable Interrupt INT if Frame Interrupt is enabled
	auto assertFrameIrq = [&]()
	{
		if (reg1.ie0 == 1)
			sms->cpu.irq();
						
		return true;
	};
	
	//Render the screen if Blank enabled else Blank the video
	if (reg1.blk)
	{
		if ((HCount < 256) & (VCount < (active_period + additional_scan)))
		{
			//Reset Backgroung to Reg7 Color
			pFrameBuffer->SetPixel(HCount, VCount, GetColorFromCRam(reg7 & 0x0f, 1));

			//Render Background Priority 0 Tiles
			RenderBackground(0);

			//Render Sprites
			RenderSprites();

			//Render Background Priority 1 Tiles
			RenderBackground(1);

			//Mask Column one if Reg0 - Bit5 is enabled
			MaskColumnOne();
		}
	}
	else
	{
		if ((HCount < 256) & (VCount < (active_period + additional_scan)))
		{
			pFrameBuffer->SetPixel(HCount, VCount, GetColorFromCRam(reg7 & 0x0f, 1));
		}
	}
	
	//HCount & VCount Loop
	//Each Scanline is 342 pixel long counting from 0..341
	//While going to the screen assert IRQ if necessary

	//Increment Vertical Counter
	if (HCount == scanline_lenght)
	{
		HCount = 0;

		VCount++;
		if (VCount == scanline_number)
		{
			//Start a New Frame
			bFrameComplete = true;
			VCount = 0;
			nFrameCounter++;
		}
	}

	//Exiting Active Area of the Screen
	if (VCount == active_period)
	{
		//Set VBlank Status Flag
		if (HCount == 303)
			status.vblank = 1;

		//Assert Frame IRQ
		if (HCount == 304)
			assertFrameIrq();
	}

	if (checkRasterCounter())
	{
		//Try to Call CPU Maskable Line Interrupt
		if (HCount == 304)
			assertLineIrq();
	}

	//Increment Horizontal Counter
	HCount++;
	nCycleCounter++;
		
	return true;
}

////////////////////////////////////////////////////////////////////////////////
//
//                      Video Output & Initialization Functions
//
////////////////////////////////////////////////////////////////////////////////
void VDP::SetVideoStandard(uint8_t mode)
{
	if (mode == PAL)
	{
		video_std = PAL;
		scanline_lenght = 342;
		scanline_number = 313;
		fFPS = 50;
		//fFPS = 49.701459f;
		//pFrameBuffer = new FrameBuffer(scanline_lenght, scanline_number);
		pFrameBuffer = new FrameBuffer(256, 240);
		pCharTable[0] = new FrameBuffer(128, 128);
		pCharTable[1] = new FrameBuffer(128, 128);
	}
	else if (mode == NTSC)
	{
		video_std = NTSC;
		scanline_lenght = 342;
		scanline_number = 262;
		fFPS = 60;
		//fFPS = 59.922743f;
		//pFrameBuffer = new FrameBuffer(scanline_lenght, scanline_number);
		pFrameBuffer = new FrameBuffer(256, 240);
		pCharTable[0] = new FrameBuffer(128, 128);
		pCharTable[1] = new FrameBuffer(128, 128);
	}
}

uint32_t* VDP::GetScreen()
{
	// Simply returns the current sprite holding the rendered screen
	return pFrameBuffer->GetBuffer();
}

uint32_t VDP::GetPixel(int x, int y)
{
	return pFrameBuffer->GetPixel(x, y);
}

uint16_t VDP::GetScreenWidth()
{
	return 256;
}

uint16_t VDP::GetScreenHeight()
{
	return active_period;
}

uint32_t VDP::GetColorFromCRam(uint8_t color, uint8_t palette)
{
	// Read the value from CRAM given: Color Index (0..15) and Palette (0..1)
	// The value read from CRAM is used as an index for the Palette
	// The Actual Palette Color is returned as RGB Value.
	return palScreen[cram[(palette * 16) + color]];	
}

uint32_t* VDP::GetCharTable(uint8_t table, uint8_t palette)
{
	for (uint16_t nTileY = 0; nTileY < 16; nTileY++)
	{
		for (uint16_t nTileX = 0; nTileX < 16; nTileX++)
		{
			uint16_t nOffset = nTileY * 512 + nTileX * 32;
			for (uint16_t row = 0; row < 8; row++)
			{
				uint8_t bitplane0 = vram[table * 0x2000 + nOffset + row * 0x0004];
				uint8_t bitplane1 = vram[table * 0x2000 + nOffset + row * 0x0004 + 1];
				uint8_t bitplane2 = vram[table * 0x2000 + nOffset + row * 0x0004 + 2];
				uint8_t bitplane3 = vram[table * 0x2000 + nOffset + row * 0x0004 + 3];
				for (uint16_t col = 0; col < 8; col++)
				{
					uint8_t pixel = ((bitplane0 >> 7) * 1) +
									((bitplane1 >> 7) * 2) +
									((bitplane2 >> 7) * 4) +
									((bitplane3 >> 7) * 8);

					bitplane0 = bitplane0 << 1; bitplane1 = bitplane1 << 1;
					bitplane2 = bitplane2 << 1; bitplane3 = bitplane3 << 1;
					
					pCharTable[table]->SetPixel(nTileX * 8 + col, nTileY * 8 + row, GetColorFromCRam(pixel, palette));
				}
			}
		}
	}

	return pCharTable[table]->GetBuffer();
}

/////////////////////////////////////////////////////////////////////////////////////////
//
//                VRAM, CRAM and Register Read & Write Functions
//
/////////////////////////////////////////////////////////////////////////////////////////

//---- Read Functions Data Port 0xBE
bool VDP::readDataPort(uint8_t& data)
{
	//Reset First Byte Received Flag
	bFirstByteRecv = false;

	data = read_buf;
	read_buf = vram[addr_reg++];
	addr_reg &= 0x3fff;

	return true;
}

//---- Write Functions Data Port 0xBE
bool VDP::writeDataPort(uint8_t data)
{
	//Reset First Byte Received Flag
	bFirstByteRecv = false;

	switch (code_reg)
	{
	case 0: vram[addr_reg++] = data; addr_reg &= 0x3fff; read_buf = data; break;					//VRAM Read Mode, Write is Deprecated but works
	case 1: vram[addr_reg++] = data; addr_reg &= 0x3fff; read_buf = data; break;					//VRAM Write Mode, it also load Read Buffer
	case 2: vram[addr_reg++] = data; addr_reg &= 0x3fff; break;										//Register Write Mode, VRAM Write is deprecated
	case 3: cram[addr_reg & 0x001f] = data; addr_reg++; addr_reg &= 0x3fff; read_buf = data; break;	//CRAM Write Mode, it also load Read Buffer
	}

	return true;
}

//---- Read Functions Control Port 0xBF
bool VDP::readControlPort(uint8_t& data)
{
	//Reset First Byte Received Flag
	bFirstByteRecv = false;

	//Read Status Register
	data = status.b;

	//Reset vBlank Interupt Flag
	status.vblank = 0;

	//Reset SpriteOverflow Flag
	status.sprovf = 0;

	return true;
}


//---- Write Functions Control Port 0xBF
bool VDP::writeControlPort(uint8_t data)
{
	if (bFirstByteRecv)
	{
		//Write MSB of Command Word
		command_word &= 0x00ff;
		command_word |= (data << 8);   //Second Byte goes to MSB
		bFirstByteRecv = false;

		//Decode Command Word
		code_reg = command_word >> 14;
		switch (code_reg)
		{
		case 0: //Set Address Register for Read from VRAM
			addr_reg = command_word & 0x3fff;	//Address Register Wraps at 16K
			read_buf = vram[addr_reg++];
			addr_reg &= 0x3fff;
			break;
		case 1: //Set Address Register for Write to VRAM
			addr_reg = command_word & 0x3fff;	//Address Register Wraps at 16K
			break;
		case 2: //Register Writes, it also set Address Register for Write to VRAM
			addr_reg = command_word & 0x3fff;	//Address Register Wraps at 16K
			writeRegister();
			break;
		case 3: //Set Address Register for Write to CRAM
			addr_reg = command_word & 0x3fff;  //Address Register Wraps at 32
			break;
		}	
	}
	else
	{
		//Write LSB without cleaning previous MSB of Command Word
		command_word = (command_word & 0xff00) | data;
		addr_reg = command_word & 0x3fff;
		bFirstByteRecv = true;
	}

	return true;
}

//---- Write To Register and Update internal status accordingly
bool VDP::writeRegister()
{
	//Define Sprite Attribute Table Address from Reg5
	auto setSpriteAttributeTableAddr = [&]()
	{
		sprAttrTabAddr = (((reg5 >> 1) & 0x3f) << 8);
		return;
	};

	//Define Name Table Address from Reg6
	auto setNameTableAddr = [&]()
	{
		nameTabAddr = (((reg2 >> 1) & 0x07) << 11);
		return;
	};

	//Define Video Mode & Vertical Resolution according to Reg0 & Reg1
	auto setVideoMode = [&]()
	{
		uint8_t mode = 0x00;
		mode = (reg0.m4 * 8) + (reg1.m3 * 4) + (reg0.m2 * 2) + reg1.m1;

		switch (mode)
		{
		case 0x00: break;					//Graphic 1
		case 0x01: break;					//Text
		case 0x02: video_mode = 2; break;	//Graphic 2
		case 0x03: break;					//Mode 1+2
		case 0x04: break;					//Multicolor
		case 0x05: break;					//Mode 1+3
		case 0x06: break;					//Mode 2+3
		case 0x07: break;					//Mode 1+2+3
		case 0x08:							//Mode 4 (192-line display)
			video_mode = 4;
			active_period = 192;
			additional_scan = 32;			
			row_number = 28;
			break;
		case 0x09: break;					//Invalid Text Mode
		case 0x0a:							//Mode 4 (192-line display)
			video_mode = 4;
			active_period = 192; 
			additional_scan = 32;			
			row_number = 28;
			break;
		case 0x0b:							//Mode 4 (224-line display)
			video_mode = 4;
			active_period = 224;
			additional_scan = 32;			
			row_number = 32;
			break;
		case 0x0c:							//Mode 4 (192-line display)
			video_mode = 4;
			active_period = 192;
			additional_scan = 32;			
			row_number = 28;
			break;
		case 0x0d: break;					//Invalid Text Mode															
		case 0x0e:							//Mode 4 (240-line display)
			video_mode = 4;
			active_period = 240; 
			additional_scan = 16;			
			row_number = 32;
			break;
		case 0x0f:							//Mode 4 (192-line display)
			video_mode = 4;
			active_period = 192; 
			additional_scan = 32;			
			row_number = 28;
			break;
		}
	};

	//Register Number is bit11 - bit8
	//command_word LSB is copied in the selected Register.
	switch ((command_word >> 8) & 0x0f)
	{
	case 0x00: reg0.b = (uint8_t)command_word; setVideoMode(); break;
	case 0x01: reg1.b = (uint8_t)command_word; setVideoMode(); break;
	case 0x02: reg2 = (uint8_t)command_word; break;
	case 0x03: reg3 = (uint8_t)command_word; break;
	case 0x04: reg4 = (uint8_t)command_word; break;
	case 0x05: reg5 = (uint8_t)command_word; setSpriteAttributeTableAddr(); break;
	case 0x06: reg6 = (uint8_t)command_word; setNameTableAddr(); break;
	case 0x07: reg7 = (uint8_t)command_word; break;
	case 0x08: reg8 = (uint8_t)command_word; break;
	case 0x09: reg9 = (uint8_t)command_word; break;
	case 0x0a: reg10 = (uint8_t)command_word; break;
	}
	
	return true;
}

////////////////////////////////////////////////////////////////////////////////
//
//                      Background & Sprite Renderer
//
////////////////////////////////////////////////////////////////////////////////
bool VDP::RenderBackground(uint8_t priority)
{
	NameTableEntry elem;
	uint16_t nameTableOffset;
	uint16_t tileOffset;
	uint16_t tileLineAddr;

	uint8_t current_row = VCount >> 3;
	uint8_t current_col = HCount >> 3;
	uint8_t vscroll_row = reg9 >> 3;
	uint8_t hscroll_col = 0x20 - (reg8 >> 3);
	uint8_t vscroll_fine = reg9 & 0x07;
	uint8_t hscroll_fine = reg8 & 0x07;
	
	uint8_t starting_row;
	uint8_t starting_col;

	uint8_t x, y;

	//Render one full scanline at a time for Active Period Only
	if (HCount == 0xff)
	{
		//Go thru all 32 column
		for (uint8_t col = 0; col < 32; col++)
		{
			//Check if Horizontal Scroll in inibited for Rows 0-1 and set starting Column according
			//to Hardware Horizontal Scrolling Register Reg8
			if (reg0.hsi)
			{
				//Horizontal Scrolling Disabled for Row 0-1
				if (current_row < 2)
				{
					starting_col = col;
				}
				else
				{
					starting_col = (col + hscroll_col) % 32;
				}
			}
			else
			{
				starting_col = (col + hscroll_col) % 32;
			}

			//Check if Vertical Scroll in inibited for Columns 24-31 and set starting Row according
			//to Hardware Vertical Scrolling Register Reg9
			if (reg0.vsi)
			{
				//Vertical Scrolling Disabled for Columns 24 - 31
				if (col < 25)
				{
					starting_row = ((VCount + reg9) >> 3) % row_number;
				}
				else
				{
					starting_row = current_row;
				}
			}
			else
			{
				starting_row = ((VCount + reg9) >> 3) % row_number;
			}

			//Set Name Table Offset according to Starting Row and Columnw
			//Each Name Table Entry is 2 Byte per Tile
			nameTableOffset = nameTabAddr + starting_col * 2 + starting_row * 64;

			//Read Name Table Entry for current Tile (2 Byte)
			elem.lsb = vram[nameTableOffset];
			elem.msb = vram[nameTableOffset + 1];

			//Point to Tile in VRAM
			tileOffset = elem.char_id * 32;

			//Read all 4 bitplane for current Tile Line
			//Specific Line is swapped Horizontally and/or Vertically according
			//to Name Table Entry flags 
			for (int i = 0; i < 4; i++)
			{
				//Define Tile Line Address in VRAM according to Vertical Scrolling Settings
				uint8_t vline_offset;
				if (reg0.vsi)
				{
					//Vertical Scrolling Disabled for Columns 24 - 31
					if (current_col < 25)
					{
						vline_offset = (VCount + vscroll_fine) % 8;
					}
					else
					{
						vline_offset = (VCount % 8);
					}
				}
				else
				{
					vline_offset = (VCount + vscroll_fine) % 8;
				}
				
				//Check if the tile is Flipped Vertically
				if (elem.vflip)
				{
					tileLineAddr = tileOffset + (7 - vline_offset) * 0x04 + i;
				}
				else
				{
					tileLineAddr = tileOffset + vline_offset * 0x04 + i;
				}

				//Check if the Tile is flipped Horizontally
				if (elem.hflip)
				{
					bplShifter.LoadBitplane(i, _bitswap_uint8(vram[tileLineAddr]));
				}
				else
				{
					bplShifter.LoadBitplane(i, vram[tileLineAddr]);
				}
			}
			
			//Render a full line (8 pixel) of the current Tile
			if (elem.priority == priority)
			{
				for (int j = 0; j < 8; j++)
				{
					//Get Color Value for Each Pixel
					uint8_t color = bplShifter.GetPixelColor();

					//Set Screen Renderer Coordinates according to Scrolling Settings
					if (reg0.hsi)
					{
						//Horizontal Scrolling Disabled for Row 0-1
						if (current_row < 2)
						{
							x = col * 8 + j;
						}
						else
						{
							x = col * 8 + j + hscroll_fine;
						}
					}
					else
					{
						x = col * 8 + j + hscroll_fine;
					}
					
					y = VCount;
					
					//Render Pixel - Color 0 is always transparent.
					if (color != 0x00)
						pFrameBuffer->SetPixel(x, y, GetColorFromCRam(color, elem.palette));
				}
			}
		}
	}

	return true;
}

bool VDP::RenderSprites()
{
	auto spriteOnScanline = [&](uint8_t y)
	{
		int upper, lower;

		if ((y >= active_period) && (y <= 0xff))
		{ 
			upper = y - 256;
		}
		else
		{
			upper = y;
		}

		lower = upper + 7 + 8 * reg1.sz;
		
		if ((VCount >= upper) && (VCount <= lower))
		{
			return true;
		}
		else
		{
			return false;
		}
	};
	
	//Render one full scanline at a time for Active Period Only
	if (HCount == 0xff)
	{
		//Scan Sprite Attribute Table vpos for this scanline
		sprCounter = 0;
		for (uint8_t i = 0; i < 64; i++)
		{
			uint8_t vpos = vram[sprAttrTabAddr + i];

			//if vertical resolution is 192 and Sprite Y coordinate is 0xd0 stop searching
			if (active_period == 192 && vpos == 0xd0)
				break;
			
			//VPOS id stored as minus one, a value of 1 refer to scanline 2 (counting from zero)
			vpos += 1;
		
			//Check which sprite falls on this scanline depending on its vpos and height
			if (spriteOnScanline(vpos))
			{
				if (sprCounter < MAXSPRITEPERLINE)
				{
					//Put Sprite Position on Sprite Attribute Table on the Sprite Buffer
					sprBuffer[sprCounter] = i;
					sprCounter++;
				}
				else
				{
					//Set Sprite Overflow Flag on the Status Register
					status.sprovf = 1;
					break;
				}
			}
		}

		//if Sprite Buffer is not Empty Draw Sprite on this Scanline
		if (sprCounter != 0)
		{
			for (uint8_t i = 0; i < sprCounter; i++)
			{
				//Extract Current Sprite Attributes from Attribute Table
				uint8_t sprY = vram[sprAttrTabAddr + sprBuffer[i]] + 1;
				uint8_t sprX = vram[sprAttrTabAddr + 0x80 + sprBuffer[i] * 2];
				uint16_t sprID = vram[sprAttrTabAddr + 0x80 + sprBuffer[i] * 2 + 1];

				//Update sprID according to Reg6 Setting, it select beetwen first 256 tile and second 256 Tiles
				if (((reg6 >> 2) & 0x01) == 1)
				{
					sprID += 256;
				}

				//Read all 4 bitplane for current Sprite Line, Fifth Bitplane in always 0xff
				uint16_t sprLineAddr;
				uint8_t sprLineOffset;

				for (uint8_t j = 0; j < 4; j++)
				{
					
					if ((sprY >= active_period) && (sprY <= 0xff))
					{
						sprLineOffset = VCount - (sprY - 256);
					}
					else
					{
						sprLineOffset = VCount - sprY;
					}

					sprLineAddr = sprID * 32 + sprLineOffset * 4 + j;
					bplShifter.LoadBitplane(j, vram[sprLineAddr]);
				}
				
				//Render Sprite Line
				for (uint8_t k = 0; k < 8; k++)
				{
					uint8_t color = bplShifter.GetPixelColor();

					//Don't draw pixel if color is transparent (Palette 1, Color #0)
					if (color != 0x00)
					{
						pFrameBuffer->SetPixel(sprX + k - 8 * reg0.ss, VCount, GetColorFromCRam(color, 1));
					}
				}
			}
		}
	}
	
	return true;
}

bool VDP::MaskColumnOne()
{
	if (HCount == 0xff)
	{
		if (reg0.lcb)
		{
			for (int i = 0; i < 8; i++)
				pFrameBuffer->SetPixel(i, VCount, GetColorFromCRam(reg7 & 0x0f, 1));
		}
	}
	
	return true;
};

