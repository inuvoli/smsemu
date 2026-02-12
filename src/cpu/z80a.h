#pragma once
#include<array>
#include<vector>
#include<string>
#include<map>
#include<iostream>

class SMS;

#define LITTLEENDIAN

//8bit Register Direct Access Macro
#define A_ af.b.h
#define F_ af.b.l
#define B_ bc.b.h
#define C_ bc.b.l
#define D_ de.b.h
#define E_ de.b.l
#define H_ hl.b.h
#define L_ hl.b.l
#define I_ ir.b.h
#define R_ ir.b.l

#define W_ wz.b.h
#define Z_ wz.b.l

#define AF_ af.w
#define BC_ bc.w
#define DE_ de.w
#define HL_ hl.w

#define IX_ ix.w
#define IY_ iy.w
#define SP_ sp.w
#define PC_ pc.w

#define IR_ ir.w
#define WZ_ wz.w

#define IXH_ ix.b.h
#define IXL_ ix.b.l
#define IYH_ iy.b.h
#define IYL_ iy.b.l
#define SPH_ sp.b.h
#define SPL_ sp.b.l
#define PCH_ pc.b.h
#define PCL_ pc.b.l

//Status Register Flags Direct Access Macro
#define SF af.f.sf	//Sign
#define ZF af.f.zf	//Zero
#define YF af.f.yf	//Not Used
#define HF af.f.hf	//Half Carry
#define XF af.f.xf	//Not Used
#define PF af.f.pf	//Parity - Overflow
#define NF af.f.nf	//Add Subtract
#define CF af.f.cf	//Carry Bit

//OPCODE Direct Access Macro
#define OPB(x) opcode[x]
#define OPW(x) (opcode[x+1] << 8) + opcode[x]

//Status Register F Flags Masks
enum FLAGS
{
	SM = (1 << 7),	//Sign
	ZM = (1 << 6),	//Zero
	YM = (1 << 5),	//Not Used
	HM = (1 << 4),	//Half Carry
	XM = (1 << 3),	//Not Used
	PM = (1 << 2),	//Parity - Overflow
	NM = (1 << 1),	//Add Subtract
	CM = (1 << 0),	//Carry Bit
};

union PAIR
{
#ifdef LITTLEENDIAN
	struct { uint8_t l, h; } b;
#endif // LITTLEENDIAN

#ifdef BIGENDIAN
	struct { uint8_t h, l; } b;
#endif // BIGENDIAN
	uint16_t w;
};

union PAIR2
{
#ifdef LITTLEENDIAN
	struct { uint8_t l, h; } b;
	struct {
		uint8_t cf : 1, nf : 1, pf : 1, xf : 1, hf : 1, yf : 1, zf : 1, sf : 1;
		uint8_t : 0;
	} f;
#endif // LITTLEENDIAN

#ifdef BIGENDIAN
	struct { uint8_t l, h; } b;
	struct {
		uint8_t : 0;
		uint8_t cf : 1, nf : 1, pf : 1, xf : 1, hf : 1, yf : 1, zf : 1, sf : 1;
	} f;
#endif // BIGENDIAN
	uint16_t w;
};

class Z80A
{


public:
	Z80A();
	~Z80A();

public:
	//Z80 Internal Registers
	PAIR2 af, af1;
	PAIR bc, de, hl;
	PAIR bc1, de1, hl1;
	PAIR ir;
	PAIR ix, iy;
	PAIR sp, pc;
	PAIR wz;
		
	//CPU Status
	uint8_t	intmode;
	bool bIFF1, bIFF2;
	bool bHalt;

	//Debug Only
	bool bOpcodeComplete;

public:
	void ConnectBus(SMS* n) { sms = n; }
	
	//External Signal
	bool clock();
	bool reset();
	bool irq();
	bool nmi();
	
	// Produces a map of strings, with keys equivalent to instruction start locations
	// in memory, for the specified address range
	std::map<uint16_t, std::string> disassemble(uint16_t nStart, uint16_t nStop);

private:
	////////////////////////////////////////////////////////////////////////////////
	//
	//               DOCUMENTED INSTRUCTIONS
	//
	////////////////////////////////////////////////////////////////////////////////
	//8-Bit Load Instructions
	bool LDRR();	bool LDiRR();	bool LDRRi();
	bool LDRN();	bool LDAiBC();	bool LDAiDE();	bool LDAiNN();	
	bool LDiBCA();	bool LDiDEA();	bool LDiNNA();	bool LDAI();	bool LDAR();	
	bool LDIA();	bool LDRA();

	//16-Bit Load Instructions
	bool LDDDNN();	bool LDHLiNN();	bool LDDDiNN(); bool LDIXiNN();	bool LDIYiNN();
	bool LDiNNHL();	bool LDiNNDD();	bool LDiNNIX();	bool LDiNNIY();	bool LDSPHL();	
	bool LDSPIX();	bool LDSPIY();	bool PUSHQQ();	bool PUSHIX();	bool PUSHIY();
	bool POPQQ();	bool POPIX();	bool POPIY();

