#pragma once
#include <cstdint>
#include <array>
#include "bitplaneshifter.h"
#include "framebuffer.h"

class SMS;

#define MAXSPRITEPERLINE	8

union NameTableEntry {
	struct { uint8_t lsb, msb; };
	uint16_t w;
	struct {
		uint16_t char_id : 9;
		uint16_t hflip : 1;
		uint16_t vflip : 1;
		uint16_t palette : 1;
		uint16_t priority : 1;
		uint16_t : 3;
	};
};

class VDP
{
public:
	VDP();
	~VDP();

public:
	bool bFrameComplete;
	float fFPS;

	enum VIDEOMODE
	{
		PAL = 0,
		NTSC = 1
	};
	
	// --------------------- VDP Internal Registers ----------------------------
	union {
		struct {
			uint8_t es : 1;						//External Sync
			uint8_t m2 : 1;						//Mode Select M2
			uint8_t m4 : 1;						//Mode Select M4
			uint8_t ss : 1;						//Sprite Shift
			uint8_t ie1 : 1;					//Line Interrupt Enable
			uint8_t lcb : 1;					//Left Column Blank
			uint8_t hsi : 1;					//Horizontal Scroll Inhibit
			uint8_t vsi : 1;					//Vertical Scoll Inhibit
		};
		uint8_t b;
	} reg0;										//MODE 1 Register

	union {
		struct {
			uint8_t sd : 1;						//Sprite pixel double in size						
			uint8_t sz : 1;						//Sprite 8x16 (1), 8x8 (0)
			uint8_t : 1;						//Unused
			uint8_t m3 : 1;						//Mode Select M3
			uint8_t m1 : 1;						//Mode Select M1
			uint8_t ie0 : 1;					//Frame Interrupt Enable
			uint8_t blk : 1;					//Video Enable
			uint8_t : 1;						//Unused
		};
		uint8_t b;
	} reg1;										//MODE 2 Register

	uint8_t reg2, reg3, reg4, reg5, reg6;		//VDP Internal Registers
	uint8_t reg7, reg8, reg9, reg10;			//VDP Internal Registers

	union {
		struct {
			uint8_t : 5;
			uint8_t sprcol : 1;
			uint8_t sprovf : 1;
			uint8_t vblank : 1;
		};
		uint8_t b;
	} status;										//VDP Status Register
	
public:
	//Pointer to BUS Object
	SMS *sms = nullptr;

	void ConnectBus(SMS* n) { sms = n; }
	void SetVideoStandard(uint8_t mode);
	bool read(uint8_t addr, uint8_t& data);
	bool write(uint8_t addr, uint8_t data);
	bool reset();
	bool clock();
	
	uint32_t* GetScreen();
	uint32_t GetPixel(int x, int y);
	uint16_t GetScreenWidth();
	uint16_t GetScreenHeight();
	uint32_t* GetCharTable(uint8_t table, uint8_t palette);
	uint32_t GetColorFromCRam(uint8_t color, uint8_t palette);
		
private:
	uint32_t  palScreen[0x40];						//SMS Palette, contains all 64 Colors available in RGBA format
	FrameBuffer* pRenderBuffer;						//Render Buffer, used to render all the scanlines including overscan
	FrameBuffer* pFrameBuffer;						//Frame Buffer, contain only the visible part of the Render Buffer 
	FrameBuffer* pCharTable[2];						//Character Table split in two Tables (0x000-0x0ff) and (0x100 - 0x1ff), debub only 
	BitplaneShifter	bplShifter{ 4 };				//Bitplane Shift Register, used to extract Color Value for each Pixel
	
	std::array<uint8_t, 16 * 1024> vram;			//VDP 16K Video RAM
	std::array<uint8_t, 32 > cram;					//VDP 32 Byte Color Palette RAM

	uint16_t sprAttrTabAddr;						//Pointer to Sprite Attribute Table in VRAM, defined by Reg5;
	uint16_t nameTabAddr;							//Pointer to Name Table in VRAM, defined by Reg2;

	uint16_t HCount, VCount;
	uint16_t scanline_lenght, scanline_number;
	uint32_t nFrameCounter;
	uint32_t nCycleCounter;
	
	bool bFirstByteRecv;
	uint8_t read_buf;								//VDP Read Buffer
	uint16_t command_word;
	uint16_t addr_reg;
	uint8_t code_reg;

	uint8_t video_std;								//Video Standard (PAL - NTSC)
	uint8_t video_mode;								//VDP Video Mode - Set By Reg0 & Reg1
	uint16_t active_period;							//VDP Vertical Visible Area - Set By Reg & Reg1
	uint16_t additional_scan;						//VDP Additional Scan Line, used for Scrolling - Depends on Resolution
	uint8_t raster_counter;							//Line Counter used for Line Interrupt - Set by RegA

	uint8_t col_counter, row_counter;				//Column and Row counters, used for rendering
	uint8_t row_number;								//Number of Row per Frame, depends on Resolution (28 or 32)
	
	uint8_t sprBuffer[64];							//Internal Sprite Buffer, used for rendering Sprite
	uint8_t sprCounter;								//Number of Sprite on the Sprite Buffer, used for rendering Sprite
		
	//Read/Write VRAM, CRAM, Register
	bool readDataPort(uint8_t& data);
	bool readControlPort(uint8_t& data);
	bool writeDataPort(uint8_t data);
	bool writeControlPort(uint8_t data);
	bool writeRegister();

	inline uint8_t _bitswap_uint8(uint8_t x);
	inline uint32_t _rgba_to_int(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

	bool RenderBackground(uint8_t priority);
	bool RenderSprites();
	bool MaskColumnOne();
};