	//Exchange, Block Transfer and Search Instructions
	bool EXDEHL();	bool EXAF();	bool EXX();		bool EXiSPHL();	bool EXiSPIX();
	bool EXiSPIY();	bool LDI();		bool LDIR();	bool LDD();		bool LDDR();
	bool CPI();		bool CPIR();	bool CPD();		bool CPDR();

	//8-Bit Arithmetic Instructions
	bool ADDR();	bool ADDN();	
	bool ADCR();	bool ADCN();	
	bool SUBR();	bool SUBN();	
	bool SBCR();	bool SBCN();	
	bool ANDR();	bool ANDN();	
	bool ORR();		bool ORN();		
	bool XORR();	bool XORN();	
	bool CPR();		bool CPN();		
	bool INCR();	bool DECR();
	
	//General Purpose Arithmetic and CPU Control Instructions
	bool DAA();		bool CPL();		bool NEG();		bool CCF();		bool SCF();
	bool NOP();		bool HALT();	bool DI();		bool EI();		bool IM0();
	bool IM1();		bool IM2();

	//16-Bit Arithmetic Instructions
	bool ADDHLSS();	bool ADCHLSS();	bool SBCHLSS();	bool ADDIXSS();	bool ADDIYSS();
	bool INCSS();	bool INCIX();	bool INCIY();	bool DECSS();	bool DECIX();
	bool DECIY();

	//Rotate and Shift Instructions
	bool RLCA();	bool RLA();		bool RRCA();	bool RRA();		bool RLCR();
	bool RLR();		bool RRCR();	bool RRR();		bool SLAR();	bool SRAR();
	bool SRLR();	bool SLLR();	bool RLD();		bool RRD();

	//Bit Set, Reset and Test Instructions
	bool BITBR();	bool SETBR();	bool RESBR();	
	

	//Jump Instructions
	bool JPNN();	bool JPCCNN();	bool JRE();		bool JRCE();	bool JRNCE();
	bool JRZE();	bool JRNZE();	bool JPiHL();	bool JPiIX();	bool JPiIY();
	bool DJNZE();

	//Call and Return Instructions
	bool CALLNN();	bool CALLCCNN();bool RET();		bool RETCC();	bool RETI();
	bool RETN();	bool RSTP();

	//Input and Output Instructions
	bool INAiN();	bool INRiC();	bool INI();		bool INIR();	bool IND();
	bool INDR();	bool OUTiNA();	bool OUTiCR();	bool OUTI();	bool OTIR();
	bool OUTD();	bool OTDR();

	////////////////////////////////////////////////////////////////////////////////
	//
	//               UNDOCUMENTED INSTRUCTIONS
	//
	////////////////////////////////////////////////////////////////////////////////
	//Input and Output Instructions
	bool INiC();	bool OUTiC0();

	//Undocumented Instructions
	bool UNDOC(); bool INVAL();

	////////////////////////////////////////////////////////////////////////////////
	//
	//               NMI & INT Execution Functions
	//
	////////////////////////////////////////////////////////////////////////////////
	bool NMIEXEC();	bool IRQEXEC();

	//Helper Function
	bool isP(uint8_t res);
	bool isC(uint32_t res, uint32_t b, uint8_t c, uint8_t bit);
	bool isB(uint32_t res, uint32_t b, uint8_t c, uint8_t bit);
	bool isO(uint32_t res, uint32_t b, uint8_t c, uint8_t bit);
	bool isU(uint32_t res, uint32_t b, uint8_t c, uint8_t bit);

	// Read-Write instruction from Stack, Memory and I/O Ports
	uint8_t		memRd(uint16_t addr);
	bool		memWr(uint16_t addr, uint8_t data);
	uint16_t	memRd16(uint16_t addr);
	bool		memWr16(uint16_t addr, uint16_t data);
	uint16_t	stkRd16(uint16_t addr);
	bool		stkWr16(uint16_t addr, uint16_t data);
	uint8_t		ioRd(uint8_t addr);
	bool		ioWr(uint8_t addr, uint8_t data);
	   		   	 	
private:
	//Pointer to BUS Object
	SMS		*sms = nullptr;

	//Z80 Opcodes range from 1 to 4 bytes long
	std::array<uint8_t, 4> opcode;
	uint16_t currentOp, indexOp;

	//Contains the number of clock ticks to complete the instruction now in execution
	uint8_t  cycles = 0;
	uint32_t nCycleCounter;

	//OPcode PLA Status
	bool cbprefix, ddprefix, edprefix, fdprefix, ddcbprefix, fdcbprefix;

	//Interrupt Status
	bool nmi_latch, irq_latch;

	//Internal Variable Used by OpCodes Implementation
	uint8_t u8, res;
	uint16_t u16;
		
	// Z80 Instruction Dictionaries
	struct INSTRUCTION
	{
		std::string operation;
		bool(Z80A::*operate)() = nullptr;
		uint8_t cycles1 = 0;
		uint8_t cycles2 = 0;
		uint8_t lenght = 0;
	};
	
	//Full set of Instruction Dictionaries
	std::vector<INSTRUCTION> instSet;		//Full Instruction Set
};

