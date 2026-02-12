#include <loguru.hpp>
#include "z80a.h"
#include "sms.h"

//using Register

Z80A::Z80A()
{
	//Init Registers
	AF_ = 0xffff;	af1.w = 0xffff;
	BC_ = 0x0000;	bc1.w = 0x0000;
	DE_ = 0x0000;	de1.w = 0x0000;
	HL_ = 0x0000;	hl1.w = 0x0000;
	IR_ = 0x0000;
	IX_ = 0x0000;
	IY_ = 0x0000;
	SP_ = 0xffff;
	PC_ = 0x0000;
	
	//Init CPU Status
	bHalt = false;
	bIFF1 = false;
	bIFF2 = false;
	intmode = 0;
	nmi_latch = false;
	irq_latch = false;

	nCycleCounter = 0;
	
	//Debug Only
	bOpcodeComplete = false;

	//Init Main Dictionary
	using z = Z80A;
		
	instSet =
	{
		//MAIN Instruction Set
		{"NOP ", &z::NOP, 4, 0, 1},
		{"LD BC,0xhhll", &z::LDDDNN, 10, 0, 3},
		{"LD (BC),A", &z::LDiBCA, 7, 0, 1},
		{"INC BC", &z::INCSS, 6, 0, 1},
		{"INC B", &z::INCR, 4, 0, 1},
		{"DEC B", &z::DECR, 4, 0, 1},
		{"LD B,0xll", &z::LDRN, 7, 0, 2},
		{"RLCA ", &z::RLCA, 4, 0, 1},
		{"EX AF,AF'", &z::EXAF, 4, 0, 1},
		{"ADD HL,BC", &z::ADDHLSS, 11, 0, 1},
		{"LD A,(BC)", &z::LDAiBC, 7, 0, 1},
		{"DEC BC", &z::DECSS, 6, 0, 1},
		{"INC C", &z::INCR, 4, 0, 1},
		{"DEC C", &z::DECR, 4, 0, 1},
		{"LD C,0xll", &z::LDRN, 7, 0, 2},
		{"RRCA ", &z::RRCA, 4, 0, 1},
		{"DJNZ 0xll", &z::DJNZE, 13, 8, 2},
		{"LD DE,0xhhll", &z::LDDDNN, 10, 0, 3},
		{"LD (DE),A", &z::LDiDEA, 7, 0, 1},
		{"INC DE", &z::INCSS, 6, 0, 1},
		{"INC D", &z::INCR, 4, 0, 1},
		{"DEC D", &z::DECR, 4, 0, 1},
		{"LD D,0xll", &z::LDRN, 7, 0, 2},
		{"RLA ", &z::RLA, 4, 0, 1},
		{"JR 0xll", &z::JRE, 12, 0, 2},
		{"ADD HL,DE", &z::ADDHLSS, 11, 0, 1},
		{"LD A,(DE)", &z::LDAiDE, 7, 0, 1},
		{"DEC DE", &z::DECSS, 6, 0, 1},
		{"INC E", &z::INCR, 4, 0, 1},
		{"DEC E", &z::DECR, 4, 0, 1},
		{"LD E,0xll", &z::LDRN, 7, 0, 2},
		{"RRA ", &z::RRA, 4, 0, 1},
		{"JR NZ 0xll", &z::JRNZE, 12, 7, 2},
		{"LD HL,0xhhll", &z::LDDDNN, 10, 0, 3},
		{"LD (0xhhll),HL", &z::LDiNNHL, 16, 0, 3},
		{"INC HL", &z::INCSS, 6, 0, 1},
		{"INC H", &z::INCR, 4, 0, 1},
		{"DEC H", &z::DECR, 4, 0, 1},
		{"LD H,0xll", &z::LDRN, 7, 0, 2},
		{"DAA ", &z::DAA, 4, 0, 1},
		{"JR Z 0xll", &z::JRZE, 12, 7, 2},
		{"ADD HL,HL", &z::ADDHLSS, 11, 0, 1},
		{"LD HL,(0xhhll)", &z::LDHLiNN, 16, 0, 3},
		{"DEC HL", &z::DECSS, 6, 0, 1},
		{"INC L", &z::INCR, 4, 0, 1},
		{"DEC L", &z::DECR, 4, 0, 1},
		{"LD L,0xll", &z::LDRN, 7, 0, 2},
		{"CPL ", &z::CPL, 4, 0, 1},
		{"JR NC 0xll", &z::JRNCE, 12, 7, 2},
		{"LD SP,0xhhll", &z::LDDDNN, 10, 0, 3},
		{"LD (0xhhll),A", &z::LDiNNA, 13, 0, 3},
		{"INC SP", &z::INCSS, 6, 0, 1},
		{"INC (HL)", &z::INCR, 11, 0, 1},
		{"DEC (HL)", &z::DECR, 11, 0, 1},
		{"LD (HL),0xll", &z::LDRN, 10, 0, 2},
		{"SCF ", &z::SCF, 4, 0, 1},
		{"JR C 0xll", &z::JRCE, 12, 7, 2},
		{"ADD HL,SP", &z::ADDHLSS, 11, 0, 1},
		{"LD A,(0xhhll)", &z::LDAiNN, 13, 0, 3},
		{"DEC SP", &z::DECSS, 6, 0, 1},
		{"INC A", &z::INCR, 4, 0, 1},
		{"DEC A", &z::DECR, 4, 0, 1},
		{"LD A,0xll", &z::LDRN, 7, 0, 2},
		{"CCF ", &z::CCF, 4, 0, 1},
		{"LD B,B", &z::LDRR, 4, 0, 1},
		{"LD B,C", &z::LDRR, 4, 0, 1},
		{"LD B,D", &z::LDRR, 4, 0, 1},
		{"LD B,E", &z::LDRR, 4, 0, 1},
		{"LD B,H", &z::LDRR, 4, 0, 1},
		{"LD B,L", &z::LDRR, 4, 0, 1},
		{"LD B,(HL)", &z::LDRRi, 7, 0, 1},
		{"LD B,A", &z::LDRR, 4, 0, 1},
		{"LD C,B", &z::LDRR, 4, 0, 1},
		{"LD C,C", &z::LDRR, 4, 0, 1},
		{"LD C,D", &z::LDRR, 4, 0, 1},
		{"LD C,E", &z::LDRR, 4, 0, 1},
		{"LD C,H", &z::LDRR, 4, 0, 1},
		{"LD C,L", &z::LDRR, 4, 0, 1},
		{"LD C,(HL)", &z::LDRRi, 7, 0, 1},
		{"LD C,A", &z::LDRR, 4, 0, 1},
		{"LD D,B", &z::LDRR, 4, 0, 1},
		{"LD D,C", &z::LDRR, 4, 0, 1},
		{"LD D,D", &z::LDRR, 4, 0, 1},
		{"LD D,E", &z::LDRR, 4, 0, 1},
		{"LD D,H", &z::LDRR, 4, 0, 1},
		{"LD D,L", &z::LDRR, 4, 0, 1},
		{"LD D,(HL)", &z::LDRRi, 7, 0, 1},
		{"LD D,A", &z::LDRR, 4, 0, 1},
		{"LD E,B", &z::LDRR, 4, 0, 1},
		{"LD E,C", &z::LDRR, 4, 0, 1},
		{"LD E,D", &z::LDRR, 4, 0, 1},
		{"LD E,E", &z::LDRR, 4, 0, 1},
		{"LD E,H", &z::LDRR, 4, 0, 1},
		{"LD E,L", &z::LDRR, 4, 0, 1},
		{"LD E,(HL)", &z::LDRRi, 7, 0, 1},
		{"LD E,A", &z::LDRR, 4, 0, 1},
		{"LD H,B", &z::LDRR, 4, 0, 1},
		{"LD H,C", &z::LDRR, 4, 0, 1},
		{"LD H,D", &z::LDRR, 4, 0, 1},
		{"LD H,E", &z::LDRR, 4, 0, 1},
		{"LD H,H", &z::LDRR, 4, 0, 1},
		{"LD H,L", &z::LDRR, 4, 0, 1},
		{"LD H,(HL)", &z::LDRRi, 7, 0, 1},
		{"LD H,A", &z::LDRR, 4, 0, 1},
		{"LD L,B", &z::LDRR, 4, 0, 1},
		{"LD L,C", &z::LDRR, 4, 0, 1},
		{"LD L,D", &z::LDRR, 4, 0, 1},
		{"LD L,E", &z::LDRR, 4, 0, 1},
		{"LD L,H", &z::LDRR, 4, 0, 1},
		{"LD L,L", &z::LDRR, 4, 0, 1},
		{"LD L,(HL)", &z::LDRRi, 7, 0, 1},
		{"LD L,A", &z::LDRR, 4, 0, 1},
		{"LD (HL),B", &z::LDiRR, 7, 0, 1},
		{"LD (HL),C", &z::LDiRR, 7, 0, 1},
		{"LD (HL),D", &z::LDiRR, 7, 0, 1},
		{"LD (HL),E", &z::LDiRR, 7, 0, 1},
		{"LD (HL),H", &z::LDiRR, 7, 0, 1},
		{"LD (HL),L", &z::LDiRR, 7, 0, 1},
		{"HALT ", &z::HALT, 4, 0, 1},
		{"LD (HL),A", &z::LDiRR, 7, 0, 1},
		{"LD A,B", &z::LDRR, 4, 0, 1},
		{"LD A,C", &z::LDRR, 4, 0, 1},
		{"LD A,D", &z::LDRR, 4, 0, 1},
		{"LD A,E", &z::LDRR, 4, 0, 1},
		{"LD A,H", &z::LDRR, 4, 0, 1},
		{"LD A,L", &z::LDRR, 4, 0, 1},
		{"LD A,(HL)", &z::LDRRi, 7, 0, 1},
		{"LD A,A", &z::LDRR, 4, 0, 1},
		{"ADD A,B", &z::ADDR, 4, 0, 1},
		{"ADD A,C", &z::ADDR, 4, 0, 1},
		{"ADD A,D", &z::ADDR, 4, 0, 1},
		{"ADD A,E", &z::ADDR, 4, 0, 1},
		{"ADD A,H", &z::ADDR, 4, 0, 1},
		{"ADD A,L", &z::ADDR, 4, 0, 1},
		{"ADD A,(HL)", &z::ADDR, 7, 0, 1},
		{"ADD A,A", &z::ADDR, 4, 0, 1},
		{"ADC A,B", &z::ADCR, 4, 0, 1},
		{"ADC A,C", &z::ADCR, 4, 0, 1},
		{"ADC A,D", &z::ADCR, 4, 0, 1},
		{"ADC A,E", &z::ADCR, 4, 0, 1},
		{"ADC A,H", &z::ADCR, 4, 0, 1},
		{"ADC A,L", &z::ADCR, 4, 0, 1},
		{"ADC A,(HL)", &z::ADCR, 7, 0, 1},
		{"ADC A,A", &z::ADCR, 4, 0, 1},
		{"SUB A,B", &z::SUBR, 4, 0, 1},
		{"SUB A,C", &z::SUBR, 4, 0, 1},
		{"SUB A,D", &z::SUBR, 4, 0, 1},
		{"SUB A,E", &z::SUBR, 4, 0, 1},
		{"SUB A,H", &z::SUBR, 4, 0, 1},
		{"SUB A,L", &z::SUBR, 4, 0, 1},
		{"SUB A,(HL)", &z::SUBR, 7, 0, 1},
		{"SUB A,A", &z::SUBR, 4, 0, 1},
		{"SBC A,B", &z::SBCR, 4, 0, 1},
		{"SBC A,C", &z::SBCR, 4, 0, 1},
		{"SBC A,D", &z::SBCR, 4, 0, 1},
		{"SBC A,E", &z::SBCR, 4, 0, 1},
		{"SBC A,H", &z::SBCR, 4, 0, 1},
		{"SBC A,L", &z::SBCR, 4, 0, 1},
		{"SBC A,(HL)", &z::SBCR, 7, 0, 1},
		{"SBC A,A", &z::SBCR, 4, 0, 1},
		{"AND B", &z::ANDR, 4, 0, 1},
		{"AND C", &z::ANDR, 4, 0, 1},
		{"AND D", &z::ANDR, 4, 0, 1},
		{"AND E", &z::ANDR, 4, 0, 1},
		{"AND H", &z::ANDR, 4, 0, 1},
		{"AND L", &z::ANDR, 4, 0, 1},
		{"AND (HL)", &z::ANDR, 7, 0, 1},
		{"AND A", &z::ANDR, 4, 0, 1},
		{"XOR B", &z::XORR, 4, 0, 1},
		{"XOR C", &z::XORR, 4, 0, 1},
		{"XOR D", &z::XORR, 4, 0, 1},
		{"XOR E", &z::XORR, 4, 0, 1},
		{"XOR H", &z::XORR, 4, 0, 1},
		{"XOR L", &z::XORR, 4, 0, 1},
		{"XOR (HL)", &z::XORR, 7, 0, 1},
		{"XOR A", &z::XORR, 4, 0, 1},
		{"OR B", &z::ORR, 4, 0, 1},
		{"OR C", &z::ORR, 4, 0, 1},
		{"OR D", &z::ORR, 4, 0, 1},
		{"OR E", &z::ORR, 4, 0, 1},
		{"OR H", &z::ORR, 4, 0, 1},
		{"OR L", &z::ORR, 4, 0, 1},
		{"OR (HL)", &z::ORR, 7, 0, 1},
		{"OR A", &z::ORR, 4, 0, 1},
		{"CP B", &z::CPR, 4, 0, 1},
		{"CP C", &z::CPR, 4, 0, 1},
		{"CP D", &z::CPR, 4, 0, 1},
		{"CP E", &z::CPR, 4, 0, 1},
		{"CP H", &z::CPR, 4, 0, 1},
		{"CP L", &z::CPR, 4, 0, 1},
		{"CP (HL)", &z::CPR, 7, 0, 1},
		{"CP A", &z::CPR, 4, 0, 1},
		{"RET NZ ", &z::RETCC, 11, 5, 1},
		{"POP BC", &z::POPQQ, 10, 0, 1},
		{"JP NZ 0xhhll", &z::JPCCNN, 10, 0, 3},
		{"JP 0xhhll", &z::JPNN, 10, 0, 3},
		{"CALL NZ 0xhhll", &z::CALLCCNN, 17, 10, 3},
		{"PUSH BC", &z::PUSHQQ, 11, 0, 1},
		{"ADD A,0xll", &z::ADDN, 7, 0, 2},
		{"RST 00h", &z::RSTP, 11, 0, 1},
		{"RET Z ", &z::RETCC, 11, 5, 1},
		{"RET ", &z::RET, 10, 0, 1},
		{"JP Z 0xhhll", &z::JPCCNN, 10, 0, 3},
		{"XXX", &z::INVAL, 0, 0, 1},
		{"CALL Z 0xhhll", &z::CALLCCNN, 17, 10, 3},
		{"CALL 0xhhll", &z::CALLNN, 17, 0, 3},
		{"ADC A,0xll", &z::ADCN, 7, 0, 2},
		{"RST 08h", &z::RSTP, 11, 0, 1},
		{"RET NC ", &z::RETCC, 11, 5, 1},
		{"POP DE", &z::POPQQ, 10, 0, 1},
		{"JP NC 0xhhll", &z::JPCCNN, 10, 0, 3},
		{"OUT (0xll),A", &z::OUTiNA, 11, 0, 2},
		{"CALL NC 0xhhll", &z::CALLCCNN, 17, 10, 3},
		{"PUSH DE", &z::PUSHQQ, 11, 0, 1},
		{"SUB A,0xll", &z::SUBN, 7, 0, 2},
		{"RST 10h", &z::RSTP, 11, 0, 1},
		{"RET C ", &z::RETCC, 11, 5, 1},
		{"EXX ", &z::EXX, 4, 0, 1},
		{"JP C 0xhhll", &z::JPCCNN, 10, 0, 3},
		{"IN A,(0xll)", &z::INAiN, 11, 0, 2},
		{"CALL C 0xhhll", &z::CALLCCNN, 17, 10, 3},
		{"XXX", &z::INVAL, 0, 0, 1},
		{"SBC A,0xll", &z::SBCN, 7, 0, 2},
		{"RST 18h", &z::RSTP, 11, 0, 1},
		{"RET PO ", &z::RETCC, 11, 5, 1},
		{"POP HL", &z::POPQQ, 10, 0, 1},
		{"JP PO 0xhhll", &z::JPCCNN, 10, 0, 3},
		{"EX (SP),HL", &z::EXiSPHL, 19, 0, 1},
		{"CALL PO 0xhhll", &z::CALLCCNN, 17, 10, 3},
		{"PUSH HL", &z::PUSHQQ, 11, 0, 1},
		{"AND 0xll", &z::ANDN, 7, 0, 2},
		{"RST 20h", &z::RSTP, 11, 0, 1},
		{"RET PE ", &z::RETCC, 11, 5, 1},
		{"JP (HL)", &z::JPiHL, 4, 0, 1},
		{"JP PE 0xhhll", &z::JPCCNN, 10, 0, 3},
		{"EX DE,HL", &z::EXDEHL, 4, 0, 1},
		{"CALL PE 0xhhll", &z::CALLCCNN, 17, 10, 3},
		{"XXX", &z::INVAL, 0, 0, 1},
		{"XOR 0xll", &z::XORN, 7, 0, 2},
		{"RST 28h", &z::RSTP, 11, 0, 1},
		{"RET P ", &z::RETCC, 11, 5, 1},
		{"POP AF", &z::POPQQ, 10, 0, 1},
		{"JP P 0xhhll", &z::JPCCNN, 10, 0, 3},
		{"DI ", &z::DI, 4, 0, 1},
		{"CALL P 0xhhll", &z::CALLCCNN, 17, 10, 3},
		{"PUSH AF", &z::PUSHQQ, 11, 0, 1},
		{"OR 0xll", &z::ORN, 7, 0, 2},
		{"RST 30h", &z::RSTP, 11, 0, 1},
		{"RET M ", &z::RETCC, 11, 5, 1},
		{"LD SP,HL", &z::LDSPHL, 6, 0, 1},
		{"JP M 0xhhll", &z::JPCCNN, 10, 0, 3},
		{"EI ", &z::EI, 4, 0, 1},
		{"CALL M 0xhhll", &z::CALLCCNN, 17, 10, 3},
		{"XXX", &z::INVAL, 0, 0, 1},
		{"CP 0xll", &z::CPN, 7, 0, 2},
		{"RST 38h", &z::RSTP, 11, 0, 1},
					
		//BIT Instruction Set
		{ "RLC B", &z::RLCR, 8, 0, 2 },
		{ "RLC C", &z::RLCR, 8, 0, 2 },
		{ "RLC D", &z::RLCR, 8, 0, 2 },
		{ "RLC E", &z::RLCR, 8, 0, 2 },
		{ "RLC H", &z::RLCR, 8, 0, 2 },
		{ "RLC L", &z::RLCR, 8, 0, 2 },
		{ "RLC (HL)", &z::RLCR, 15, 0, 2 },
		{ "RLC A", &z::RLCR, 8, 0, 2 },
		{ "RRC B", &z::RRCR, 8, 0, 2 },
		{ "RRC C", &z::RRCR, 8, 0, 2 },
		{ "RRC D", &z::RRCR, 8, 0, 2 },
		{ "RRC E", &z::RRCR, 8, 0, 2 },
		{ "RRC H", &z::RRCR, 8, 0, 2 },
		{ "RRC L", &z::RRCR, 8, 0, 2 },
		{ "RRC (HL)", &z::RRCR, 15, 0, 2 },
		{ "RRC A", &z::RRCR, 8, 0, 2 },
		{ "RL B", &z::RLR, 8, 0, 2 },
		{ "RL C", &z::RLR, 8, 0, 2 },
		{ "RL D", &z::RLR, 8, 0, 2 },
		{ "RL E", &z::RLR, 8, 0, 2 },
		{ "RL H", &z::RLR, 8, 0, 2 },
		{ "RL L", &z::RLR, 8, 0, 2 },
		{ "RL (HL)", &z::RLR, 15, 0, 2 },
		{ "RL A", &z::RLR, 8, 0, 2 },
		{ "RR B", &z::RRR, 8, 0, 2 },
		{ "RR C", &z::RRR, 8, 0, 2 },
		{ "RR D", &z::RRR, 8, 0, 2 },
		{ "RR E", &z::RRR, 8, 0, 2 },
		{ "RR H", &z::RRR, 8, 0, 2 },
		{ "RR L", &z::RRR, 8, 0, 2 },
		{ "RR (HL)", &z::RRR, 15, 0, 2 },
		{ "RR A", &z::RRR, 8, 0, 2 },
		{ "SLA B", &z::SLAR, 8, 0, 2 },
		{ "SLA C", &z::SLAR, 8, 0, 2 },
		{ "SLA D", &z::SLAR, 8, 0, 2 },
		{ "SLA E", &z::SLAR, 8, 0, 2 },
		{ "SLA H", &z::SLAR, 8, 0, 2 },
		{ "SLA L", &z::SLAR, 8, 0, 2 },
		{ "SLA (HL)", &z::SLAR, 15, 0, 2 },
		{ "SLA A", &z::SLAR, 8, 0, 2 },
		{ "SRA B", &z::SRAR, 8, 0, 2 },
		{ "SRA C", &z::SRAR, 8, 0, 2 },
		{ "SRA D", &z::SRAR, 8, 0, 2 },
		{ "SRA E", &z::SRAR, 8, 0, 2 },
		{ "SRA H", &z::SRAR, 8, 0, 2 },
		{ "SRA L", &z::SRAR, 8, 0, 2 },
		{ "SRA (HL)", &z::SRAR, 15, 0, 2 },
		{ "SRA A", &z::SRAR, 8, 0, 2 },
		{ "SLL B", &z::SLLR, 8, 0, 2 },
		{ "SLL C", &z::SLLR, 8, 0, 2 },
		{ "SLL D", &z::SLLR, 8, 0, 2 },
		{ "SLL E", &z::SLLR, 8, 0, 2 },
		{ "SLL H", &z::SLLR, 8, 0, 2 },
		{ "SLL L", &z::SLLR, 8, 0, 2 },
		{ "SLL (HL)", &z::SLLR, 15, 0, 2 },
		{ "SLL A", &z::SLLR, 8, 0, 2 },
		{ "SRL B", &z::SRLR, 8, 0, 2 },
		{ "SRL C", &z::SRLR, 8, 0, 2 },
		{ "SRL D", &z::SRLR, 8, 0, 2 },
		{ "SRL E", &z::SRLR, 8, 0, 2 },
		{ "SRL H", &z::SRLR, 8, 0, 2 },
		{ "SRL L", &z::SRLR, 8, 0, 2 },
		{ "SRL (HL)", &z::SRLR, 15, 0, 2 },
		{ "SRL A", &z::SRLR, 8, 0, 2 },
		{ "BIT 0,B", &z::BITBR, 8, 0, 2 },
		{ "BIT 0,C", &z::BITBR, 8, 0, 2 },
		{ "BIT 0,D", &z::BITBR, 8, 0, 2 },
		{ "BIT 0,E", &z::BITBR, 8, 0, 2 },
		{ "BIT 0,H", &z::BITBR, 8, 0, 2 },
		{ "BIT 0,L", &z::BITBR, 8, 0, 2 },
		{ "BIT 0,(HL)", &z::BITBR, 12, 0, 2 },
		{ "BIT 0,A", &z::BITBR, 8, 0, 2 },
		{ "BIT 1,B", &z::BITBR, 8, 0, 2 },
		{ "BIT 1,C", &z::BITBR, 8, 0, 2 },
		{ "BIT 1,D", &z::BITBR, 8, 0, 2 },
		{ "BIT 1,E", &z::BITBR, 8, 0, 2 },
		{ "BIT 1,H", &z::BITBR, 8, 0, 2 },
		{ "BIT 1,L", &z::BITBR, 8, 0, 2 },
		{ "BIT 1,(HL)", &z::BITBR, 12, 0, 2 },
		{ "BIT 1,A", &z::BITBR, 8, 0, 2 },
		{ "BIT 2,B", &z::BITBR, 8, 0, 2 },
		{ "BIT 2,C", &z::BITBR, 8, 0, 2 },
		{ "BIT 2,D", &z::BITBR, 8, 0, 2 },
		{ "BIT 2,E", &z::BITBR, 8, 0, 2 },
		{ "BIT 2,H", &z::BITBR, 8, 0, 2 },
		{ "BIT 2,L", &z::BITBR, 8, 0, 2 },
		{ "BIT 2,(HL)", &z::BITBR, 12, 0, 2 },
		{ "BIT 2,A", &z::BITBR, 8, 0, 2 },
		{ "BIT 3,B", &z::BITBR, 8, 0, 2 },
		{ "BIT 3,C", &z::BITBR, 8, 0, 2 },
		{ "BIT 3,D", &z::BITBR, 8, 0, 2 },
		{ "BIT 3,E", &z::BITBR, 8, 0, 2 },
		{ "BIT 3,H", &z::BITBR, 8, 0, 2 },
		{ "BIT 3,L", &z::BITBR, 8, 0, 2 },
		{ "BIT 3,(HL)", &z::BITBR, 12, 0, 2 },
		{ "BIT 3,A", &z::BITBR, 8, 0, 2 },
		{ "BIT 4,B", &z::BITBR, 8, 0, 2 },
		{ "BIT 4,C", &z::BITBR, 8, 0, 2 },
		{ "BIT 4,D", &z::BITBR, 8, 0, 2 },
		{ "BIT 4,E", &z::BITBR, 8, 0, 2 },
		{ "BIT 4,H", &z::BITBR, 8, 0, 2 },
		{ "BIT 4,L", &z::BITBR, 8, 0, 2 },
		{ "BIT 4,(HL)", &z::BITBR, 12, 0, 2 },
		{ "BIT 4,A", &z::BITBR, 8, 0, 2 },
		{ "BIT 5,B", &z::BITBR, 8, 0, 2 },
		{ "BIT 5,C", &z::BITBR, 8, 0, 2 },
		{ "BIT 5,D", &z::BITBR, 8, 0, 2 },
		{ "BIT 5,E", &z::BITBR, 8, 0, 2 },
		{ "BIT 5,H", &z::BITBR, 8, 0, 2 },
		{ "BIT 5,L", &z::BITBR, 8, 0, 2 },
		{ "BIT 5,(HL)", &z::BITBR, 12, 0, 2 },
		{ "BIT 5,A", &z::BITBR, 8, 0, 2 },
		{ "BIT 6,B", &z::BITBR, 8, 0, 2 },
		{ "BIT 6,C", &z::BITBR, 8, 0, 2 },
		{ "BIT 6,D", &z::BITBR, 8, 0, 2 },
		{ "BIT 6,E", &z::BITBR, 8, 0, 2 },
		{ "BIT 6,H", &z::BITBR, 8, 0, 2 },
		{ "BIT 6,L", &z::BITBR, 8, 0, 2 },
		{ "BIT 6,(HL)", &z::BITBR, 12, 0, 2 },
		{ "BIT 6,A", &z::BITBR, 8, 0, 2 },
		{ "BIT 7,B", &z::BITBR, 8, 0, 2 },
		{ "BIT 7,C", &z::BITBR, 8, 0, 2 },
		{ "BIT 7,D", &z::BITBR, 8, 0, 2 },
		{ "BIT 7,E", &z::BITBR, 8, 0, 2 },
		{ "BIT 7,H", &z::BITBR, 8, 0, 2 },
		{ "BIT 7,L", &z::BITBR, 8, 0, 2 },
		{ "BIT 7,(HL)", &z::BITBR, 12, 0, 2 },
		{ "BIT 7,A", &z::BITBR, 8, 0, 2 },
		{ "RES 0,B", &z::RESBR, 8, 0, 2 },
		{ "RES 0,C", &z::RESBR, 8, 0, 2 },
		{ "RES 0,D", &z::RESBR, 8, 0, 2 },
		{ "RES 0,E", &z::RESBR, 8, 0, 2 },
		{ "RES 0,H", &z::RESBR, 8, 0, 2 },
		{ "RES 0,L", &z::RESBR, 8, 0, 2 },
		{ "RES 0,(HL)", &z::RESBR, 15, 0, 2 },
		{ "RES 0,A", &z::RESBR, 8, 0, 2 },
		{ "RES 1,B", &z::RESBR, 8, 0, 2 },
		{ "RES 1,C", &z::RESBR, 8, 0, 2 },
		{ "RES 1,D", &z::RESBR, 8, 0, 2 },
		{ "RES 1,E", &z::RESBR, 8, 0, 2 },
		{ "RES 1,H", &z::RESBR, 8, 0, 2 },
		{ "RES 1,L", &z::RESBR, 8, 0, 2 },
		{ "RES 1,(HL)", &z::RESBR, 15, 0, 2 },
		{ "RES 1,A", &z::RESBR, 8, 0, 2 },
		{ "RES 2,B", &z::RESBR, 8, 0, 2 },
		{ "RES 2,C", &z::RESBR, 8, 0, 2 },
		{ "RES 2,D", &z::RESBR, 8, 0, 2 },
		{ "RES 2,E", &z::RESBR, 8, 0, 2 },
		{ "RES 2,H", &z::RESBR, 8, 0, 2 },
		{ "RES 2,L", &z::RESBR, 8, 0, 2 },
		{ "RES 2,(HL)", &z::RESBR, 15, 0, 2 },
		{ "RES 2,A", &z::RESBR, 8, 0, 2 },
		{ "RES 3,B", &z::RESBR, 8, 0, 2 },
		{ "RES 3,C", &z::RESBR, 8, 0, 2 },
		{ "RES 3,D", &z::RESBR, 8, 0, 2 },
		{ "RES 3,E", &z::RESBR, 8, 0, 2 },
		{ "RES 3,H", &z::RESBR, 8, 0, 2 },
		{ "RES 3,L", &z::RESBR, 8, 0, 2 },
		{ "RES 3,(HL)", &z::RESBR, 15, 0, 2 },
		{ "RES 3,A", &z::RESBR, 8, 0, 2 },
		{ "RES 4,B", &z::RESBR, 8, 0, 2 },
		{ "RES 4,C", &z::RESBR, 8, 0, 2 },
		{ "RES 4,D", &z::RESBR, 8, 0, 2 },
		{ "RES 4,E", &z::RESBR, 8, 0, 2 },
		{ "RES 4,H", &z::RESBR, 8, 0, 2 },
		{ "RES 4,L", &z::RESBR, 8, 0, 2 },
		{ "RES 4,(HL)", &z::RESBR, 15, 0, 2 },
		{ "RES 4,A", &z::RESBR, 8, 0, 2 },
		{ "RES 5,B", &z::RESBR, 8, 0, 2 },
		{ "RES 5,C", &z::RESBR, 8, 0, 2 },
		{ "RES 5,D", &z::RESBR, 8, 0, 2 },
		{ "RES 5,E", &z::RESBR, 8, 0, 2 },
		{ "RES 5,H", &z::RESBR, 8, 0, 2 },
		{ "RES 5,L", &z::RESBR, 8, 0, 2 },
		{ "RES 5,(HL)", &z::RESBR, 15, 0, 2 },
		{ "RES 5,A", &z::RESBR, 8, 0, 2 },
		{ "RES 6,B", &z::RESBR, 8, 0, 2 },
		{ "RES 6,C", &z::RESBR, 8, 0, 2 },
		{ "RES 6,D", &z::RESBR, 8, 0, 2 },
		{ "RES 6,E", &z::RESBR, 8, 0, 2 },
		{ "RES 6,H", &z::RESBR, 8, 0, 2 },
		{ "RES 6,L", &z::RESBR, 8, 0, 2 },
		{ "RES 6,(HL)", &z::RESBR, 15, 0, 2 },
		{ "RES 6,A", &z::RESBR, 8, 0, 2 },
		{ "RES 7,B", &z::RESBR, 8, 0, 2 },
		{ "RES 7,C", &z::RESBR, 8, 0, 2 },
		{ "RES 7,D", &z::RESBR, 8, 0, 2 },
		{ "RES 7,E", &z::RESBR, 8, 0, 2 },
		{ "RES 7,H", &z::RESBR, 8, 0, 2 },
		{ "RES 7,L", &z::RESBR, 8, 0, 2 },
		{ "RES 7,(HL)", &z::RESBR, 15, 0, 2 },
		{ "RES 7,A", &z::RESBR, 8, 0, 2 },
		{ "SET 0,B", &z::SETBR, 8, 0, 2 },
		{ "SET 0,C", &z::SETBR, 8, 0, 2 },
		{ "SET 0,D", &z::SETBR, 8, 0, 2 },
		{ "SET 0,E", &z::SETBR, 8, 0, 2 },
		{ "SET 0,H", &z::SETBR, 8, 0, 2 },
		{ "SET 0,L", &z::SETBR, 8, 0, 2 },
		{ "SET 0,(HL)", &z::SETBR, 15, 0, 2 },
		{ "SET 0,A", &z::SETBR, 8, 0, 2 },
		{ "SET 1,B", &z::SETBR, 8, 0, 2 },
		{ "SET 1,C", &z::SETBR, 8, 0, 2 },
		{ "SET 1,D", &z::SETBR, 8, 0, 2 },
		{ "SET 1,E", &z::SETBR, 8, 0, 2 },
		{ "SET 1,H", &z::SETBR, 8, 0, 2 },
		{ "SET 1,L", &z::SETBR, 8, 0, 2 },
		{ "SET 1,(HL)", &z::SETBR, 15, 0, 2 },
		{ "SET 1,A", &z::SETBR, 8, 0, 2 },
		{ "SET 2,B", &z::SETBR, 8, 0, 2 },
		{ "SET 2,C", &z::SETBR, 8, 0, 2 },
		{ "SET 2,D", &z::SETBR, 8, 0, 2 },
		{ "SET 2,E", &z::SETBR, 8, 0, 2 },
		{ "SET 2,H", &z::SETBR, 8, 0, 2 },
		{ "SET 2,L", &z::SETBR, 8, 0, 2 },
		{ "SET 2,(HL)", &z::SETBR, 15, 0, 2 },
		{ "SET 2,A", &z::SETBR, 8, 0, 2 },
		{ "SET 3,B", &z::SETBR, 8, 0, 2 },
		{ "SET 3,C", &z::SETBR, 8, 0, 2 },
		{ "SET 3,D", &z::SETBR, 8, 0, 2 },
		{ "SET 3,E", &z::SETBR, 8, 0, 2 },
		{ "SET 3,H", &z::SETBR, 8, 0, 2 },
		{ "SET 3,L", &z::SETBR, 8, 0, 2 },
		{ "SET 3,(HL)", &z::SETBR, 15, 0, 2 },
		{ "SET 3,A", &z::SETBR, 8, 0, 2 },
		{ "SET 4,B", &z::SETBR, 8, 0, 2 },
		{ "SET 4,C", &z::SETBR, 8, 0, 2 },
		{ "SET 4,D", &z::SETBR, 8, 0, 2 },
		{ "SET 4,E", &z::SETBR, 8, 0, 2 },
		{ "SET 4,H", &z::SETBR, 8, 0, 2 },
		{ "SET 4,L", &z::SETBR, 8, 0, 2 },
		{ "SET 4,(HL)", &z::SETBR, 15, 0, 2 },
		{ "SET 4,A", &z::SETBR, 8, 0, 2 },
		{ "SET 5,B", &z::SETBR, 8, 0, 2 },
		{ "SET 5,C", &z::SETBR, 8, 0, 2 },
		{ "SET 5,D", &z::SETBR, 8, 0, 2 },
		{ "SET 5,E", &z::SETBR, 8, 0, 2 },
		{ "SET 5,H", &z::SETBR, 8, 0, 2 },
		{ "SET 5,L", &z::SETBR, 8, 0, 2 },
		{ "SET 5,(HL)", &z::SETBR, 15, 0, 2 },
		{ "SET 5,A", &z::SETBR, 8, 0, 2 },
		{ "SET 6,B", &z::SETBR, 8, 0, 2 },
		{ "SET 6,C", &z::SETBR, 8, 0, 2 },
		{ "SET 6,D", &z::SETBR, 8, 0, 2 },
		{ "SET 6,E", &z::SETBR, 8, 0, 2 },
		{ "SET 6,H", &z::SETBR, 8, 0, 2 },
		{ "SET 6,L", &z::SETBR, 8, 0, 2 },
		{ "SET 6,(HL)", &z::SETBR, 15, 0, 2 },
		{ "SET 6,A", &z::SETBR, 8, 0, 2 },
		{ "SET 7,B", &z::SETBR, 8, 0, 2 },
		{ "SET 7,C", &z::SETBR, 8, 0, 2 },
		{ "SET 7,D", &z::SETBR, 8, 0, 2 },
		{ "SET 7,E", &z::SETBR, 8, 0, 2 },
		{ "SET 7,H", &z::SETBR, 8, 0, 2 },
		{ "SET 7,L", &z::SETBR, 8, 0, 2 },
		{ "SET 7,(HL)", &z::SETBR, 15, 0, 2 },
		{ "SET 7,A", &z::SETBR, 8, 0, 2 },

		//IX Instruction Set
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "ADD IX,BC", &z::ADDIXSS, 15, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "ADD IX,DE", &z::ADDIXSS, 15, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "LD IX,0xhhll", &z::LDDDNN, 14, 0, 4 },
		{ "LD (0xhhll),IX", &z::LDiNNIX, 20, 0, 4 },
		{ "INC IX", &z::INCIX, 10, 0, 2 },
		{ "INC IXH", &z::INCR, 8, 0, 2 },
		{ "DEC IXH", &z::DECR, 8, 0, 2 },
		{ "LD IXH,0xll", &z::LDRN, 11, 0, 3 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "ADD IX,IX", &z::ADDIXSS, 15, 0, 2 },
		{ "LD IX,(0xhhll)", &z::LDIXiNN, 20, 0, 4 },
		{ "DEC IX", &z::DECIX, 10, 0, 2 },
		{ "INC IXL", &z::INCR, 8, 0, 2 },
		{ "DEC IXL", &z::DECR, 8, 0, 2 },
		{ "LD IXL,0xll", &z::LDRN, 11, 0, 3 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "INC (IX+0xll)", &z::INCR, 23, 0, 3 },
		{ "DEC (IX+0xll)", &z::DECR, 23, 0, 3 },
		{ "LD (IX+0xll),0xhh", &z::LDRN, 19, 0, 4 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "ADD IX,SP", &z::ADDIXSS, 15, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "LD B,B", &z::LDRR, 8, 0, 2 },
		{ "LD B,C", &z::LDRR, 8, 0, 2 },
		{ "LD B,D", &z::LDRR, 8, 0, 2 },
		{ "LD B,E", &z::LDRR, 8, 0, 2 },
		{ "LD B,IXH", &z::LDRR, 8, 0, 2 },
		{ "LD B,IXL", &z::LDRR, 8, 0, 2 },
		{ "LD B,(IX+0xll)", &z::LDRRi, 19, 0, 3 },
		{ "LD B,A", &z::LDRR, 8, 0, 2 },
		{ "LD C,B", &z::LDRR, 8, 0, 2 },
		{ "LD C,C", &z::LDRR, 8, 0, 2 },
		{ "LD C,D", &z::LDRR, 8, 0, 2 },
		{ "LD C,E", &z::LDRR, 8, 0, 2 },
		{ "LD C,IXH", &z::LDRR, 8, 0, 2 },
		{ "LD C,IXL", &z::LDRR, 8, 0, 2 },
		{ "LD C,(IX+0xll)", &z::LDRRi, 19, 0, 3 },
		{ "LD C,A", &z::LDRR, 8, 0, 2 },
		{ "LD D,B", &z::LDRR, 8, 0, 2 },
		{ "LD D,C", &z::LDRR, 8, 0, 2 },
		{ "LD D,D", &z::LDRR, 8, 0, 2 },
		{ "LD D,E", &z::LDRR, 8, 0, 2 },
		{ "LD D,IXH", &z::LDRR, 8, 0, 2 },
		{ "LD D,IXL", &z::LDRR, 8, 0, 2 },
		{ "LD D,(IX+0xll)", &z::LDRRi, 19, 0, 3 },
		{ "LD D,A", &z::LDRR, 8, 0, 2 },
		{ "LD E,B", &z::LDRR, 8, 0, 2 },
		{ "LD E,C", &z::LDRR, 8, 0, 2 },
		{ "LD E,D", &z::LDRR, 8, 0, 2 },
		{ "LD E,E", &z::LDRR, 8, 0, 2 },
		{ "LD E,IXH", &z::LDRR, 8, 0, 2 },
		{ "LD E,IXL", &z::LDRR, 8, 0, 2 },
		{ "LD E,(IX+0xll)", &z::LDRRi, 19, 0, 3 },
		{ "LD E,A", &z::LDRR, 8, 0, 2 },
		{ "LD IXH,B", &z::LDRR, 8, 0, 2 },
		{ "LD IXH,C", &z::LDRR, 8, 0, 2 },
		{ "LD IXH,D", &z::LDRR, 8, 0, 2 },
		{ "LD IXH,E", &z::LDRR, 8, 0, 2 },
		{ "LD IXH,IXH", &z::LDRR, 8, 0, 2 },
		{ "LD IXH,IXL", &z::LDRR, 8, 0, 2 },
		{ "LD H,(IX+0xll)", &z::LDRRi, 19, 0, 3 },
		{ "LD IXH,A", &z::LDRR, 8, 0, 2 },
		{ "LD IXL,B", &z::LDRR, 8, 0, 2 },
		{ "LD IXL,C", &z::LDRR, 8, 0, 2 },
		{ "LD IXL,D", &z::LDRR, 8, 0, 2 },
		{ "LD IXL,E", &z::LDRR, 8, 0, 2 },
		{ "LD IXL,IXH", &z::LDRR, 8, 0, 2 },
		{ "LD IXL,IXL", &z::LDRR, 8, 0, 2 },
		{ "LD L,(IX+0xll)", &z::LDRRi, 19, 0, 3 },
		{ "LD IXL,A", &z::LDRR, 8, 0, 2 },
		{ "LD (IX+0xll),B", &z::LDiRR, 19, 0, 3 },
		{ "LD (IX+0xll),C", &z::LDiRR, 19, 0, 3 },
		{ "LD (IX+0xll),D", &z::LDiRR, 19, 0, 3 },
		{ "LD (IX+0xll),E", &z::LDiRR, 19, 0, 3 },
		{ "LD (IX+0xll),H", &z::LDiRR, 19, 0, 3 },
		{ "LD (IX+0xll),L", &z::LDiRR, 19, 0, 3 },
		{ "HALT ", &z::HALT, 8, 0, 2 },
		{ "LD (IX+0xll),A", &z::LDiRR, 19, 0, 3 },
		{ "LD A,B", &z::LDRR, 8, 0, 2 },
		{ "LD A,C", &z::LDRR, 8, 0, 2 },
		{ "LD A,D", &z::LDRR, 8, 0, 2 },
		{ "LD A,E", &z::LDRR, 8, 0, 2 },
		{ "LD A,IXH", &z::LDRR, 8, 0, 2 },
		{ "LD A,IXL", &z::LDRR, 8, 0, 2 },
		{ "LD A,(IX+0xll)", &z::LDRRi, 19, 0, 3 },
		{ "LD A,A", &z::LDRR, 8, 0, 2 },
		{ "ADD A,B", &z::ADDR, 8, 0, 2 },
		{ "ADD A,C", &z::ADDR, 8, 0, 2 },
		{ "ADD A,D", &z::ADDR, 8, 0, 2 },
		{ "ADD A,E", &z::ADDR, 8, 0, 2 },
		{ "ADD A,IXH", &z::ADDR, 8, 0, 2 },
		{ "ADD A,IXL", &z::ADDR, 8, 0, 2 },
		{ "ADD A,(IX+0xll)", &z::ADDR, 19, 0, 3 },
		{ "ADD A,A", &z::ADDR, 8, 0, 2 },
		{ "ADC A,B", &z::ADCR, 8, 0, 2 },
		{ "ADC A,C", &z::ADCR, 8, 0, 2 },
		{ "ADC A,D", &z::ADCR, 8, 0, 2 },
		{ "ADC A,E", &z::ADCR, 8, 0, 2 },
		{ "ADC A,IXH", &z::ADCR, 8, 0, 2 },
		{ "ADC A,IXL", &z::ADCR, 8, 0, 2 },
		{ "ADC A,(IX+0xll)", &z::ADCR, 19, 0, 3 },
		{ "ADC A,A", &z::ADCR, 8, 0, 2 },
		{ "SUB A,B", &z::SUBR, 8, 0, 2 },
		{ "SUB A,C", &z::SUBR, 8, 0, 2 },
		{ "SUB A,D", &z::SUBR, 8, 0, 2 },
		{ "SUB A,E", &z::SUBR, 8, 0, 2 },
		{ "SUB A,IXH", &z::SUBR, 8, 0, 2 },
		{ "SUB A,IXL", &z::SUBR, 8, 0, 2 },
		{ "SUB A,(IX+0xll)", &z::SUBR, 19, 0, 3 },
		{ "SUB A,A", &z::SUBR, 8, 0, 2 },
		{ "SBC A,B", &z::SBCR, 8, 0, 2 },
		{ "SBC A,C", &z::SBCR, 8, 0, 2 },
		{ "SBC A,D", &z::SBCR, 8, 0, 2 },
		{ "SBC A,E", &z::SBCR, 8, 0, 2 },
		{ "SBC A,IXH", &z::SBCR, 8, 0, 2 },
		{ "SBC A,IXL", &z::SBCR, 8, 0, 2 },
		{ "SBC A,(IX+0xll)", &z::SBCR, 19, 0, 3 },
		{ "SBC A,A", &z::SBCR, 8, 0, 2 },
		{ "AND B", &z::ANDR, 8, 0, 2 },
		{ "AND C", &z::ANDR, 8, 0, 2 },
		{ "AND D", &z::ANDR, 8, 0, 2 },
		{ "AND E", &z::ANDR, 8, 0, 2 },
		{ "AND IXH", &z::ANDR, 8, 0, 2 },
		{ "AND IXL", &z::ANDR, 8, 0, 2 },
		{ "AND (IX+0xll)", &z::ANDR, 19, 0, 3 },
		{ "AND A", &z::ANDR, 8, 0, 2 },
		{ "XOR B", &z::XORR, 8, 0, 2 },
		{ "XOR C", &z::XORR, 8, 0, 2 },
		{ "XOR D", &z::XORR, 8, 0, 2 },
		{ "XOR E", &z::XORR, 8, 0, 2 },
		{ "XOR IXH", &z::XORR, 8, 0, 2 },
		{ "XOR IXL", &z::XORR, 8, 0, 2 },
		{ "XOR (IX+0xll)", &z::XORR, 19, 0, 3 },
		{ "XOR A", &z::XORR, 8, 0, 2 },
		{ "OR B", &z::ORR, 8, 0, 2 },
		{ "OR C", &z::ORR, 8, 0, 2 },
		{ "OR D", &z::ORR, 8, 0, 2 },
		{ "OR E", &z::ORR, 8, 0, 2 },
		{ "OR IXH", &z::ORR, 8, 0, 2 },
		{ "OR IXL", &z::ORR, 8, 0, 2 },
		{ "OR (IX+0xll)", &z::ORR, 19, 0, 3 },
		{ "OR A", &z::ORR, 8, 0, 2 },
		{ "CP B", &z::CPR, 8, 0, 2 },
		{ "CP C", &z::CPR, 8, 0, 2 },
		{ "CP D", &z::CPR, 8, 0, 2 },
		{ "CP E", &z::CPR, 8, 0, 2 },
		{ "CP IXH", &z::CPR, 8, 0, 2 },
		{ "CP IXL", &z::CPR, 8, 0, 2 },
		{ "CP (IX+0xll)", &z::CPR, 19, 0, 3 },
		{ "CP A", &z::CPR, 8, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "POP IX", &z::POPIX, 14, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "EX (SP),IX", &z::EXiSPIX, 23, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "PUSH IX", &z::PUSHIX, 15, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "JP (IX)", &z::JPiIX, 8, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "LD SP,IX", &z::LDSPIX, 10, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },

		//EXT Instruction Set
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "IN B,(C)", &z::INRiC, 12, 0, 2 },
		{ "OUT (C),B", &z::OUTiCR, 12, 0, 2 },
		{ "SBC HL,BC", &z::SBCHLSS, 15, 0, 2 },
		{ "LD (0xhhll),BC", &z::LDiNNDD, 20, 0, 4 },
		{ "NEG ", &z::NEG, 8, 0, 2 },
		{ "RETN ", &z::RETN, 14, 0, 2 },
		{ "IM 0 ", &z::IM0, 8, 0, 2 },
		{ "LD I,A", &z::LDIA, 9, 0, 2 },
		{ "IN C,(C)", &z::INRiC, 12, 0, 2 },
		{ "OUT (C),C", &z::OUTiCR, 12, 0, 2 },
		{ "ADC HL,BC", &z::ADCHLSS, 15, 0, 2 },
		{ "LD BC,(0xhhll)", &z::LDDDiNN, 20, 0, 4 },
		{ "NEG ", &z::NEG, 8, 0, 2 },
		{ "RETI ", &z::RETI, 14, 0, 2 },
		{ "IM 0", &z::IM0, 8, 0, 2 },
		{ "LD R,A", &z::LDRA, 9, 0, 2 },
		{ "IN D,(C)", &z::INRiC, 12, 0, 2 },
		{ "OUT (C),D", &z::OUTiCR, 12, 0, 2 },
		{ "SBC HL,DE", &z::SBCHLSS, 15, 0, 2 },
		{ "LD (0xhhll),DE", &z::LDiNNDD, 20, 0, 4 },
		{ "NEG ", &z::NEG, 8, 0, 2 },
		{ "RETN ", &z::RETN, 14, 0, 2 },
		{ "IM 1 ", &z::IM1, 8, 0, 2 },
		{ "LD A,I", &z::LDAI, 9, 0, 2 },
		{ "IN E,(C)", &z::INRiC, 12, 0, 2 },
		{ "OUT (C),E", &z::OUTiCR, 12, 0, 2 },
		{ "ADC HL,DE", &z::ADCHLSS, 15, 0, 2 },
		{ "LD DE,(0xhhll)", &z::LDDDiNN, 20, 0, 4 },
		{ "NEG ", &z::NEG, 8, 0, 2 },
		{ "RETN ", &z::RETN, 14, 0, 2 },
		{ "IM 2 ", &z::IM2, 8, 0, 2 },
		{ "LD A,R", &z::LDAR, 9, 0, 2 },
		{ "IN H,(C)", &z::INRiC, 12, 0, 2 },
		{ "OUT (C),H", &z::OUTiCR, 12, 0, 2 },
		{ "SBC HL,HL", &z::SBCHLSS, 15, 0, 2 },
		{ "LD (0xhhll),HL", &z::LDiNNDD, 20, 0, 4 },
		{ "NEG ", &z::NEG, 8, 0, 2 },
		{ "RETN ", &z::RETN, 14, 0, 2 },
		{ "IM 0 ", &z::IM0, 8, 0, 2 },
		{ "RRD ", &z::RRD, 18, 0, 2 },
		{ "IN L,(C)", &z::INRiC, 12, 0, 2 },
		{ "OUT (C),L", &z::OUTiCR, 12, 0, 2 },
		{ "ADC HL,HL", &z::ADCHLSS, 15, 0, 2 },
		{ "LD HL,(0xhhll)", &z::LDDDiNN, 20, 0, 4 },
		{ "NEG ", &z::NEG, 8, 0, 2 },
		{ "RETN ", &z::RETN, 14, 0, 2 },
		{ "IM 0", &z::IM0, 8, 0, 2 },
		{ "RLD ", &z::RLD, 18, 0, 2 },
		{ "IN (C)", &z::INiC, 12, 0, 2 },
		{ "OUT (C),0", &z::OUTiC0, 12, 0, 2 },
		{ "SBC HL,SP", &z::SBCHLSS, 15, 0, 2 },
		{ "LD (0xhhll),SP", &z::LDiNNDD, 20, 0, 4 },
		{ "NEG ", &z::NEG, 8, 0, 2 },
		{ "RETN ", &z::RETN, 14, 0, 2 },
		{ "IM 1 ", &z::IM1, 8, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "IN A,(C)", &z::INRiC, 12, 0, 2 },
		{ "OUT (C),A", &z::OUTiCR, 12, 0, 2 },
		{ "ADC HL,SP", &z::ADCHLSS, 15, 0, 2 },
		{ "LD SP,(0xhhll)", &z::LDDDiNN, 40, 0, 4 },
		{ "NEG ", &z::NEG, 8, 0, 2 },
		{ "RETN ", &z::RETN, 14, 0, 2 },
		{ "IM 2 ", &z::IM2, 8, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "LDI ", &z::LDI, 16, 0, 2 },
		{ "CPI ", &z::CPI, 16, 0, 2 },
		{ "INI ", &z::INI, 16, 0, 2 },
		{ "OUTI ", &z::OUTI, 16, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "LDD ", &z::LDD, 16, 0, 2 },
		{ "CPD ", &z::CPD, 16, 0, 2 },
		{ "IND ", &z::IND, 16, 0, 2 },
		{ "OUTD ", &z::OUTD, 16, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "LDIR ", &z::LDIR, 21, 16, 2 },
		{ "CPIR ", &z::CPIR, 21, 16, 2 },
		{ "INIR ", &z::INIR, 21, 16, 2 },
		{ "OTIR ", &z::OTIR, 21, 16, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "LDDR ", &z::LDDR, 21, 16, 2 },
		{ "CPDR ", &z::CPDR, 21, 16, 2 },
		{ "INDR ", &z::INDR, 21, 16, 2 },
		{ "OTDR ", &z::OTDR, 21, 16, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },

		//IY Instruction Set
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "ADD IY,BC", &z::ADDIYSS, 15, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "ADD IY,DE", &z::ADDIYSS, 15, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "LD IY,0xhhll", &z::LDDDNN, 14, 0, 4 },
		{ "LD (0xhhll),IY", &z::LDiNNIY, 20, 0, 4 },
		{ "INC IY", &z::INCIY, 10, 0, 2 },
		{ "INC IYH", &z::INCR, 8, 0, 2 },
		{ "DEC IYH", &z::DECR, 8, 0, 2 },
		{ "LD IYH,0xll", &z::LDRN, 11, 0, 3 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "ADD IY,IY", &z::ADDIYSS, 15, 0, 2 },
		{ "LD IY,(0xhhll)", &z::LDIYiNN, 20, 0, 4 },
		{ "DEC IY", &z::DECIY, 10, 0, 2 },
		{ "INC IYL", &z::INCR, 8, 0, 2 },
		{ "DEC IYL", &z::DECR, 8, 0, 2 },
		{ "LD IYL,0xll", &z::LDRN, 11, 0, 3 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "INC (IY+0xll)", &z::INCR, 23, 0, 3 },
		{ "DEC (IY+0xll)", &z::DECR, 23, 0, 3 },
		{ "LD (IY+0xll),0xhh", &z::LDRN, 19, 0, 4 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "ADD IY,SP", &z::ADDIYSS, 15, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "LD B,B", &z::LDRR, 8, 0, 2 },
		{ "LD B,C", &z::LDRR, 8, 0, 2 },
		{ "LD B,D", &z::LDRR, 8, 0, 2 },
		{ "LD B,E", &z::LDRR, 8, 0, 2 },
		{ "LD B,IYH", &z::LDRR, 8, 0, 2 },
		{ "LD B,IYL", &z::LDRR, 8, 0, 2 },
		{ "LD B,(IY+0xll)", &z::LDRRi, 19, 0, 3 },
		{ "LD B,A", &z::LDRR, 8, 0, 2 },
		{ "LD C,B", &z::LDRR, 8, 0, 2 },
		{ "LD C,C", &z::LDRR, 8, 0, 2 },
		{ "LD C,D", &z::LDRR, 8, 0, 2 },
		{ "LD C,E", &z::LDRR, 8, 0, 2 },
		{ "LD C,IYH", &z::LDRR, 8, 0, 2 },
		{ "LD C,IYL", &z::LDRR, 8, 0, 2 },
		{ "LD C,(IY+0xll)", &z::LDRRi, 19, 0, 3 },
		{ "LD C,A", &z::LDRR, 8, 0, 2 },
		{ "LD D,B", &z::LDRR, 8, 0, 2 },
		{ "LD D,C", &z::LDRR, 8, 0, 2 },
		{ "LD D,D", &z::LDRR, 8, 0, 2 },
		{ "LD D,E", &z::LDRR, 8, 0, 2 },
		{ "LD D,IYH", &z::LDRR, 8, 0, 2 },
		{ "LD D,IYL", &z::LDRR, 8, 0, 2 },
		{ "LD D,(IY+0xll)", &z::LDRRi, 19, 0, 3 },
		{ "LD D,A", &z::LDRR, 8, 0, 2 },
		{ "LD E,B", &z::LDRR, 8, 0, 2 },
		{ "LD E,C", &z::LDRR, 8, 0, 2 },
		{ "LD E,D", &z::LDRR, 8, 0, 2 },
		{ "LD E,E", &z::LDRR, 8, 0, 2 },
		{ "LD E,IYH", &z::LDRR, 8, 0, 2 },
		{ "LD E,IYL", &z::LDRR, 8, 0, 2 },
		{ "LD E,(IY+0xll)", &z::LDRRi, 19, 0, 3 },
		{ "LD E,A", &z::LDRR, 8, 0, 2 },
		{ "LD IYH,B", &z::LDRR, 8, 0, 2 },
		{ "LD IYH,C", &z::LDRR, 8, 0, 2 },
		{ "LD IYH,D", &z::LDRR, 8, 0, 2 },
		{ "LD IYH,E", &z::LDRR, 8, 0, 2 },
		{ "LD IYH,IYH", &z::LDRR, 8, 0, 2 },
		{ "LD IYH,IYL", &z::LDRR, 8, 0, 2 },
		{ "LD H,(IY+0xll)", &z::LDRRi, 19, 0, 3 },
		{ "LD IYH,A", &z::LDRR, 8, 0, 2 },
		{ "LD IYL,B", &z::LDRR, 8, 0, 2 },
		{ "LD IYL,C", &z::LDRR, 8, 0, 2 },
		{ "LD IYL,D", &z::LDRR, 8, 0, 2 },
		{ "LD IYL,E", &z::LDRR, 8, 0, 2 },
		{ "LD IYL,IYH", &z::LDRR, 8, 0, 2 },
		{ "LD IYL,IYL", &z::LDRR, 8, 0, 2 },
		{ "LD L,(IY+0xll)", &z::LDRRi, 19, 0, 3 },
		{ "LD IYL,A", &z::LDRR, 8, 0, 2 },
		{ "LD (IY+0xll),B", &z::LDiRR, 19, 0, 3 },
		{ "LD (IY+0xll),C", &z::LDiRR, 19, 0, 3 },
		{ "LD (IY+0xll),D", &z::LDiRR, 19, 0, 3 },
		{ "LD (IY+0xll),E", &z::LDiRR, 19, 0, 3 },
		{ "LD (IY+0xll),H", &z::LDiRR, 19, 0, 3 },
		{ "LD (IY+0xll),L", &z::LDiRR, 19, 0, 3 },
		{ "HALT ", &z::HALT, 8, 0, 2 },
		{ "LD (IY+0xll),A", &z::LDiRR, 19, 0, 3 },
		{ "LD A,B", &z::LDRR, 8, 0, 2 },
		{ "LD A,C", &z::LDRR, 8, 0, 2 },
		{ "LD A,D", &z::LDRR, 8, 0, 2 },
		{ "LD A,E", &z::LDRR, 8, 0, 2 },
		{ "LD A,IYH", &z::LDRR, 8, 0, 2 },
		{ "LD A,IYL", &z::LDRR, 8, 0, 2 },
		{ "LD A,(IY+0xll)", &z::LDRRi, 19, 0, 3 },
		{ "LD A,A", &z::LDRR, 8, 0, 2 },
		{ "ADD A,B", &z::ADDR, 8, 0, 2 },
		{ "ADD A,C", &z::ADDR, 8, 0, 2 },
		{ "ADD A,D", &z::ADDR, 8, 0, 2 },
		{ "ADD A,E", &z::ADDR, 8, 0, 2 },
		{ "ADD A,IYH", &z::ADDR, 8, 0, 2 },
		{ "ADD A,IYL", &z::ADDR, 8, 0, 2 },
		{ "ADD A,(IY+0xll)", &z::ADDR, 19, 0, 3 },
		{ "ADD A,A", &z::ADDR, 8, 0, 2 },
		{ "ADC A,B", &z::ADCR, 8, 0, 2 },
		{ "ADC A,C", &z::ADCR, 8, 0, 2 },
		{ "ADC A,D", &z::ADCR, 8, 0, 2 },
		{ "ADC A,E", &z::ADCR, 8, 0, 2 },
		{ "ADC A,IYH", &z::ADCR, 8, 0, 2 },
		{ "ADC A,IYL", &z::ADCR, 8, 0, 2 },
		{ "ADC A,(IY+0xll)", &z::ADCR, 19, 0, 3 },
		{ "ADC A,A", &z::ADCR, 8, 0, 2 },
		{ "SUB A,B", &z::SUBR, 8, 0, 2 },
		{ "SUB A,C", &z::SUBR, 8, 0, 2 },
		{ "SUB A,D", &z::SUBR, 8, 0, 2 },
		{ "SUB A,E", &z::SUBR, 8, 0, 2 },
		{ "SUB A,IYH", &z::SUBR, 8, 0, 2 },
		{ "SUB A,IYL", &z::SUBR, 8, 0, 2 },
		{ "SUB A,(IY+0xll)", &z::SUBR, 19, 0, 3 },
		{ "SUB A,A", &z::SUBR, 8, 0, 2 },
		{ "SBC A,B", &z::SBCR, 8, 0, 2 },
		{ "SBC A,C", &z::SBCR, 8, 0, 2 },
		{ "SBC A,D", &z::SBCR, 8, 0, 2 },
		{ "SBC A,E", &z::SBCR, 8, 0, 2 },
		{ "SBC A,IYH", &z::SBCR, 8, 0, 2 },
		{ "SBC A,IYL", &z::SBCR, 8, 0, 2 },
		{ "SBC A,(IY+0xll)", &z::SBCR, 19, 0, 3 },
		{ "SBC A,A", &z::SBCR, 8, 0, 2 },
		{ "AND B", &z::ANDR, 8, 0, 2 },
		{ "AND C", &z::ANDR, 8, 0, 2 },
		{ "AND D", &z::ANDR, 8, 0, 2 },
		{ "AND E", &z::ANDR, 8, 0, 2 },
		{ "AND IYH", &z::ANDR, 8, 0, 2 },
		{ "AND IYL", &z::ANDR, 8, 0, 2 },
		{ "AND (IY+0xll)", &z::ANDR, 19, 0, 3 },
		{ "AND A", &z::ANDR, 8, 0, 2 },
		{ "XOR B", &z::XORR, 8, 0, 2 },
		{ "XOR C", &z::XORR, 8, 0, 2 },
		{ "XOR D", &z::XORR, 8, 0, 2 },
		{ "XOR E", &z::XORR, 8, 0, 2 },
		{ "XOR IYH", &z::XORR, 8, 0, 2 },
		{ "XOR IYL", &z::XORR, 8, 0, 2 },
		{ "XOR (IY+0xll)", &z::XORR, 19, 0, 3 },
		{ "XOR A", &z::XORR, 8, 0, 2 },
		{ "OR B", &z::ORR, 8, 0, 2 },
		{ "OR C", &z::ORR, 8, 0, 2 },
		{ "OR D", &z::ORR, 8, 0, 2 },
		{ "OR E", &z::ORR, 8, 0, 2 },
		{ "OR IYH", &z::ORR, 8, 0, 2 },
		{ "OR IYL", &z::ORR, 8, 0, 2 },
		{ "OR (IY+0xll)", &z::ORR, 19, 0, 3 },
		{ "OR A", &z::ORR, 8, 0, 2 },
		{ "CP B", &z::CPR, 8, 0, 2 },
		{ "CP C", &z::CPR, 8, 0, 2 },
		{ "CP D", &z::CPR, 8, 0, 2 },
		{ "CP E", &z::CPR, 8, 0, 2 },
		{ "CP IYH", &z::CPR, 8, 0, 2 },
		{ "CP IYL", &z::CPR, 8, 0, 2 },
		{ "CP (IY+0xll)", &z::CPR, 19, 0, 3 },
		{ "CP A", &z::CPR, 8, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "POP IY", &z::POPIY, 14, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "EX (SP),IY", &z::EXiSPIY, 23, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "PUSH IY", &z::PUSHIY, 15, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "JP (IY)", &z::JPiIY, 8, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "LD SP,IY", &z::LDSPIY, 10, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },
		{ "XXX ", &z::INVAL, 0, 0, 2 },

		//IX BIT Instruction Set
		{ "RLC (IX+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "RLC (IX+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "RLC (IX+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "RLC (IX+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "RLC (IX+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "RLC (IX+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "RLC (IX+0xll)", &z::RLCR, 23, 0, 4 },
		{ "RLC (IX+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "RRC (IX+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "RRC (IX+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "RRC (IX+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "RRC (IX+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "RRC (IX+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "RRC (IX+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "RRC (IX+0xll)", &z::RRCR, 23, 0, 4 },
		{ "RRC (IX+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "RL (IX+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "RL (IX+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "RL (IX+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "RL (IX+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "RL (IX+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "RL (IX+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "RL (IX+0xll)", &z::RLR, 23, 0, 4 },
		{ "RL (IX+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "RR (IX+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "RR (IX+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "RR (IX+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "RR (IX+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "RR (IX+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "RR (IX+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "RR (IX+0xll)", &z::RRR, 23, 0, 4 },
		{ "RR (IX+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "SLA (IX+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "SLA (IX+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "SLA (IX+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "SLA (IX+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "SLA (IX+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "SLA (IX+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "SLA (IX+0xll)", &z::SLAR, 23, 0, 4 },
		{ "SLA (IX+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "SRA (IX+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "SRA (IX+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "SRA (IX+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "SRA (IX+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "SRA (IX+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "SRA (IX+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "SRA (IX+0xll)", &z::SRAR, 23, 0, 4 },
		{ "SRA (IX+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "SLL (IX+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "SLL (IX+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "SLL (IX+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "SLL (IX+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "SLL (IX+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "SLL (IX+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "SLL (IX+0xll)", &z::SLLR, 23, 0, 4 },
		{ "SLL (IX+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "SRL (IX+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "SRL (IX+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "SRL (IX+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "SRL (IX+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "SRL (IX+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "SRL (IX+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "SRL (IX+0xll)", &z::SRLR, 23, 0, 4 },
		{ "SRL (IX+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "BIT 0,(IX+0xll),B", &z::UNDOC, 20, 0, 4 },
		{ "BIT 0,(IX+0xll),C", &z::UNDOC, 20, 0, 4 },
		{ "BIT 0,(IX+0xll),D", &z::UNDOC, 20, 0, 4 },
		{ "BIT 0,(IX+0xll),E", &z::UNDOC, 20, 0, 4 },
		{ "BIT 0,(IX+0xll),H", &z::UNDOC, 20, 0, 4 },
		{ "BIT 0,(IX+0xll),L", &z::UNDOC, 20, 0, 4 },
		{ "BIT 0,(IX+0xll)", &z::BITBR, 20, 0, 4 },
		{ "BIT 0,(IX+0xll),A", &z::UNDOC, 20, 0, 4 },
		{ "BIT 1,(IX+0xll),B", &z::UNDOC, 20, 0, 4 },
		{ "BIT 1,(IX+0xll),C", &z::UNDOC, 20, 0, 4 },
		{ "BIT 1,(IX+0xll),D", &z::UNDOC, 20, 0, 4 },
		{ "BIT 1,(IX+0xll),E", &z::UNDOC, 20, 0, 4 },
		{ "BIT 1,(IX+0xll),H", &z::UNDOC, 20, 0, 4 },
		{ "BIT 1,(IX+0xll),L", &z::UNDOC, 20, 0, 4 },
		{ "BIT 1,(IX+0xll)", &z::BITBR, 20, 0, 4 },
		{ "BIT 1,(IX+0xll),A", &z::UNDOC, 20, 0, 4 },
		{ "BIT 2,(IX+0xll),B", &z::UNDOC, 20, 0, 4 },
		{ "BIT 2,(IX+0xll),C", &z::UNDOC, 20, 0, 4 },
		{ "BIT 2,(IX+0xll),D", &z::UNDOC, 20, 0, 4 },
		{ "BIT 2,(IX+0xll),E", &z::UNDOC, 20, 0, 4 },
		{ "BIT 2,(IX+0xll),H", &z::UNDOC, 20, 0, 4 },
		{ "BIT 2,(IX+0xll),L", &z::UNDOC, 20, 0, 4 },
		{ "BIT 2,(IX+0xll)", &z::BITBR, 20, 0, 4 },
		{ "BIT 2,(IX+0xll),A", &z::UNDOC, 20, 0, 4 },
		{ "BIT 3,(IX+0xll),B", &z::UNDOC, 20, 0, 4 },
		{ "BIT 3,(IX+0xll),C", &z::UNDOC, 20, 0, 4 },
		{ "BIT 3,(IX+0xll),D", &z::UNDOC, 20, 0, 4 },
		{ "BIT 3,(IX+0xll),E", &z::UNDOC, 20, 0, 4 },
		{ "BIT 3,(IX+0xll),H", &z::UNDOC, 20, 0, 4 },
		{ "BIT 3,(IX+0xll),L", &z::UNDOC, 20, 0, 4 },
		{ "BIT 3,(IX+0xll)", &z::BITBR, 20, 0, 4 },
		{ "BIT 3,(IX+0xll),A", &z::UNDOC, 20, 0, 4 },
		{ "BIT 4,(IX+0xll),B", &z::UNDOC, 20, 0, 4 },
		{ "BIT 4,(IX+0xll),C", &z::UNDOC, 20, 0, 4 },
		{ "BIT 4,(IX+0xll),D", &z::UNDOC, 20, 0, 4 },
		{ "BIT 4,(IX+0xll),E", &z::UNDOC, 20, 0, 4 },
		{ "BIT 4,(IX+0xll),H", &z::UNDOC, 20, 0, 4 },
		{ "BIT 4,(IX+0xll),L", &z::UNDOC, 20, 0, 4 },
		{ "BIT 4,(IX+0xll)", &z::BITBR, 20, 0, 4 },
		{ "BIT 4,(IX+0xll),A", &z::UNDOC, 20, 0, 4 },
		{ "BIT 5,(IX+0xll),B", &z::UNDOC, 20, 0, 4 },
		{ "BIT 5,(IX+0xll),C", &z::UNDOC, 20, 0, 4 },
		{ "BIT 5,(IX+0xll),D", &z::UNDOC, 20, 0, 4 },
		{ "BIT 5,(IX+0xll),E", &z::UNDOC, 20, 0, 4 },
		{ "BIT 5,(IX+0xll),H", &z::UNDOC, 20, 0, 4 },
		{ "BIT 5,(IX+0xll),L", &z::UNDOC, 20, 0, 4 },
		{ "BIT 5,(IX+0xll)", &z::BITBR, 20, 0, 4 },
		{ "BIT 5,(IX+0xll),A", &z::UNDOC, 20, 0, 4 },
		{ "BIT 6,(IX+0xll),B", &z::UNDOC, 20, 0, 4 },
		{ "BIT 6,(IX+0xll),C", &z::UNDOC, 20, 0, 4 },
		{ "BIT 6,(IX+0xll),D", &z::UNDOC, 20, 0, 4 },
		{ "BIT 6,(IX+0xll),E", &z::UNDOC, 20, 0, 4 },
		{ "BIT 6,(IX+0xll),H", &z::UNDOC, 20, 0, 4 },
		{ "BIT 6,(IX+0xll),L", &z::UNDOC, 20, 0, 4 },
		{ "BIT 6,(IX+0xll)", &z::BITBR, 20, 0, 4 },
		{ "BIT 6,(IX+0xll),A", &z::UNDOC, 20, 0, 4 },
		{ "BIT 7,(IX+0xll),B", &z::UNDOC, 20, 0, 4 },
		{ "BIT 7,(IX+0xll),C", &z::UNDOC, 20, 0, 4 },
		{ "BIT 7,(IX+0xll),D", &z::UNDOC, 20, 0, 4 },
		{ "BIT 7,(IX+0xll),E", &z::UNDOC, 20, 0, 4 },
		{ "BIT 7,(IX+0xll),H", &z::UNDOC, 20, 0, 4 },
		{ "BIT 7,(IX+0xll),L", &z::UNDOC, 20, 0, 4 },
		{ "BIT 7,(IX+0xll)", &z::BITBR, 20, 0, 4 },
		{ "BIT 7,(IX+0xll),A", &z::UNDOC, 20, 0, 4 },
		{ "RES 0,(IX+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "RES 0,(IX+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "RES 0,(IX+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "RES 0,(IX+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "RES 0,(IX+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "RES 0,(IX+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "RES 0,(IX+0xll)", &z::RESBR, 23, 0, 4 },
		{ "RES 0,(IX+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "RES 1,(IX+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "RES 1,(IX+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "RES 1,(IX+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "RES 1,(IX+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "RES 1,(IX+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "RES 1,(IX+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "RES 1,(IX+0xll)", &z::RESBR, 23, 0, 4 },
		{ "RES 1,(IX+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "RES 2,(IX+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "RES 2,(IX+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "RES 2,(IX+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "RES 2,(IX+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "RES 2,(IX+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "RES 2,(IX+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "RES 2,(IX+0xll)", &z::RESBR, 23, 0, 4 },
		{ "RES 2,(IX+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "RES 3,(IX+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "RES 3,(IX+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "RES 3,(IX+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "RES 3,(IX+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "RES 3,(IX+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "RES 3,(IX+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "RES 3,(IX+0xll)", &z::RESBR, 23, 0, 4 },
		{ "RES 3,(IX+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "RES 4,(IX+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "RES 4,(IX+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "RES 4,(IX+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "RES 4,(IX+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "RES 4,(IX+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "RES 4,(IX+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "RES 4,(IX+0xll)", &z::RESBR, 23, 0, 4 },
		{ "RES 4,(IX+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "RES 5,(IX+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "RES 5,(IX+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "RES 5,(IX+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "RES 5,(IX+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "RES 5,(IX+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "RES 5,(IX+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "RES 5,(IX+0xll)", &z::RESBR, 23, 0, 4 },
		{ "RES 5,(IX+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "RES 6,(IX+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "RES 6,(IX+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "RES 6,(IX+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "RES 6,(IX+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "RES 6,(IX+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "RES 6,(IX+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "RES 6,(IX+0xll)", &z::RESBR, 23, 0, 4 },
		{ "RES 6,(IX+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "RES 7,(IX+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "RES 7,(IX+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "RES 7,(IX+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "RES 7,(IX+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "RES 7,(IX+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "RES 7,(IX+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "RES 7,(IX+0xll)", &z::RESBR, 23, 0, 4 },
		{ "RES 7,(IX+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "SET 0,(IX+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "SET 0,(IX+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "SET 0,(IX+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "SET 0,(IX+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "SET 0,(IX+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "SET 0,(IX+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "SET 0,(IX+0xll)", &z::SETBR, 23, 0, 4 },
		{ "SET 0,(IX+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "SET 1,(IX+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "SET 1,(IX+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "SET 1,(IX+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "SET 1,(IX+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "SET 1,(IX+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "SET 1,(IX+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "SET 1,(IX+0xll)", &z::SETBR, 23, 0, 4 },
		{ "SET 1,(IX+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "SET 2,(IX+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "SET 2,(IX+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "SET 2,(IX+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "SET 2,(IX+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "SET 2,(IX+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "SET 2,(IX+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "SET 2,(IX+0xll)", &z::SETBR, 23, 0, 4 },
		{ "SET 2,(IX+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "SET 3,(IX+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "SET 3,(IX+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "SET 3,(IX+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "SET 3,(IX+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "SET 3,(IX+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "SET 3,(IX+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "SET 3,(IX+0xll)", &z::SETBR, 23, 0, 4 },
		{ "SET 3,(IX+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "SET 4,(IX+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "SET 4,(IX+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "SET 4,(IX+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "SET 4,(IX+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "SET 4,(IX+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "SET 4,(IX+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "SET 4,(IX+0xll)", &z::SETBR, 23, 0, 4 },
		{ "SET 4,(IX+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "SET 5,(IX+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "SET 5,(IX+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "SET 5,(IX+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "SET 5,(IX+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "SET 5,(IX+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "SET 5,(IX+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "SET 5,(IX+0xll)", &z::SETBR, 23, 0, 4 },
		{ "SET 5,(IX+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "SET 6,(IX+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "SET 6,(IX+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "SET 6,(IX+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "SET 6,(IX+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "SET 6,(IX+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "SET 6,(IX+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "SET 6,(IX+0xll)", &z::SETBR, 23, 0, 4 },
		{ "SET 6,(IX+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "SET 7,(IX+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "SET 7,(IX+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "SET 7,(IX+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "SET 7,(IX+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "SET 7,(IX+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "SET 7,(IX+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "SET 7,(IX+0xll)", &z::SETBR, 23, 0, 4 },
		{ "SET 7,(IX+0xll),A", &z::UNDOC, 23, 0, 4 },

		//IY BIT Instruction Set
		{ "RLC (IY+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "RLC (IY+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "RLC (IY+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "RLC (IY+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "RLC (IY+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "RLC (IY+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "RLC (IY+0xll)", &z::RLCR, 23, 0, 4 },
		{ "RLC (IY+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "RRC (IY+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "RRC (IY+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "RRC (IY+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "RRC (IY+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "RRC (IY+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "RRC (IY+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "RRC (IY+0xll)", &z::RRCR, 23, 0, 4 },
		{ "RRC (IY+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "RL (IY+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "RL (IY+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "RL (IY+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "RL (IY+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "RL (IY+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "RL (IY+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "RL (IY+0xll)", &z::RLR, 23, 0, 4 },
		{ "RL (IY+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "RR (IY+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "RR (IY+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "RR (IY+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "RR (IY+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "RR (IY+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "RR (IY+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "RR (IY+0xll)", &z::RRR, 23, 0, 4 },
		{ "RR (IY+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "SLA (IY+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "SLA (IY+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "SLA (IY+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "SLA (IY+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "SLA (IY+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "SLA (IY+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "SLA (IY+0xll)", &z::SLAR, 23, 0, 4 },
		{ "SLA (IY+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "SRA (IY+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "SRA (IY+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "SRA (IY+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "SRA (IY+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "SRA (IY+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "SRA (IY+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "SRA (IY+0xll)", &z::SRAR, 23, 0, 4 },
		{ "SRA (IY+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "SLL (IY+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "SLL (IY+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "SLL (IY+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "SLL (IY+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "SLL (IY+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "SLL (IY+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "SLL (IY+0xll)", &z::SLLR, 23, 0, 4 },
		{ "SLL (IY+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "SRL (IY+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "SRL (IY+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "SRL (IY+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "SRL (IY+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "SRL (IY+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "SRL (IY+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "SRL (IY+0xll)", &z::SRLR, 23, 0, 4 },
		{ "SRL (IY+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "BIT 0,(IY+0xll),B", &z::UNDOC, 20, 0, 4 },
		{ "BIT 0,(IY+0xll),C", &z::UNDOC, 20, 0, 4 },
		{ "BIT 0,(IY+0xll),D", &z::UNDOC, 20, 0, 4 },
		{ "BIT 0,(IY+0xll),E", &z::UNDOC, 20, 0, 4 },
		{ "BIT 0,(IY+0xll),H", &z::UNDOC, 20, 0, 4 },
		{ "BIT 0,(IY+0xll),L", &z::UNDOC, 20, 0, 4 },
		{ "BIT 0,(IY+0xll)", &z::BITBR, 20, 0, 4 },
		{ "BIT 0,(IY+0xll),A", &z::UNDOC, 20, 0, 4 },
		{ "BIT 1,(IY+0xll),B", &z::UNDOC, 20, 0, 4 },
		{ "BIT 1,(IY+0xll),C", &z::UNDOC, 20, 0, 4 },
		{ "BIT 1,(IY+0xll),D", &z::UNDOC, 20, 0, 4 },
		{ "BIT 1,(IY+0xll),E", &z::UNDOC, 20, 0, 4 },
		{ "BIT 1,(IY+0xll),H", &z::UNDOC, 20, 0, 4 },
		{ "BIT 1,(IY+0xll),L", &z::UNDOC, 20, 0, 4 },
		{ "BIT 1,(IY+0xll)", &z::BITBR, 20, 0, 4 },
		{ "BIT 1,(IY+0xll),A", &z::UNDOC, 20, 0, 4 },
		{ "BIT 2,(IY+0xll),B", &z::UNDOC, 20, 0, 4 },
		{ "BIT 2,(IY+0xll),C", &z::UNDOC, 20, 0, 4 },
		{ "BIT 2,(IY+0xll),D", &z::UNDOC, 20, 0, 4 },
		{ "BIT 2,(IY+0xll),E", &z::UNDOC, 20, 0, 4 },
		{ "BIT 2,(IY+0xll),H", &z::UNDOC, 20, 0, 4 },
		{ "BIT 2,(IY+0xll),L", &z::UNDOC, 20, 0, 4 },
		{ "BIT 2,(IY+0xll)", &z::BITBR, 20, 0, 4 },
		{ "BIT 2,(IY+0xll),A", &z::UNDOC, 20, 0, 4 },
		{ "BIT 3,(IY+0xll),B", &z::UNDOC, 20, 0, 4 },
		{ "BIT 3,(IY+0xll),C", &z::UNDOC, 20, 0, 4 },
		{ "BIT 3,(IY+0xll),D", &z::UNDOC, 20, 0, 4 },
		{ "BIT 3,(IY+0xll),E", &z::UNDOC, 20, 0, 4 },
		{ "BIT 3,(IY+0xll),H", &z::UNDOC, 20, 0, 4 },
		{ "BIT 3,(IY+0xll),L", &z::UNDOC, 20, 0, 4 },
		{ "BIT 3,(IY+0xll)", &z::BITBR, 20, 0, 4 },
		{ "BIT 3,(IY+0xll),A", &z::UNDOC, 20, 0, 4 },
		{ "BIT 4,(IY+0xll),B", &z::UNDOC, 20, 0, 4 },
		{ "BIT 4,(IY+0xll),C", &z::UNDOC, 20, 0, 4 },
		{ "BIT 4,(IY+0xll),D", &z::UNDOC, 20, 0, 4 },
		{ "BIT 4,(IY+0xll),E", &z::UNDOC, 20, 0, 4 },
		{ "BIT 4,(IY+0xll),H", &z::UNDOC, 20, 0, 4 },
		{ "BIT 4,(IY+0xll),L", &z::UNDOC, 20, 0, 4 },
		{ "BIT 4,(IY+0xll)", &z::BITBR, 20, 0, 4 },
		{ "BIT 4,(IY+0xll),A", &z::UNDOC, 20, 0, 4 },
		{ "BIT 5,(IY+0xll),B", &z::UNDOC, 20, 0, 4 },
		{ "BIT 5,(IY+0xll),C", &z::UNDOC, 20, 0, 4 },
		{ "BIT 5,(IY+0xll),D", &z::UNDOC, 20, 0, 4 },
		{ "BIT 5,(IY+0xll),E", &z::UNDOC, 20, 0, 4 },
		{ "BIT 5,(IY+0xll),H", &z::UNDOC, 20, 0, 4 },
		{ "BIT 5,(IY+0xll),L", &z::UNDOC, 20, 0, 4 },
		{ "BIT 5,(IY+0xll)", &z::BITBR, 20, 0, 4 },
		{ "BIT 5,(IY+0xll),A", &z::UNDOC, 20, 0, 4 },
		{ "BIT 6,(IY+0xll),B", &z::UNDOC, 20, 0, 4 },
		{ "BIT 6,(IY+0xll),C", &z::UNDOC, 20, 0, 4 },
		{ "BIT 6,(IY+0xll),D", &z::UNDOC, 20, 0, 4 },
		{ "BIT 6,(IY+0xll),E", &z::UNDOC, 20, 0, 4 },
		{ "BIT 6,(IY+0xll),H", &z::UNDOC, 20, 0, 4 },
		{ "BIT 6,(IY+0xll),L", &z::UNDOC, 20, 0, 4 },
		{ "BIT 6,(IY+0xll)", &z::BITBR, 20, 0, 4 },
		{ "BIT 6,(IY+0xll),A", &z::UNDOC, 20, 0, 4 },
		{ "BIT 7,(IY+0xll),B", &z::UNDOC, 20, 0, 4 },
		{ "BIT 7,(IY+0xll),C", &z::UNDOC, 20, 0, 4 },
		{ "BIT 7,(IY+0xll),D", &z::UNDOC, 20, 0, 4 },
		{ "BIT 7,(IY+0xll),E", &z::UNDOC, 20, 0, 4 },
		{ "BIT 7,(IY+0xll),H", &z::UNDOC, 20, 0, 4 },
		{ "BIT 7,(IY+0xll),L", &z::UNDOC, 20, 0, 4 },
		{ "BIT 7,(IY+0xll)", &z::BITBR, 20, 0, 4 },
		{ "BIT 7,(IY+0xll),A", &z::UNDOC, 20, 0, 4 },
		{ "RES 0,(IY+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "RES 0,(IY+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "RES 0,(IY+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "RES 0,(IY+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "RES 0,(IY+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "RES 0,(IY+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "RES 0,(IY+0xll)", &z::RESBR, 23, 0, 4 },
		{ "RES 0,(IY+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "RES 1,(IY+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "RES 1,(IY+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "RES 1,(IY+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "RES 1,(IY+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "RES 1,(IY+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "RES 1,(IY+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "RES 1,(IY+0xll)", &z::RESBR, 23, 0, 4 },
		{ "RES 1,(IY+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "RES 2,(IY+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "RES 2,(IY+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "RES 2,(IY+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "RES 2,(IY+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "RES 2,(IY+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "RES 2,(IY+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "RES 2,(IY+0xll)", &z::RESBR, 23, 0, 4 },
		{ "RES 2,(IY+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "RES 3,(IY+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "RES 3,(IY+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "RES 3,(IY+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "RES 3,(IY+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "RES 3,(IY+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "RES 3,(IY+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "RES 3,(IY+0xll)", &z::RESBR, 23, 0, 4 },
		{ "RES 3,(IY+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "RES 4,(IY+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "RES 4,(IY+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "RES 4,(IY+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "RES 4,(IY+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "RES 4,(IY+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "RES 4,(IY+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "RES 4,(IY+0xll)", &z::RESBR, 23, 0, 4 },
		{ "RES 4,(IY+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "RES 5,(IY+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "RES 5,(IY+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "RES 5,(IY+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "RES 5,(IY+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "RES 5,(IY+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "RES 5,(IY+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "RES 5,(IY+0xll)", &z::RESBR, 23, 0, 4 },
		{ "RES 5,(IY+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "RES 6,(IY+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "RES 6,(IY+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "RES 6,(IY+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "RES 6,(IY+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "RES 6,(IY+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "RES 6,(IY+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "RES 6,(IY+0xll)", &z::RESBR, 23, 0, 4 },
		{ "RES 6,(IY+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "RES 7,(IY+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "RES 7,(IY+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "RES 7,(IY+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "RES 7,(IY+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "RES 7,(IY+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "RES 7,(IY+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "RES 7,(IY+0xll)", &z::RESBR, 23, 0, 4 },
		{ "RES 7,(IY+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "SET 0,(IY+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "SET 0,(IY+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "SET 0,(IY+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "SET 0,(IY+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "SET 0,(IY+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "SET 0,(IY+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "SET 0,(IY+0xll)", &z::SETBR, 23, 0, 4 },
		{ "SET 0,(IY+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "SET 1,(IY+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "SET 1,(IY+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "SET 1,(IY+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "SET 1,(IY+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "SET 1,(IY+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "SET 1,(IY+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "SET 1,(IY+0xll)", &z::SETBR, 23, 0, 4 },
		{ "SET 1,(IY+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "SET 2,(IY+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "SET 2,(IY+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "SET 2,(IY+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "SET 2,(IY+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "SET 2,(IY+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "SET 2,(IY+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "SET 2,(IY+0xll)", &z::SETBR, 23, 0, 4 },
		{ "SET 2,(IY+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "SET 3,(IY+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "SET 3,(IY+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "SET 3,(IY+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "SET 3,(IY+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "SET 3,(IY+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "SET 3,(IY+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "SET 3,(IY+0xll)", &z::SETBR, 23, 0, 4 },
		{ "SET 3,(IY+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "SET 4,(IY+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "SET 4,(IY+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "SET 4,(IY+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "SET 4,(IY+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "SET 4,(IY+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "SET 4,(IY+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "SET 4,(IY+0xll)", &z::SETBR, 23, 0, 4 },
		{ "SET 4,(IY+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "SET 5,(IY+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "SET 5,(IY+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "SET 5,(IY+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "SET 5,(IY+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "SET 5,(IY+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "SET 5,(IY+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "SET 5,(IY+0xll)", &z::SETBR, 23, 0, 4 },
		{ "SET 5,(IY+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "SET 6,(IY+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "SET 6,(IY+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "SET 6,(IY+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "SET 6,(IY+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "SET 6,(IY+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "SET 6,(IY+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "SET 6,(IY+0xll)", &z::SETBR, 23, 0, 4 },
		{ "SET 6,(IY+0xll),A", &z::UNDOC, 23, 0, 4 },
		{ "SET 7,(IY+0xll),B", &z::UNDOC, 23, 0, 4 },
		{ "SET 7,(IY+0xll),C", &z::UNDOC, 23, 0, 4 },
		{ "SET 7,(IY+0xll),D", &z::UNDOC, 23, 0, 4 },
		{ "SET 7,(IY+0xll),E", &z::UNDOC, 23, 0, 4 },
		{ "SET 7,(IY+0xll),H", &z::UNDOC, 23, 0, 4 },
		{ "SET 7,(IY+0xll),L", &z::UNDOC, 23, 0, 4 },
		{ "SET 7,(IY+0xll)", &z::SETBR, 23, 0, 4 },
		{ "SET 7,(IY+0xll),A", &z::UNDOC, 23, 0, 4 },
	};
}

Z80A::~Z80A()
{
	
}

//Externat Signal Functions
//Clock function extract and execute an entire Istruction from memory if cycles = 0

bool Z80A::clock()
{
	if (cycles == 0)
	{
		// Check if theres INT or NMI pending ----------------------------------
		if (nmi_latch) { NMIEXEC(); cycles--; return true; }
		if (irq_latch) { IRQEXEC(); cycles--; return true; }
		
		//Read First 4 Byte From Memory
		OPB(0) = memRd(PC_);
		OPB(1) = memRd(PC_ + 1);
		OPB(2) = memRd(PC_ + 2);
		OPB(3) = memRd(PC_ + 3);
		
		cbprefix = false;
		ddprefix = false;
		fdprefix = false;
		edprefix = false;
		ddcbprefix = false;
		fdcbprefix = false;

		//Select Instruction Group
		// 0 - MAIN Instruction Set
		// 1 - BIT Instruction Set - CB Prefix
		// 2 - IX Instruction Set - DD Prefix
		// 3 - EXT Instruction Set - ED Prefix
		// 4 - IY Instruction Set - FD Prefix
		// 5 - IX BIT Instruction Set - DD CB Prefix
		// 5 - IY BIT Instruction Set - FD CB Prefix
		switch (OPB(0))
		{
		case 0xcb: indexOp = OPB(1) + 0x100; cbprefix = true; break;
		case 0xdd: indexOp = (OPB(1) != 0xcb) ? (OPB(1) + 0x200) : (OPB(3) + 0x500);
							 (OPB(1) != 0xcb) ? ddprefix = true : ddcbprefix = true;
							 break;
		case 0xed: indexOp = OPB(1) + 0x300; edprefix = true; break;
		case 0xfd: indexOp = (OPB(1) != 0xcb) ? (OPB(1) + 0x400) : (OPB(3) + 0x600);
							 (OPB(1) != 0xcb) ? fdprefix = true : fdcbprefix = true;
							 break;
		default:
			indexOp = OPB(0);
		}

		//Increment PC accordingly to lenght
		PC_ += instSet[indexOp].lenght;

		//Increment R Register according to Opcode Prefix
		// No Prefix:	R = R + 1
		// Prefix:		R = R + 2
		// Register R bit 7 is not changed by the operation
		uint8_t bit = R_ & 0x80;
		(indexOp <= 0xff) ? R_++ : R_ += 2;
		R_ = bit + (R_ & 0x7f);

		//Executes Opcode
		bool bResult = (this->*instSet[indexOp].operate)();

		//Set the number of cycles per current Opcode
		cycles = (bResult) ? instSet[indexOp].cycles1 : instSet[indexOp].cycles2;
	}

	cycles--;
	nCycleCounter++;

	//DEBUG ONLY
	if (cycles == 0)
		bOpcodeComplete = true;
	   	  
	return true;
}

bool Z80A::reset()
{
	//Reset All Registers
	AF_ = 0xffff;	af1.w = 0xffff;
	BC_ = 0x0000;	bc1.w = 0x0000;
	DE_ = 0x0000;	de1.w = 0x0000;
	HL_ = 0x0000;	hl1.w = 0x0000;
	IR_ = 0x0000;
	IX_ = 0x0000;
	IY_ = 0x0000;
	SP_ = 0xffff;
	PC_ = 0x0000;
	
	//Init CPU Status
	bHalt = false;
	bIFF1 = false;
	bIFF2 = false;
	intmode = 0;
	nmi_latch = false;
	irq_latch = false;

	nCycleCounter = 0;

	return true;
}

bool Z80A::irq()
{
	//Store /INT Status till the end of the current instruction. 
	irq_latch = true;

	return true;
}

bool Z80A::nmi()
{
	//Store /NMI Status till the end of the current instruction. 
	nmi_latch = true;

	return true;
}

uint8_t Z80A::memRd(uint16_t addr)
{
	return sms->readMem(addr);
}

uint16_t Z80A::memRd16(uint16_t addr)
{
	uint16_t uResult = sms->readMem(addr) + (sms->readMem(addr + 1) << 8);

	return uResult;
}

bool Z80A::memWr(uint16_t addr, uint8_t data)
{
	sms->writeMem(addr, data);

	return true;
}

bool Z80A::memWr16(uint16_t addr, uint16_t data)
{
	sms->writeMem(addr, (uint8_t)data);
	sms->writeMem(addr + 1, (uint8_t)(data >> 8));

	return true;
}

uint16_t Z80A::stkRd16(uint16_t addr)
{
	uint16_t uResult = sms->readMem(addr) + (sms->readMem(addr + 1) << 8);

	SP_ += 2;

	return uResult;
}

bool Z80A::stkWr16(uint16_t addr, uint16_t data)
{
	sms->writeMem(addr - 2, (uint8_t)data);
	sms->writeMem(addr - 1 , (uint8_t)(data >> 8));

	SP_ -= 2;

	return true;
}

uint8_t Z80A::ioRd(uint8_t addr)
{
	return sms->readIO(addr);
}

bool Z80A::ioWr(uint8_t addr, uint8_t data)
{
	sms->writeIO(addr, data);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                         General Helper Functions
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Evaluate Parity
inline bool Z80A::isP(uint8_t a)
{
	//Check Parity on one Byte in Even

	//return 0 for even number of 1's
	//return 1 for odd number of 1's
	a ^= a >> 4;
	a &= 0xf;
	return !((0x6996 >> a) & 1);
}

//Evaluate Carry
inline bool Z80A::isC(uint32_t res, uint32_t b, uint8_t c, uint8_t bit)
{
	uint32_t a = res - b - c;
	uint32_t uResult = ((a & (0xffffffff >> (32 - bit))) + ((b + c) & (0xffffffff >> (32 - bit)))) >> bit;

	return uResult;
}

//Evaluate Borrow
inline bool Z80A::isB(uint32_t res, uint32_t b, uint8_t c, uint8_t bit)
{
	uint32_t a = res + b + c;
	uint32_t uResult = (a & (0xffffffff >> (32 - bit))) < ((b + c) & (0xffffffff >> (32 - bit)));

	return uResult;
}

//Evaluate Overflow
inline bool Z80A::isO(uint32_t res, uint32_t b, uint8_t c, uint8_t bit)
{
	uint32_t a = res - b - c;
	uint32_t uResult = (b ^ a ^ (1 << (bit - 1))) & ((b ^ res) & (1 << (bit - 1)));
	return uResult;
}

//Evaluate Underflow
inline bool Z80A::isU(uint32_t res, uint32_t b, uint8_t c, uint8_t bit)
{
	uint32_t a = res + b + c;
	uint32_t uResult = (b ^ a) & (a ^ res) & (1 << (bit - 1));
	return uResult;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                         Implementation of all Documented Istruction Set for Z80A Processor
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//8-Bit Load Instructions ------------------------------------------------------------------------------------------
// ---- LD r,r' - LD r,r' (IXH, IXL) - LD r,r' (IYH, IYL)  
// ---- Include Undocumented DD and FD Prefixed OPCODES
bool Z80A::LDRR()
{
	uint8_t d = (ddprefix | fdprefix);

	//Get Value
	switch (OPB(d) & 0x07)
	{
	case 0b000: u8 = B_; break;
	case 0b001: u8 = C_; break;
	case 0b010: u8 = D_; break;
	case 0b011: u8 = E_; break;
	case 0b100: u8 = (ddprefix) ? IXH_ : (fdprefix) ? IYH_ : H_; break;
	case 0b101: u8 = (ddprefix) ? IXL_ : (fdprefix) ? IYL_ : L_; break;
	case 0b110: break; //(HL)
	case 0b111: u8 = A_; break;
	};
	
	//Write Value
	switch ((OPB(d) >> 3) & 0x07)
	{
	case 0b000: B_ = u8; break;
	case 0b001: C_ = u8; break;
	case 0b010: D_ = u8; break;
	case 0b011: E_ = u8; break;
	case 0b100: (ddprefix) ? IXH_ = u8 : (fdprefix) ? IYH_ = u8 : H_ = u8; break;
	case 0b101: (ddprefix) ? IXL_ = u8 : (fdprefix) ? IYL_ = u8 : L_ = u8; break;
	case 0b110: break; //(HL)
	case 0b111: A_ = u8; break;
	};
	   
	return true;
}

// ---- LD (HL),r - LD (IX+d),r - LD (IY+d),r
// ---- Include Undocumented DD and FD Prefixed OPCODES
bool Z80A::LDiRR()
{
	uint8_t d = (ddprefix | fdprefix);

	//Get Value
	switch (OPB(d) & 0x07)
	{
	case 0b000: u8 = B_; break;
	case 0b001: u8 = C_; break;
	case 0b010: u8 = D_; break;
	case 0b011: u8 = E_; break;
	case 0b100: u8 = H_; break;
	case 0b101: u8 = L_; break;
	case 0b110: break;
	case 0b111: u8 = A_; break;
	};

	//Write Value
	(ddprefix) ? memWr(IX_ + (int8_t)OPB(2), u8) : (fdprefix) ? memWr(IY_ + (int8_t)OPB(2), u8) : memWr(HL_, u8);
	
	return true;
}

// ---- LD r,(HL) - LD r,(IX+d) - LD r,(IY+d)
// ---- Include Undocumented DD and FD Prefixed OPCODES
bool Z80A::LDRRi()
{
	uint8_t d = (ddprefix | fdprefix);

	//Get Value
	(ddprefix) ? u8 = memRd(IX_ + (int8_t)OPB(2)) : (fdprefix) ? u8 = memRd(IY_ + (int8_t)OPB(2)) : u8 = memRd(HL_);
	
	//Write Value
	switch ((OPB(d) >> 3) & 0x07)
	{
	case 0b000: B_ = u8; break;
	case 0b001: C_ = u8; break;
	case 0b010: D_ = u8; break;
	case 0b011: E_ = u8; break;
	case 0b100: H_ = u8; break;
	case 0b101: L_ = u8; break;
	case 0b110: break;
	case 0b111: A_ = u8; break;
	};

	return true;
}

// ---- LD r,n - LD (HL),n - LD (IX+d),n - LD (IY+d),n
// ---- Include Undocumented DD and FD Prefixed OPCODES
bool Z80A::LDRN()
{
	uint8_t d = (ddprefix | fdprefix);

	//Write Value
	switch ((OPB(d) >> 3) & 0x07)
	{
	case 0b000: B_ = OPB(1+d*2); break;
	case 0b001: C_ = OPB(1+d*2); break;
	case 0b010: D_ = OPB(1+d*2); break;
	case 0b011: E_ = OPB(1+d*2); break;
	case 0b100: (ddprefix) ? IXH_ = OPB(2) : (fdprefix) ? IYH_ = OPB(2) : H_ = OPB(1); break;
	case 0b101: (ddprefix) ? IXL_ = OPB(2) : (fdprefix) ? IYL_ = OPB(2) : L_ = OPB(1); break;
	case 0b110: (ddprefix) ? memWr(IX_ + (int8_t)OPB(2), OPB(3)) : (fdprefix) ? memWr(IY_ + (int8_t)OPB(2), OPB(3)) : memWr(HL_, OPB(1)); break;
	case 0b111: A_ = OPB(1+d*2); break;
	};

	return true;
}

// ---- LD A,(BC)
bool Z80A::LDAiBC()
{
	//Write to register
	A_ = memRd(BC_);
	
	return true;
}

// ---- LD A,(DE)
bool Z80A::LDAiDE()
{
	//Write to register
	A_ = memRd(DE_);

	return true;
}

// ---- LD A,(0xnnnn)
bool Z80A::LDAiNN()
{
	//Write to register
	A_ = memRd(OPW(1));

	return true;
}

// ---- LD (BC),A
bool Z80A::LDiBCA()
{
	//Write to Memory
	memWr(BC_, A_);

	return true;
}

// ---- LD (DE),A
bool Z80A::LDiDEA()
{
	//Write to Memory
	memWr(DE_, A_);

	return true;
}

// ---- LD (0xnnnn),A
bool Z80A::LDiNNA()
{
	//Write to Memory
	memWr(OPW(1), A_);

	return true;
}

// ---- LD A,I
bool Z80A::LDAI()
{
	//Write to Register
	A_ = I_;
	
	//Set Flags
	SF = (I_ >> 7); ZF = (I_ == 0); HF = 0; PF = bIFF2; NF = 0;
		   
	return true;
}

// ---- LD A,R
bool Z80A::LDAR()
{
	//Write to Register
	A_ = R_;

	//Set Flags
	SF = (R_ >> 7); ZF = (R_ == 0); HF = 0; PF = bIFF2; NF = 0;
			
	return true;
}

// ---- LD I,A
bool Z80A::LDIA()
{
	//Write to Register
	I_ = A_;

	return true;
}

// ---- LD R,A
bool Z80A::LDRA()
{
	//Write to Register
	R_ = A_;

	return true;
}

//16-Bit Load Instructions --------------------------------------------------------------------------------------------------
// ---- LD dd,nn - LD IX,nn - LD IY,nn
bool Z80A::LDDDNN()
{
	uint8_t d = (ddprefix | fdprefix);

	//Write to Register
	switch ((OPB(d) >> 4) & 0x03)
	{
	case 0b00: BC_ = OPW(1); break;
	case 0b01: DE_ = OPW(1); break;
	case 0b10: (ddprefix) ? IX_ = OPW(2) : (fdprefix) ? IY_ = OPW(2) : HL_ = OPW(1); break;
	case 0b11: SP_ = OPW(1); break;
	};

	return true;
}

// ---- LD HL,(nn)
bool Z80A::LDHLiNN()
{
	//Write to Register
	HL_ = memRd16(OPW(1));
	
	return true;
}

// ---- LD dd,(nn)
bool Z80A::LDDDiNN()
{
	//Write to Register
	switch ((OPB(1) >> 4) & 0x03)
	{
	case 0b00: BC_ = memRd16(OPW(2)); break;
	case 0b01: DE_ = memRd16(OPW(2)); break;
	case 0b10: HL_ = memRd16(OPW(2)); break;
	case 0b11: SP_ = memRd16(OPW(2)); break;
	};

	return true;
}

// ---- LD IX,(nn)
bool Z80A::LDIXiNN()
{
	//Write to Register
	IX_ = memRd16(OPW(2));
	
	return true;
}

// ---- LD IY,(nn)
bool Z80A::LDIYiNN()
{
	//Write to Register
	IY_ = memRd16(OPW(2));

	return true;
}

// ---- LD (nn),HL
bool Z80A::LDiNNHL()
{
	//Write to Memory
	memWr16(OPW(1), HL_);
	
	return true;
}

// ---- LD (nn),dd
bool Z80A::LDiNNDD()
{
	//Write to Memory
	switch ((OPB(1) >> 4) & 0x03)
	{
	case 0b00: memWr16(OPW(2), BC_); break;
	case 0b01: memWr16(OPW(2), DE_); break;
	case 0b10: memWr16(OPW(2), HL_); break;
	case 0b11: memWr16(OPW(2), SP_); break;
	};

	return true;
}

// ---- LD (nn),IX
bool Z80A::LDiNNIX()
{
	//Write to Memory
	memWr16(OPW(2), IX_);
	
	return true;
}

// ---- LD (nn),IY
bool Z80A::LDiNNIY()
{
	//Write to Memory
	memWr16(OPW(2), IY_);

	return true;
}

// ---- LD SP,HL
bool Z80A::LDSPHL()
{
	//Write to Register
	SP_ = HL_;

	return true;
}

// ---- LD SP,IX
bool Z80A::LDSPIX()
{
	//Write to Register
	SP_ = IX_;

	return true;
}

// ---- LD SP,IY
bool Z80A::LDSPIY()
{
	//Write to Register
	SP_ = IY_;

	return true;
}

// ---- PUSH qq
bool Z80A::PUSHQQ()
{
	//Write to Stack
	switch ((OPB(0) >> 4) & 0x03)
	{
	case 0b00: stkWr16(SP_, BC_); break;
	case 0b01: stkWr16(SP_, DE_); break;
	case 0b10: stkWr16(SP_, HL_); break;
	case 0b11: stkWr16(SP_, AF_); break;
	};
	
	return true;
}

// ---- PUSH IX
bool Z80A::PUSHIX()
{
	//Write to Stack
	stkWr16(SP_, IX_);
	
	return true;
}

// ---- PUSH IY
bool Z80A::PUSHIY()
{
	//Write to Stack
	stkWr16(SP_, IY_);

	return true;
}

// ---- POP qq
bool Z80A::POPQQ()
{
	//Read from Stack
	switch ((OPB(0) >> 4) & 0x03)
	{
	case 0b00: BC_ = stkRd16(SP_); break;
	case 0b01: DE_ = stkRd16(SP_); break;
	case 0b10: HL_ = stkRd16(SP_); break;
	case 0b11: AF_ = stkRd16(SP_); break;
	};

	return true;
}

// ---- POP IX
bool Z80A::POPIX()
{
	//Read from Stack
	IX_ = stkRd16(SP_);
	
	return true;
}

// ---- POP IY
bool Z80A::POPIY()
{
	//Read from Stack
	IY_ = stkRd16(SP_);
	
	return true;
}

//Exchange, Block Transfer and Search Instructions ------------------------------------------------------------------------
// ---- EX DE,HL
bool Z80A::EXDEHL()
{
	WZ_ = HL_;
	HL_ = DE_;
	DE_ = WZ_;
	
	return true;
}

// ---- EX AF,AF'
bool Z80A::EXAF()
{
	WZ_ = AF_;
	AF_ = af1.w;
	af1.w = WZ_;
	
	return true;
}

// ---- EXX
bool Z80A::EXX()
{
	WZ_ = BC_; BC_ = bc1.w; bc1.w = WZ_;
	WZ_ = DE_; DE_ = de1.w; de1.w = WZ_;
	WZ_ = HL_; HL_ = hl1.w; hl1.w = WZ_;
	
	return true;
}

// ---- EX (SP), HL
bool Z80A::EXiSPHL()
{
	WZ_ = memRd16(SP_);
	memWr16(SP_, HL_);
	HL_ = WZ_;
		   
	return true;
}

// ---- EX (SP), IX
bool Z80A::EXiSPIX()
{
	WZ_ = memRd16(SP_);
	memWr16(SP_, IX_);
	IX_ = WZ_;

	return true;
}

// ---- EX (SP), IY
bool Z80A::EXiSPIY()
{
	WZ_ = memRd16(SP_);
	memWr16(SP_, IY_);
	IY_ = WZ_;

	return true;
}

// ---- LDI
bool Z80A::LDI()
{
	//Copy Src to Dst Memory
	memWr(DE_, memRd(HL_));
		
	//Update Counter Register
	DE_++; HL_++; BC_--;
	
	//Update Flags
	HF = 0;  PF = (BC_ != 0); NF = 0;
		
	return true;
}

// ---- LDIR
bool Z80A::LDIR()
{
	LDI();

	if (BC_ != 0)
	{
		//Repeat OPCode until BC = 0
		PC_ -= 2;
		return true;
	}

	return false;
}

// ---- LDD
bool Z80A::LDD()
{
	//Copy Src to Dst Memory
	memWr(DE_, memRd(HL_));
	
	//Update Counter Register
	DE_--; HL_--; BC_--;
	
	//Update Flags
	HF = 0; PF = (BC_ != 0); NF = 0;
		
	return true;
}

// ---- LDDR
bool Z80A::LDDR()
{
	LDD();

	if (BC_ != 0)
	{
		//Repeat Opcode until BC = 0
		PC_ -= 2;
		return true;
	}

	return false;
}

// ---- CPI
bool Z80A::CPI()
{
	u8 = memRd(HL_);
	res = A_ - u8;

	//Update Counter Register
	HL_++; BC_--;
			
	//Update Flags
	SF = (res >> 7); ZF = (res == 0); HF = isB(res, u8, 0, 4); PF = (BC_ != 0); NF = 1;
			
	return true;
}

// ---- CPIR
bool Z80A::CPIR()
{
	CPI();

	if ((BC_ != 0) && (res != 0))
	{
		//Repeat Opcode
		PC_ -= 2;
		return true;
	}
	
	return false;
}

// ---- CPD
bool Z80A::CPD()
{
	u8 = memRd(HL_);
	res = A_ - u8;
		
	//Update Counter Register
	HL_--; BC_--;
	
	//Update Flags
	SF = (res >> 7); ZF = (res == 0); HF = isB(res, u8, 0, 4); PF = (BC_ != 0); NF = 1;
			
	return true;
}

// ---- CPDR
bool Z80A::CPDR()
{
	CPD();

	if ((BC_ != 0) && (res != 0))
	{
		//Repeat Opcode
		PC_ -= 2;
		return true;
	}

	return false;
}

//8-Bit Arithmetic Instructions ---------------------------------------------------------------------------------------------
// ---- ADD A,r - ADD A,(HL) - ADD A,(IX+d), ADD A,(IY+d)
// ---- Include Undocumented DD and FD Prefixed OPCODES
bool Z80A::ADDR()
{
	uint8_t d = (ddprefix | fdprefix);

	//Read from Register
	switch (OPB(d) & 0x07)
	{
	case 0b000: u8 = B_; break;
	case 0b001: u8 = C_; break;
	case 0b010: u8 = D_; break;
	case 0b011: u8 = E_; break;
	case 0b100: u8 = (ddprefix) ? IXH_ : (fdprefix) ? IYH_ : H_; break;
	case 0b101: u8 = (ddprefix) ? IXL_ : (fdprefix) ? IYL_ : L_; break;
	case 0b110: u8 = (ddprefix) ? memRd(IX_ + (int8_t)OPB(2)) : (fdprefix) ? memRd(IY_ + (int8_t)OPB(2)) : memRd(HL_); break;
	case 0b111: u8 = A_; break;
	};

	//Update Accumulator
	A_ += u8;
	
	//Update Flags
	SF = (A_ >> 7); ZF = (A_ == 0); HF = isC(A_, u8, 0, 4); PF = isO(A_, u8, 0, 8); NF = 0; CF = isC(A_, u8, 0, 8);
		   	
	return true;
}

// ---- ADD A,n
bool Z80A::ADDN()
{
	//Update Accumulator
	u8 = OPB(1);
	A_ += u8;

	//Update Flags
	SF = (A_ >> 7); ZF = (A_ == 0); HF = isC(A_, u8, 0, 4); PF = isO(A_, u8, 0, 8); NF = 0; CF = isC(A_, u8, 0, 8);
	
	return true;
}

// ---- ADC A,r - ADC A,(HL) - ADC A,(IX+d), ADC A,(IY+d)
// ---- Include Undocumented DD and FD Prefixed OPCODES
bool Z80A::ADCR()
{
	uint8_t d = (ddprefix | fdprefix);

	//Read from Register
	switch (OPB(d) & 0x07)
	{
	case 0b000: u8 = B_; break;
	case 0b001: u8 = C_; break;
	case 0b010: u8 = D_; break;
	case 0b011: u8 = E_; break;
	case 0b100: u8 = (ddprefix) ? IXH_ : (fdprefix) ? IYH_ : H_; break;
	case 0b101: u8 = (ddprefix) ? IXL_ : (fdprefix) ? IYL_ : L_; break;
	case 0b110: u8 = (ddprefix) ? memRd(IX_ + (int8_t)OPB(2)) : (fdprefix) ? memRd(IY_ + (int8_t)OPB(2)) : memRd(HL_); break;
	case 0b111: u8 = A_; break;
	};

	//Update Accumulator
	A_ += (u8 + CF);

	//Update Flags
	SF = (A_ >> 7); ZF = (A_ == 0); HF = isC(A_, u8, CF, 4); PF = isO(A_, u8, CF, 8); NF = 0; CF = isC(A_, u8, CF, 8);
	
	return true;
}

// ---- ADC A,n
bool Z80A::ADCN()
{
	//Update Accumulator
	u8 = OPB(1);
	A_ += (u8 + CF);

	//Update Flags
	SF = (A_ >> 7); ZF = (A_ == 0); HF = isC(A_, u8, CF, 4); PF = isO(A_, u8, CF, 8); NF = 0; CF = isC(A_, u8, CF, 8);
	
	return true;
}

// ---- SUB A,r - SUB A,(HL) - SUB A,(IX+d), SUB A,(IY+d)
// ---- Include Undocumented DD and FD Prefixed OPCODES
bool Z80A::SUBR()
{
	uint8_t d = (ddprefix | fdprefix);

	//Read from Register
	switch (OPB(d) & 0x07)
	{
	case 0b000: u8 = B_; break;
	case 0b001: u8 = C_; break;
	case 0b010: u8 = D_; break;
	case 0b011: u8 = E_; break;
	case 0b100: u8 = (ddprefix) ? IXH_ : (fdprefix) ? IYH_ : H_; break;
	case 0b101: u8 = (ddprefix) ? IXL_ : (fdprefix) ? IYL_ : L_; break;
	case 0b110: u8 = (ddprefix) ? memRd(IX_ + (int8_t)OPB(2)) : (fdprefix) ? memRd(IY_ + (int8_t)OPB(2)) : memRd(HL_); break;
	case 0b111: u8 = A_; break;
	};

	//Update Accumulator
	A_ -= u8;
	
	//Update Flags
	SF = (A_ >> 7); ZF = (A_ == 0); HF = isB(A_, u8, 0, 4); PF = isU(A_, u8, 0, 8); NF = 1; CF = isB(A_, u8, 0, 8);
	
	return true;
}

// ---- SUB n
bool Z80A::SUBN()
{
	//Update Accumulator
	u8 = OPB(1);
	A_ -= u8;

	//Update Flags
	SF = (A_ >> 7); ZF = (A_ == 0); HF = isB(A_, u8, 0, 4); PF = isU(A_, u8, 0, 8); NF = 1; CF = isB(A_, u8, 0, 8);

	return true;
}

// ---- SBC A,r - SBC A,(HL) - SBC A,(IX+d), SBC A,(IY+d)
// ---- Include Undocumented DD and FD Prefixed OPCODES
bool Z80A::SBCR()
{
	uint8_t d = (ddprefix | fdprefix);

	//Read from Register
	switch (OPB(d) & 0x07)
	{
	case 0b000: u8 = B_; break;
	case 0b001: u8 = C_; break;
	case 0b010: u8 = D_; break;
	case 0b011: u8 = E_; break;
	case 0b100: u8 = (ddprefix) ? IXH_ : (fdprefix) ? IYH_ : H_; break;
	case 0b101: u8 = (ddprefix) ? IXL_ : (fdprefix) ? IYL_ : L_; break;
	case 0b110: u8 = (ddprefix) ? memRd(IX_ + (int8_t)OPB(2)) : (fdprefix) ? memRd(IY_ + (int8_t)OPB(2)) : memRd(HL_); break;
	case 0b111: u8 = A_; break;
	};
	
	//Update Accumulator
	A_ -= (u8 + CF);

	//Update Flags
	SF = (A_ >> 7); ZF = (A_ == 0); HF = isB(A_, u8, CF, 4); PF = isU(A_, u8, CF, 8); NF = 1; CF = isB(A_, u8, CF, 8);
	
	return true;
}

// ---- SBC A,n 
bool Z80A::SBCN()
{
	//Update Accumulator
	u8 = OPB(1);
	A_ -= (u8 + CF);

	//Update Flags
	SF = (A_ >> 7); ZF = (A_ == 0); HF = isB(A_, u8, CF, 4); PF = isU(A_, u8, CF, 8); NF = 1; CF = isB(A_, u8, CF, 8);

	return true;
}

// ---- AND A,r - AND A,(HL) - AND A,(IX+d), AND A,(IY+d)
// ---- Include Undocumented DD and FD Prefixed OPCODES
bool Z80A::ANDR()
{
	uint8_t d = (ddprefix | fdprefix);

	//Read from Register
	switch (OPB(d) & 0x07)
	{
	case 0b000: u8 = B_; break;
	case 0b001: u8 = C_; break;
	case 0b010: u8 = D_; break;
	case 0b011: u8 = E_; break;
	case 0b100: u8 = (ddprefix) ? IXH_ : (fdprefix) ? IYH_ : H_; break;
	case 0b101: u8 = (ddprefix) ? IXL_ : (fdprefix) ? IYL_ : L_; break;
	case 0b110: u8 = (ddprefix) ? memRd(IX_ + (int8_t)OPB(2)) : (fdprefix) ? memRd(IY_ + (int8_t)OPB(2)) : memRd(HL_); break;
	case 0b111: u8 = A_; break;
	};

	//Update Accumulator
	A_ &= u8;
	
	//Update Flags
	SF = (A_ >> 7); ZF = (A_ == 0); HF = 1; PF = isP(A_); NF = 0; CF = 0;
	
	return true;
}

// ---- AND n
bool Z80A::ANDN()
{
	//Update Accumulator
	A_ &= OPB(1);

	//Update Flags
	SF = (A_ >> 7); ZF = (A_ == 0); HF = 1; PF = isP(A_); NF = 0; CF = 0;
		
	return true;
}

// ---- OR A,r - OR A,(HL) - OR A,(IX+d), OR A,(IY+d)
// ---- Include Undocumented DD and FD Prefixed OPCODES
bool Z80A::ORR()
{
	uint8_t d = (ddprefix | fdprefix);

	//Read from Register
	switch (OPB(d) & 0x07)
	{
	case 0b000: u8 = B_; break;
	case 0b001: u8 = C_; break;
	case 0b010: u8 = D_; break;
	case 0b011: u8 = E_; break;
	case 0b100: u8 = (ddprefix) ? IXH_ : (fdprefix) ? IYH_ : H_; break;
	case 0b101: u8 = (ddprefix) ? IXL_ : (fdprefix) ? IYL_ : L_; break;
	case 0b110: u8 = (ddprefix) ? memRd(IX_ + (int8_t)OPB(2)) : (fdprefix) ? memRd(IY_ + (int8_t)OPB(2)) : memRd(HL_); break;
	case 0b111: u8 = A_; break;
	};

	//Update Accumulator
	A_ |= u8;
	
	//Update Flags
	SF = (A_ >> 7); ZF = (A_ == 0); HF = 0; PF = isP(A_); NF = 0; CF = 0;
	
	return true;
}

// ---- OR n
bool Z80A::ORN()
{
	//Update Accumulator
	A_ |= OPB(1);

	//Update Flags
	SF = (A_ >> 7); ZF = (A_ == 0); HF = 0; PF = isP(A_); NF = 0; CF = 0;

	return true;
}

// ---- XOR A,r - XOR A,(HL) - XOR A,(IX+d), XOR A,(IY+d)
// ---- Include Undocumented DD and FD Prefixed OPCODES
bool Z80A::XORR()
{
	uint8_t d = (ddprefix | fdprefix);

	//Read from Register
	switch (OPB(d) & 0x07)
	{
	case 0b000: u8 = B_; break;
	case 0b001: u8 = C_; break;
	case 0b010: u8 = D_; break;
	case 0b011: u8 = E_; break;
	case 0b100: u8 = (ddprefix) ? IXH_ : (fdprefix) ? IYH_ : H_; break;
	case 0b101: u8 = (ddprefix) ? IXL_ : (fdprefix) ? IYL_ : L_; break;
	case 0b110: u8 = (ddprefix) ? memRd(IX_ + (int8_t)OPB(2)) : (fdprefix) ? memRd(IY_ + (int8_t)OPB(2)) : memRd(HL_); break;
	case 0b111: u8 = A_; break;
	};

	//Update Accumulator
	A_ ^= u8;
	
	//Update Flags
	SF = (A_ >> 7); ZF = (A_ == 0); HF = 0; PF = isP(A_); NF = 0; CF = 0;
		
	return true;
}

// ---- XOR n
bool Z80A::XORN()
{
	//Update Accumulator
	A_ ^= OPB(1);

	//Update Flags
	SF = (A_ >> 7); ZF = (A_ == 0); HF = 0; PF = isP(A_); NF = 0; CF = 0;

	return true;
}

// ---- CP A,r - CP A,(HL) - CP A,(IX+d), CP A,(IY+d)
// ---- Include Undocumented DD and FD Prefixed OPCODES
bool Z80A::CPR()
{
	uint8_t d = (ddprefix | fdprefix);

	//Read from Register
	switch (OPB(d) & 0x07)
	{
	case 0b000: u8 = B_; break;
	case 0b001: u8 = C_; break;
	case 0b010: u8 = D_; break;
	case 0b011: u8 = E_; break;
	case 0b100: u8 = (ddprefix) ? IXH_ : (fdprefix) ? IYH_ : H_; break;
	case 0b101: u8 = (ddprefix) ? IXL_ : (fdprefix) ? IYL_ : L_; break;
	case 0b110: u8 = (ddprefix) ? memRd(IX_ + (int8_t)OPB(2)) : (fdprefix) ? memRd(IY_ + (int8_t)OPB(2)) : memRd(HL_); break;
	case 0b111: u8 = A_; break;
	};
	
	//Evaluate Result
	res = A_ - u8;

	//Update Flags
	SF = (res >> 7); ZF = (res == 0); HF = isB(res, u8, 0, 4); PF = isU(res, u8, 0, 8); NF = 1; CF = isB(res, u8, 0, 8);
		
	return true;
}

// ---- CP n
bool Z80A::CPN()
{
	//Evaluate Result
	u8 = OPB(1);
	res = A_ - u8;

	//Update Flags
	SF = (res >> 7); ZF = (res == 0); HF = isB(res, u8, 0, 4); PF = isU(res, u8, 0, 8); NF = 1; CF = isB(res, u8, 0, 8);

	return true;
}

// ---- INC r - INC (HL), INC (IX+d) - INC (IY+d)
// ---- Include Undocumented DD and FD Prefixed OPCODES
bool Z80A::INCR()
{
	uint8_t d = (ddprefix | fdprefix);

	//Read from Register
	switch ((OPB(d) >> 3) & 0x07)
	{
	case 0b000: u8 = ++B_; break;
	case 0b001: u8 = ++C_; break;
	case 0b010: u8 = ++D_; break;
	case 0b011: u8 = ++E_; break;
	case 0b100: u8 = (ddprefix) ? ++IXH_ : (fdprefix) ? ++IYH_ : ++H_; break;
	case 0b101: u8 = (ddprefix) ? ++IXL_ : (fdprefix) ? ++IYL_ : ++L_; break;
	case 0b110: (ddprefix) ? memWr(IX_ + (int8_t)OPB(2), u8 = memRd(IX_ + (int8_t)OPB(2)) + 1) : 
		        (fdprefix) ? memWr(IY_ + (int8_t)OPB(2), u8 = memRd(IY_ + (int8_t)OPB(2)) + 1) : 
							 memWr(HL_, u8 = memRd(HL_)+1); break;
	case 0b111: u8 = ++A_; break;
	};
			
	//Update Flags
	SF = (u8 >> 7); ZF = (u8 == 0); HF = isC(u8, 1, 0, 4); PF = (u8 == 0x80); NF = 0;
		
	return true;
}

// ---- DEC r - DEC (HL), DEC (IX+d) - DEC (IY+d)
// ---- Include Undocumented DD and FD Prefixed OPCODES
bool Z80A::DECR()
{
	uint8_t d = (ddprefix | fdprefix);

	//Read from Register
	switch ((OPB(d) >> 3) & 0x07)
	{
	case 0b000: u8 = --B_; break;
	case 0b001: u8 = --C_; break;
	case 0b010: u8 = --D_; break;
	case 0b011: u8 = --E_; break;
	case 0b100: u8 = (ddprefix) ? --IXH_ : (fdprefix) ? --IYH_ : --H_; break;
	case 0b101: u8 = (ddprefix) ? --IXL_ : (fdprefix) ? --IYL_ : --L_; break;
	case 0b110: (ddprefix) ? memWr(IX_ + (int8_t)OPB(2), u8 = memRd(IX_ + (int8_t)OPB(2)) - 1) :
				(fdprefix) ? memWr(IY_ + (int8_t)OPB(2), u8 = memRd(IY_ + (int8_t)OPB(2)) - 1) :
		                     memWr(HL_, u8 = memRd(HL_) - 1); break;
	case 0b111: u8 = --A_; break;
	};

	//Update Flags
	SF = (u8 >> 7); ZF = (u8 == 0); HF = isB(u8, 1, 0, 4); PF = (u8 == 0x7f); NF = 1;
	
	return true;
}

//General Purpose Arithmetic and CPU Control Instructions -------------------------------------------------------------------------
// ---- DAA
bool Z80A::DAA()
{
	int status = 0;

	if (HF || ((A_ & 0x0f) > 9))
		status++;

	if (CF || (A_ > 0x99))
	{
		status += 2;
		CF = 1;
	}

	// builds final H flag
	if (NF && !HF)
	{
		HF = 0;
	}
	else
	{
		if (NF && HF)
			HF = ((A_ & 0x0f) < 6);
		else
			HF = ((A_ & 0x0f) >= 0x0a);
	}
	
	//Update Accumulator
	switch (status)
	{
	case 1: A_ += NF ? 0xfa : 0x06; break;
	case 2: A_ += NF ? 0xa0 : 0x60; break;
	case 3: A_ += NF ? 0x9a : 0x66; break;
	}

	//Update Flags
	SF = (A_ >> 7); ZF = (A_ == 0); PF = isP(A_);
		
	return true;
}

// ---- CPL 
bool Z80A::CPL()
{
	A_ = ~A_;
	
	//Update Flags
	HF = 1; NF = 1;
		
	return true;
}

// ---- NEG 
bool Z80A::NEG()
{
	u8 = A_;
	A_ = 0x00 - A_;
	
	//Update Flags
	SF = (A_ >> 7); ZF = (A_ == 0); HF = isB(A_, u8, 0, 4); PF = (u8 == 0x80); NF = 1; CF = (u8 != 0);
	
	return true;
}

// ---- CCF 
bool Z80A::CCF()
{
	//Update Flags
	HF = CF; NF = 0; CF = !CF;
	
	return true;
}

// ---- SCF 
bool Z80A::SCF()
{
	//Update Flags
	HF = 0; NF = 0; CF = 1;

	return true;
}

// ---- NOP 
bool Z80A::NOP()
{
	//Do Nothing!
	return true;
}

// ---- HALT 
bool Z80A::HALT()
{
	//Loop Until Interrupt Occurs
	bHalt = true;
	PC_--;

	return true;
}

// ---- DI 
bool Z80A::DI()
{
	bIFF1 = false;
	bIFF2 = false;

	return true;
}

// ---- EI 
bool Z80A::EI()
{
	bIFF1 = true;
	bIFF2 = true;

	return true;
}

// ---- IM 0
bool Z80A::IM0()
{
	intmode = 0;

	return true;
}

// ---- IM 1
bool Z80A::IM1()
{
	intmode = 1;

	return true;
}

// ---- IM 2
bool Z80A::IM2()
{
	intmode = 2;

	return true;
}

//16-Bit Arithmetic Instructions -----------------------------------------------------------------------------------------
// ---- ADD HL,ss
bool Z80A::ADDHLSS()
{
	switch ((OPB(0) >> 4) & 0x03)
	{
	case 0b00: u16 = BC_; break;
	case 0b01: u16 = DE_; break;
	case 0b10: u16 = HL_; break;
	case 0b11: u16 = SP_; break;
	};
	
	//Update Register
	HL_ += u16;
	   
	//Update FLAGS
	HF = isC(HL_, u16, 0, 12); NF = 0; CF = isC(HL_, u16, 0, 16);
	
	return true;
}

// ---- ADC HL,ss
bool Z80A::ADCHLSS()
{
	switch ((OPB(1) >> 4) & 0x03)
	{
	case 0b00: u16 = BC_; break;
	case 0b01: u16 = DE_; break;
	case 0b10: u16 = HL_; break;
	case 0b11: u16 = SP_; break;
	};

	//Update Register
	HL_ += (u16 + CF);

	//Update FLAGS
	SF = (HL_ >> 15); ZF = (HL_ == 0); HF = isC(HL_, u16, CF, 12); PF = isO(HL_, u16, CF, 16); NF = 0; CF = isC(HL_, u16, CF, 16);
	
	return true;
}

// ---- SBC HL,ss
bool Z80A::SBCHLSS()
{
	switch ((OPB(1) >> 4) & 0x03)
	{
	case 0b00: u16 = BC_; break;
	case 0b01: u16 = DE_; break;
	case 0b10: u16 = HL_; break;
	case 0b11: u16 = SP_; break;
	};

	//Update Register
	HL_ -= (u16 + CF);

	//Update FLAGS
	SF = (HL_ >> 15); ZF = (HL_ == 0); HF = isB(HL_, u16, CF, 12); PF = isU(HL_, u16, CF, 16); NF = 1; CF = isB(HL_, u16, CF, 16);

	return true;
}

// ---- ADD IX,ss
bool Z80A::ADDIXSS()
{
	switch ((OPB(1) >> 4) & 0x03)
	{
	case 0b00: u16 = BC_; break;
	case 0b01: u16 = DE_; break;
	case 0b10: u16 = IX_; break;
	case 0b11: u16 = SP_; break;
	};
	
	//Update Register
	IX_ += u16;

	//Update FLAGS
	HF = isC(IX_, u16, 0, 12); NF = 0; CF = isC(IX_, u16, 0, 16);
	
	return true;
}

// ---- ADD IY,ss
bool Z80A::ADDIYSS()
{
	switch ((OPB(1) >> 4) & 0x03)
	{
	case 0b00: u16 = BC_; break;
	case 0b01: u16 = DE_; break;
	case 0b10: u16 = IY_; break;
	case 0b11: u16 = SP_; break;
	};

	//Update Register
	IY_ += u16;

	//Update FLAGS
	HF = isC(IY_, u16, 0, 12); NF = 0; CF = isC(IY_, u16, 0, 16);

	return true;
}

// ---- INC ss
bool Z80A::INCSS()
{
	switch ((OPB(0) >> 4) & 0x03)
	{
	case 0b00: BC_++; break;
	case 0b01: DE_++; break;
	case 0b10: HL_++; break;
	case 0b11: SP_++; break;
	};
	
	return true;
}

// ---- INC IX
bool Z80A::INCIX()
{
	IX_++;

	return true;
}

// ---- INC IY
bool Z80A::INCIY()
{
	IY_++;

	return true;
}

// ---- DEC ss
bool Z80A::DECSS()
{
	switch ((OPB(0) >> 4) & 0x03)
	{
	case 0b00: BC_--; break;
	case 0b01: DE_--; break;
	case 0b10: HL_--; break;
	case 0b11: SP_--; break;
	};
		
	return true;
}

// ---- DEC IX
bool Z80A::DECIX()
{
	IX_--;

	return true;
}

// ---- DEC IY
bool Z80A::DECIY()
{
	IY_--;

	return true;
}

//Rotate and Shift Instructions
// ---- RLCA
bool Z80A::RLCA()
{
	//Update Accumulator
	CF = A_ >> 7;
	A_ = (A_ << 1) + CF;
	
	//Update FLAGS
	HF = 0; NF = 0;
		   	 
	return true;
}

// ---- RLA
bool Z80A::RLA()
{
	//Update Accumulator
	u8 = CF;
	CF = A_ >> 7;
	A_ = (A_ << 1) + u8;
	
	//Update FLAGS
	HF = 0; NF = 0;

	return true;
}

// ---- RRCA
bool Z80A::RRCA()
{
	//Update Accumulator
	CF = A_ & 0x01;
	A_ = (A_ >> 1) | (CF << 7);

	//Update FLAGS
	HF = 0; NF = 0;

	return true;
}

// ---- RRA
bool Z80A::RRA()
{
	//Update Accumulator
	u8 = A_ & 0x01;
	A_ = (A_ >> 1) | (CF << 7);
	CF = u8;

	//Update FLAGS
	HF = 0; NF = 0;
	
	return true;
}

// ---- RLC r - RLC (HL) - RLC (IX+d) - RLC (IY+d) 
bool Z80A::RLCR()
{
	uint8_t d = cbprefix + (ddcbprefix | fdcbprefix) * 3;

	//Get Value
	switch (OPB(d) & 0x07)
	{
	case 0b000: W_ = B_; break;
	case 0b001: W_ = C_; break;
	case 0b010: W_ = D_; break;
	case 0b011: W_ = E_; break;
	case 0b100: W_ = H_; break;
	case 0b101: W_ = L_; break;
	case 0b110: W_ = (ddcbprefix) ? memRd(IX_ + (int8_t)OPB(2)) : (fdcbprefix) ? memRd(IY_ + (int8_t)OPB(2)) : memRd(HL_); break;
	case 0b111: W_ = A_; break;
	};

	//Update Value
	CF = W_ >> 7;
	W_ = (W_ << 1) + CF;

	//Write Value
	switch (OPB(d) & 0x07)
	{
	case 0b000: B_ = W_; break;
	case 0b001: C_ = W_; break;
	case 0b010: D_ = W_; break;
	case 0b011: E_ = W_; break;
	case 0b100: H_ = W_; break;
	case 0b101: L_ = W_; break;
	case 0b110: (ddcbprefix) ? memWr(IX_ + (int8_t)OPB(2), W_) : (fdcbprefix) ? memWr(IY_ + (int8_t)OPB(2), W_) : memWr(HL_, W_); break;
	case 0b111: A_ = W_; break;
	};

	//Update Flags
	SF = (W_ >> 7); ZF = (W_ == 0); HF = 0; PF = isP(W_); NF = 0;
	
	return true;
}

// ---- RL r - RL (HL) - RL (IX+d) - RL (IY+d) 
bool Z80A::RLR()
{
	uint8_t d = cbprefix + (ddcbprefix | fdcbprefix) * 3;

	//Get Value
	switch (OPB(d) & 0x07)
	{
	case 0b000: W_ = B_; break;
	case 0b001: W_ = C_; break;
	case 0b010: W_ = D_; break;
	case 0b011: W_ = E_; break;
	case 0b100: W_ = H_; break;
	case 0b101: W_ = L_; break;
	case 0b110: W_ = (ddcbprefix) ? memRd(IX_ + (int8_t)OPB(2)) : (fdcbprefix) ? memRd(IY_ + (int8_t)OPB(2)) : memRd(HL_); break;
	case 0b111: W_ = A_; break;
	};

	//Update Accumulator
	u8 = CF;
	CF = W_ >> 7;
	W_ = (W_ << 1) + u8;

	//Write Value
	switch (OPB(d) & 0x07)
	{
	case 0b000: B_ = W_; break;
	case 0b001: C_ = W_; break;
	case 0b010: D_ = W_; break;
	case 0b011: E_ = W_; break;
	case 0b100: H_ = W_; break;
	case 0b101: L_ = W_; break;
	case 0b110: (ddcbprefix) ? memWr(IX_ + (int8_t)OPB(2), W_) : (fdcbprefix) ? memWr(IY_ + (int8_t)OPB(2), W_) : memWr(HL_, W_); break;
	case 0b111: A_ = W_; break;
	};

	//Update Flags
	SF = (W_ >> 7); ZF = (W_ == 0); HF = 0; PF = isP(W_); NF = 0;

	return true;
}

// ---- RRC r - RRC (HL) - RRC (IX+d) - RRC (IY+d) 
bool Z80A::RRCR()
{
	uint8_t d = cbprefix + (ddcbprefix | fdcbprefix) * 3;

	//Get Value
	switch (OPB(d) & 0x07)
	{
	case 0b000: W_ = B_; break;
	case 0b001: W_ = C_; break;
	case 0b010: W_ = D_; break;
	case 0b011: W_ = E_; break;
	case 0b100: W_ = H_; break;
	case 0b101: W_ = L_; break;
	case 0b110: W_ = (ddcbprefix) ? memRd(IX_ + (int8_t)OPB(2)) : (fdcbprefix) ? memRd(IY_ + (int8_t)OPB(2)) : memRd(HL_); break;
	case 0b111: W_ = A_; break;
	};

	//Update Accumulator
	CF = W_ & 0x01;
	W_ = (W_ >> 1) | (CF << 7);

	//Write Value
	switch (OPB(d) & 0x07)
	{
	case 0b000: B_ = W_; break;
	case 0b001: C_ = W_; break;
	case 0b010: D_ = W_; break;
	case 0b011: E_ = W_; break;
	case 0b100: H_ = W_; break;
	case 0b101: L_ = W_; break;
	case 0b110: (ddcbprefix) ? memWr(IX_ + (int8_t)OPB(2), W_) : (fdcbprefix) ? memWr(IY_ + (int8_t)OPB(2), W_) : memWr(HL_, W_); break;
	case 0b111: A_ = W_; break;
	};

	//Update Flags
	SF = (W_ >> 7); ZF = (W_ == 0); HF = 0; PF = isP(W_); NF = 0;

	return true;
}

// ---- RR r - RR (HL) - RR (IX+d) - RR (IY+d) 
bool Z80A::RRR()
{
	uint8_t d = cbprefix + (ddcbprefix | fdcbprefix) * 3;

	//Get Value
	switch (OPB(d) & 0x07)
	{
	case 0b000: W_ = B_; break;
	case 0b001: W_ = C_; break;
	case 0b010: W_ = D_; break;
	case 0b011: W_ = E_; break;
	case 0b100: W_ = H_; break;
	case 0b101: W_ = L_; break;
	case 0b110: W_ = (ddcbprefix) ? memRd(IX_ + (int8_t)OPB(2)) : (fdcbprefix) ? memRd(IY_ + (int8_t)OPB(2)) : memRd(HL_); break;
	case 0b111: W_ = A_; break;
	};

	//Update Accumulator
	u8 = W_ & 0x01;
	W_ = (W_ >> 1) | (CF << 7);
	CF = u8;

	//Write Value
	switch (OPB(d) & 0x07)
	{
	case 0b000: B_ = W_; break;
	case 0b001: C_ = W_; break;
	case 0b010: D_ = W_; break;
	case 0b011: E_ = W_; break;
	case 0b100: H_ = W_; break;
	case 0b101: L_ = W_; break;
	case 0b110: (ddcbprefix) ? memWr(IX_ + (int8_t)OPB(2), W_) : (fdcbprefix) ? memWr(IY_ + (int8_t)OPB(2), W_) : memWr(HL_, W_); break;
	case 0b111: A_ = W_; break;
	};

	//Update Flags
	SF = (W_ >> 7); ZF = (W_ == 0); HF = 0; PF = isP(W_); NF = 0;

	return true;
}

// ---- SLA r - SLA (HL) - SLA (IX+d) - SLA (IY+d) 
bool Z80A::SLAR()
{
	uint8_t d = cbprefix + (ddcbprefix | fdcbprefix) * 3;

	//Get Value
	switch (OPB(d) & 0x07)
	{
	case 0b000: W_ = B_; break;
	case 0b001: W_ = C_; break;
	case 0b010: W_ = D_; break;
	case 0b011: W_ = E_; break;
	case 0b100: W_ = H_; break;
	case 0b101: W_ = L_; break;
	case 0b110: W_ = (ddcbprefix) ? memRd(IX_ + (int8_t)OPB(2)) : (fdcbprefix) ? memRd(IY_ + (int8_t)OPB(2)) : memRd(HL_); break;
	case 0b111: W_ = A_; break;
	};

	//Update Accumulator
	CF = W_ >> 7;
	W_ = W_ << 1;

	//Write Value
	switch (OPB(d) & 0x07)
	{
	case 0b000: B_ = W_; break;
	case 0b001: C_ = W_; break;
	case 0b010: D_ = W_; break;
	case 0b011: E_ = W_; break;
	case 0b100: H_ = W_; break;
	case 0b101: L_ = W_; break;
	case 0b110: (ddcbprefix) ? memWr(IX_ + (int8_t)OPB(2), W_) : (fdcbprefix) ? memWr(IY_ + (int8_t)OPB(2), W_) : memWr(HL_, W_); break;
	case 0b111: A_ = W_; break;
	};

	//Update Flags
	SF = (W_ >> 7); ZF = (W_ == 0); HF = 0; PF = isP(W_); NF = 0;

	return true;
}

// ---- SRA r - SRA (HL) - SRA (IX+d) - SRA (IY+d) 
bool Z80A::SRAR()
{
	uint8_t d = cbprefix + (ddcbprefix | fdcbprefix) * 3;

	//Get Value
	switch (OPB(d) & 0x07)
	{
	case 0b000: W_ = B_; break;
	case 0b001: W_ = C_; break;
	case 0b010: W_ = D_; break;
	case 0b011: W_ = E_; break;
	case 0b100: W_ = H_; break;
	case 0b101: W_ = L_; break;
	case 0b110: W_ = (ddcbprefix) ? memRd(IX_ + (int8_t)OPB(2)) : (fdcbprefix) ? memRd(IY_ + (int8_t)OPB(2)) : memRd(HL_); break;
	case 0b111: W_ = A_; break;
	};

	//Update Accumulator
	CF = W_ & 0x01;
	W_ = (W_ >> 1) | (W_ & 0x80);

	//Write Value
	switch (OPB(d) & 0x07)
	{
	case 0b000: B_ = W_; break;
	case 0b001: C_ = W_; break;
	case 0b010: D_ = W_; break;
	case 0b011: E_ = W_; break;
	case 0b100: H_ = W_; break;
	case 0b101: L_ = W_; break;
	case 0b110: (ddcbprefix) ? memWr(IX_ + (int8_t)OPB(2), W_) : (fdcbprefix) ? memWr(IY_ + (int8_t)OPB(2), W_) : memWr(HL_, W_); break;
	case 0b111: A_ = W_; break;
	};

	//Update Flags
	SF = (W_ >> 7); ZF = (W_ == 0); HF = 0; PF = isP(W_); NF = 0;

	return true;
}

// ---- SLL r - SLL (HL) - SLL (IX+d) - SLL (IY+d)
bool Z80A::SLLR()
{
	uint8_t d = cbprefix + (ddcbprefix | fdcbprefix) * 3;

	//Get Value
	switch (OPB(d) & 0x07)
	{
	case 0b000: W_ = B_; break;
	case 0b001: W_ = C_; break;
	case 0b010: W_ = D_; break;
	case 0b011: W_ = E_; break;
	case 0b100: W_ = H_; break;
	case 0b101: W_ = L_; break;
	case 0b110: W_ = (ddcbprefix) ? memRd(IX_ + (int8_t)OPB(2)) : (fdcbprefix) ? memRd(IY_ + (int8_t)OPB(2)) : memRd(HL_); break;
	case 0b111: W_ = A_; break;
	};

	//Update Accumulator
	CF = W_ >> 7;
	W_ = (W_ << 1) | 0x01;

	//Write Value
	switch (OPB(d) & 0x07)
	{
	case 0b000: B_ = W_; break;
	case 0b001: C_ = W_; break;
	case 0b010: D_ = W_; break;
	case 0b011: E_ = W_; break;
	case 0b100: H_ = W_; break;
	case 0b101: L_ = W_; break;
	case 0b110: (ddcbprefix) ? memWr(IX_ + (int8_t)OPB(2), W_) : (fdcbprefix) ? memWr(IY_ + (int8_t)OPB(2), W_) : memWr(HL_, W_); break;
	case 0b111: A_ = W_; break;
	};

	//Update Flags
	SF = (W_ >> 7); ZF = (W_ == 0); HF = 0; PF = isP(W_); NF = 0;

	return true;
}

// ---- SRL r - SRL (HL) - SRL (IX+d) - SRL (IY+d)
bool Z80A::SRLR()
{
	uint8_t d = cbprefix + (ddcbprefix | fdcbprefix) * 3;

	//Get Value
	switch (OPB(d) & 0x07)
	{
	case 0b000: W_ = B_; break;
	case 0b001: W_ = C_; break;
	case 0b010: W_ = D_; break;
	case 0b011: W_ = E_; break;
	case 0b100: W_ = H_; break;
	case 0b101: W_ = L_; break;
	case 0b110: W_ = (ddcbprefix) ? memRd(IX_ + (int8_t)OPB(2)) : (fdcbprefix) ? memRd(IY_ + (int8_t)OPB(2)) : memRd(HL_); break;
	case 0b111: W_ = A_; break;
	};

	//Update Accumulator
	CF = W_ & 0x01;
	W_ = (W_ >> 1);

	//Write Value
	switch (OPB(d) & 0x07)
	{
	case 0b000: B_ = W_; break;
	case 0b001: C_ = W_; break;
	case 0b010: D_ = W_; break;
	case 0b011: E_ = W_; break;
	case 0b100: H_ = W_; break;
	case 0b101: L_ = W_; break;
	case 0b110: (ddcbprefix) ? memWr(IX_ + (int8_t)OPB(2), W_) : (fdcbprefix) ? memWr(IY_ + (int8_t)OPB(2), W_) : memWr(HL_, W_); break;
	case 0b111: A_ = W_; break;
	};

	//Update Flags
	SF = (W_ >> 7); ZF = (W_ == 0); HF = 0; PF = isP(W_); NF = 0;

	return true;
}

// ---- RLD
bool Z80A::RLD()
{
	Z_ = memRd(HL_);
	W_ = A_;
	WZ_ = ((WZ_ & 0x00ff) << 4) | (WZ_ & 0x0fff) >> 8 | (WZ_ & 0xf000);
	A_ = W_;
	memWr(HL_, Z_);
	
	//Update Flags
	SF = (A_ >> 7); ZF = (A_ == 0); HF = 0; PF = isP(A_); NF = 0;
	
	return true;
}

// ---- RRD
bool Z80A::RRD()
{
	Z_ = memRd(HL_);
	W_ = A_;
	WZ_ = ((WZ_ & 0x0ff0) >> 4) | ((WZ_ & 0x000f) << 8) | (WZ_ & 0xf000);
	A_ = W_;
	memWr(HL_, Z_);

	//Update Flags
	SF = (A_ >> 7); ZF = (A_ == 0); HF = 0; PF = isP(A_); NF = 0;

	return true;
}

//Bit Set, Reset and Test Instructions
// ---- BIT b,r - BIT b,(HL) - BIT b,(IX+d) - BIT b,(IY+d)
bool Z80A::BITBR()
{
	uint8_t d = cbprefix + (ddcbprefix | fdcbprefix) * 3;

	//Get Value
	switch (OPB(d) & 0x07)
	{
	case 0b000: u8 = B_; break;
	case 0b001: u8 = C_; break;
	case 0b010: u8 = D_; break;
	case 0b011: u8 = E_; break;
	case 0b100: u8 = H_; break;
	case 0b101: u8 = L_; break;
	case 0b110: u8 = (ddcbprefix) ? memRd(IX_ + (int8_t)OPB(2)) : (fdcbprefix) ? memRd(IY_ + (int8_t)OPB(2)) : memRd(HL_); break;
	case 0b111: u8 = A_; break;
	};
	
	uint8_t bit = (OPB(d) >> 3) & 0x07;

	//Update Flags
	SF = (bit == 7) ? (u8 >> 7) : 0; ZF = ((u8 & (1 << bit)) == 0); HF = 1; PF = ZF; NF = 0;
	
	return true;
}

// ---- SET b,r - SET b,(HL) - SET b,(IX+d) - SET b,(IY+d)
bool Z80A::SETBR()
{
	uint8_t d = cbprefix + (ddcbprefix | fdcbprefix) * 3;

	//Get Value
	switch (OPB(d) & 0x07)
	{
	case 0b000: u8 = B_; break;
	case 0b001: u8 = C_; break;
	case 0b010: u8 = D_; break;
	case 0b011: u8 = E_; break;
	case 0b100: u8 = H_; break;
	case 0b101: u8 = L_; break;
	case 0b110: u8 = (ddcbprefix) ? memRd(IX_ + (int8_t)OPB(2)) : (fdcbprefix) ? memRd(IY_ + (int8_t)OPB(2)) : memRd(HL_); break;
	case 0b111: u8 = A_; break;
	};

	u8 |= (1 << ((OPB(d) >> 3) & 0x07));

	//Write Value
	switch (OPB(d) & 0x07)
	{
	case 0b000: B_ = u8; break;
	case 0b001: C_ = u8; break;
	case 0b010: D_ = u8; break;
	case 0b011: E_ = u8; break;
	case 0b100: H_ = u8; break;
	case 0b101: L_ = u8; break;
	case 0b110: (ddcbprefix) ? memWr(IX_ + (int8_t)OPB(2), u8) : (fdcbprefix) ? memWr(IY_ + (int8_t)OPB(2), u8) : memWr(HL_, u8); break;
	case 0b111: A_ = u8; break;
	};
		
	return true;
}

// ---- RES b,r - RES b,(HL) - RES b,(IX+d) - RES b,(IY+d)
bool Z80A::RESBR()
{
	uint8_t d = cbprefix + (ddcbprefix | fdcbprefix) * 3;

	//Get Value
	switch (OPB(d) & 0x07)
	{
	case 0b000: u8 = B_; break;
	case 0b001: u8 = C_; break;
	case 0b010: u8 = D_; break;
	case 0b011: u8 = E_; break;
	case 0b100: u8 = H_; break;
	case 0b101: u8 = L_; break;
	case 0b110: u8 = (ddcbprefix) ? memRd(IX_ + (int8_t)OPB(2)) : (fdcbprefix) ? memRd(IY_ + (int8_t)OPB(2)) : memRd(HL_); break;
	case 0b111: u8 = A_; break;
	};

	u8 &= ~(0x01 << ((OPB(d) >> 3) & 0x07));

	//Write Value
	switch (OPB(d) & 0x07)
	{
	case 0b000: B_ = u8; break;
	case 0b001: C_ = u8; break;
	case 0b010: D_ = u8; break;
	case 0b011: E_ = u8; break;
	case 0b100: H_ = u8; break;
	case 0b101: L_ = u8; break;
	case 0b110: (ddcbprefix) ? memWr(IX_ + (int8_t)OPB(2), u8) : (fdcbprefix) ? memWr(IY_ + (int8_t)OPB(2), u8) : memWr(HL_, u8); break;
	case 0b111: A_ = u8; break;
	};

	return true;
}

//Jump Instructions
// ---- JP nn
bool Z80A::JPNN()
{
	PC_ = OPW(1);
	
	return true;
}

// ---- JP cc,nn
bool Z80A::JPCCNN()
{
	switch ((OPB(0) >> 3) & 0x07)
	{
	case 0b000: !ZF ? PC_ = OPW(1) : 0; break;
	case 0b001: ZF ? PC_ = OPW(1) : 0; break;
	case 0b010: !CF ? PC_ = OPW(1) : 0; break;
	case 0b011: CF ? PC_ = OPW(1) : 0; break;
	case 0b100: !PF ? PC_ = OPW(1) : 0; break;
	case 0b101: PF ? PC_ = OPW(1) : 0; break;
	case 0b110: !SF ? PC_ = OPW(1) : 0; break;
	case 0b111: SF ? PC_ = OPW(1) : 0; break;
	}

	return true;
}

// ---- JR e
bool Z80A::JRE()
{
	PC_ += (int8_t)OPB(1);

	return true;
}

// ---- JR C,e
bool Z80A::JRCE()
{
	if (CF)
	{
		PC_ += (int8_t)OPB(1);
		return true;
	}

	return false;
}

// ---- JR NC,e
bool Z80A::JRNCE()
{
	if (!CF)
	{
		PC_ += (int8_t)OPB(1);
		return true;
	}

	return false;
}

// ---- JR Z,e
bool Z80A::JRZE()
{
	if (ZF)
	{
		PC_ += (int8_t)OPB(1);
		return true;
	}

	return false;
}

// ---- JR NZ,e
bool Z80A::JRNZE()
{
	if (!ZF)
	{
		PC_ += (int8_t)OPB(1);
		return true;
	}

	return false;
}

// ---- JP (HL)
bool Z80A::JPiHL()
{
	PC_ = HL_;

	return true;
}

// ---- JP (IX)
bool Z80A::JPiIX()
{
	PC_ = IX_;

	return true;
}

// ---- JP (IY)
bool Z80A::JPiIY()
{
	PC_ = IY_;

	return true;
}

// ---- DJNZ, e
bool Z80A::DJNZE()
{
	B_--;

	if (B_ != 0)
	{
		PC_ += (int8_t)OPB(1);
		return true;
	}

	return false;
}

//Call and Return Instructions
// ---- CALL nn
bool Z80A::CALLNN()
{
	//Write to Stack Old PC
	SP_ -= 2;
	memWr16(SP_, PC_);

	//Update PC
	PC_ = OPW(1);

	return true;
}

// ---- CALL cc,nn
bool Z80A::CALLCCNN()
{
	switch ((OPB(0) >> 3) & 0x07)
	{
	case 0b000: if (!ZF) { stkWr16(SP_, PC_); PC_ = OPW(1); } else return false; break;
	case 0b001: if (ZF) { stkWr16(SP_, PC_); PC_ = OPW(1); }else return false; break;
	case 0b010: if (!CF) { stkWr16(SP_, PC_); PC_ = OPW(1); } else return false; break;
	case 0b011: if (CF) { stkWr16(SP_, PC_); PC_ = OPW(1); } else return false; break;
	case 0b100: if (!PF) { stkWr16(SP_, PC_); PC_ = OPW(1); } else return false; break;
	case 0b101: if (PF) { stkWr16(SP_, PC_); PC_ = OPW(1); } else return false; break;
	case 0b110: if (!SF) { stkWr16(SP_, PC_); PC_ = OPW(1); } else return false; break;
	case 0b111: if (SF) { stkWr16(SP_, PC_); PC_ = OPW(1); } else return false; break;
	}
	
	return true;
}

// ---- RET
bool Z80A::RET()
{
	//Read from Stack old PC
	PC_ = stkRd16(SP_);
	
	return true;
}

// ---- RET cc
bool Z80A::RETCC()
{
	switch ((OPB(0) >> 3) & 0x07)
	{
	case 0b000: !ZF ? PC_ = stkRd16(SP_) : 0; break;
	case 0b001: ZF ? PC_ = stkRd16(SP_) : 0; break;
	case 0b010: !CF ? PC_ = stkRd16(SP_) : 0; break;
	case 0b011: CF ? PC_ = stkRd16(SP_) : 0; break;
	case 0b100: !PF ? PC_ = stkRd16(SP_) : 0; break;
	case 0b101: PF ? PC_ = stkRd16(SP_) : 0; break;
	case 0b110: !SF ? PC_ = stkRd16(SP_) : 0; break;
	case 0b111: SF ? PC_ = stkRd16(SP_) : 0; break;
	}
	
	return true;
}

// ---- RETI
bool Z80A::RETI()
{
	//Read from Stack old PC
	PC_ = stkRd16(SP_);

	return true;
}

bool Z80A::RETN()
{
	//Restore bIFF1 value
	bIFF1 = bIFF2;
	
	//Read from Stack old PC
	PC_ = stkRd16(SP_);

	return true;
}

// ---- RST p
bool Z80A::RSTP()
{
	//Write to Stack old PC
	stkWr16(SP_, PC_);
	
	//Update PC
	switch ((OPB(0) >> 3) & 0x07)
	{
	case 0b000: PC_ = 0x0000; break;
	case 0b001: PC_ = 0x0008; break;
	case 0b010: PC_ = 0x0010; break;
	case 0b011: PC_ = 0x0018; break;
	case 0b100: PC_ = 0x0020; break;
	case 0b101: PC_ = 0x0028; break;
	case 0b110: PC_ = 0x0030; break;
	case 0b111: PC_ = 0x0038; break;
	}

	return true;
}

//Input and Output Instructions
// ---- IN A,(n)
bool Z80A::INAiN()
{
	A_ = ioRd(OPB(1));

	return true;
}

// ---- IN r,(C)
bool Z80A::INRiC()
{
	u8 = ioRd(C_);

	//Write to Register
	switch ((OPB(1) >> 3) & 0x07)
	{
	case 0b000: B_ = u8; break;
	case 0b001: C_ = u8; break;
	case 0b010: D_ = u8; break;
	case 0b011: E_ = u8; break;
	case 0b100: H_ = u8; break;
	case 0b101: L_ = u8; break;
	case 0b110: break; //Undefined OPCODE
	case 0b111: A_ = u8; break;
	};

	//Update FLAGS
	SF = (u8 >> 7); ZF = (u8 == 0); HF = 0; PF = isP(u8); NF = 0;
		
	return true;
}

// ---- INI
bool Z80A::INI()
{
	memWr(HL_, ioRd(C_));
	B_--;
	HL_++;

	//Update FLAGS
	ZF = (B_ == 0); NF = 1;
	
	return true;
}

// ---- INIR
bool Z80A::INIR()
{
	INI();
	
	if (B_ != 0)
	{
		//Repeat Opcode
		PC_ -= 2;
		return true;
	}
		
	return false;
}

// ---- IND
bool Z80A::IND()
{
	memWr(HL_, ioRd(C_));
	B_--;
	HL_--;

	//Update FLAGS
	ZF = (B_ == 0); NF = 1;
	
	return true;
}

// ---- INDR
bool Z80A::INDR()
{
	IND();
	
	if (B_ != 0)
	{
		//Repeat Opcode
		PC_ -= 2;
		return true;
	}

	return false;
}

// ---- OUT (n),A
bool Z80A::OUTiNA()
{
	ioWr(OPB(1), A_);

	return true;
}

// ---- OUT (C),r
bool Z80A::OUTiCR()
{
	switch ((OPB(1) >> 3) & 0x07)
	{
	case 0b000: ioWr(C_ , B_); break;
	case 0b001: ioWr(C_ , C_); break;
	case 0b010: ioWr(C_ , D_); break;
	case 0b011: ioWr(C_ , E_); break;
	case 0b100: ioWr(C_ , H_); break;
	case 0b101: ioWr(C_ , L_); break;
	case 0b110: break; //Undefined OPCODE
	case 0b111: ioWr(C_ , A_); break;
	};
	
	return true;
}

// ---- OUTI
bool Z80A::OUTI()
{
	ioWr(C_, memRd(HL_));
	B_--;
	HL_++;

	//Update FLAGS
	ZF = (B_ == 0); NF = 1;
	
	return true;
}

// ---- OTIR
bool Z80A::OTIR()
{
	OUTI();
	
	if (B_ != 0)
	{
		//Repeat Opcode
		PC_ -= 2;
		return true;
	}

	return false;
}

// ---- OUTD
bool Z80A::OUTD()
{
	ioWr(C_, memRd(HL_));
	B_--;
	HL_--;

	//Update FLAGS
	ZF = (B_ == 0); NF = 1;
	
	return true;
}

// ---- OTDR
bool Z80A::OTDR()
{
	OUTD();

	if (B_ != 0)
	{
		//Repeat Opcode
		PC_ -= 2;
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                         Implementation of all Undocumented Istruction Set for Z80A Processor
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Input and Output Instructions
// ---- IN (C)
bool Z80A::INiC()
{
	u8 = ioRd(C_);
	
	//Update FLAGS
	SF = (u8 >> 7); ZF = (u8 == 0); HF = 0; PF = isP(u8); NF = 0;
	
	return true;
}

// ---- OUT (C),0
bool Z80A::OUTiC0()
{
	ioWr(C_, 0x00);

	return true;
}

//Undocumented Instructions
bool Z80A::UNDOC()
{
	LOG_F(ERROR, "Undocumented OPCODE [%.2X %.2X %.2X %.2X]", opcode[0], opcode[1], opcode[2], opcode[3]);
	return true;
}

bool Z80A::INVAL()
{
	LOG_F(ERROR, "Invalid OPCODE [%.2X %.2X %.2X %.2X]\n", opcode[0], opcode[1], opcode[2], opcode[3]);
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                         Implementation of Interrupt Call Functions
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Maskable Interupts
bool Z80A::IRQEXEC()
{
	irq_latch = false;


	if (bIFF1)
	{
		//Reset IFF Flip-Flop
		bIFF1 = bIFF2 = false;
		
		switch (intmode)
		{
		case 0:
			//Unsupported Mode by SMS
			break;
		case 1:
			//Check if HALT was active
			if (bHalt)
			{
				bHalt = false;
				PC_++;
			}
		
			//Push Program Counter to Stack
			stkWr16(SP_, PC_);

			//Set Program Counter to 0x0038 (Equivalent to RST 38h)
			PC_ = 0x0038;

			//Set IM1 clock cycles
			cycles = 13;
			break;
		case 2:
			//Unsupported Mode by SMS
			break;
		}
	}

	return true;
}

//Non Maskable Interrupt
bool Z80A::NMIEXEC()
{
	nmi_latch = false;
	
	//Disable Maskable Interrupts 
	bIFF1 = false;

	//Check if HALT was active
	if (bHalt)
	{
		bHalt = false;
		PC_++;
	}

	//Push Program Counter to Stack
	stkWr16(SP_, PC_);

	//Set PC to NMI subroutine at 0x0066
	PC_ = 0x0066;

	//Set NMI clock cycles
	cycles = 11;

	return true;
}

// This is the disassembly function. Its workings are not required for emulation.
// It is merely a convenience function to turn the binary instruction code into
// human readable form. Its included as part of the emulator because it can take
// advantage of many of the CPUs internal operations to do this.
std::map<uint16_t, std::string> Z80A::disassemble(uint16_t nStart, uint16_t nStop)
{
	uint32_t addr = nStart;
	uint8_t value = 0x00, lo = 0x00, hi = 0x00;
	std::map<uint16_t, std::string> mapLines;
	uint16_t line_addr = 0;
	uint8_t instrLen;

	// A convenient utility to convert variables into
	// hex strings because "modern C++"'s method with 
	// streams is atrocious
	auto hex = [](uint32_t n, uint8_t d)
	{
		std::string s(d, '0');
		for (int i = d - 1; i >= 0; i--, n >>= 4)
			s[i] = "0123456789ABCDEF"[n & 0xF];
		return s;
	};

	// Starting at the specified address we read an instruction
	// byte, which in turn yields information from the lookup table
	// as to how many additional bytes we need to read and what the
	// addressing mode is. I need this info to assemble human readable
	// syntax, which is different depending upon the addressing mode

	// As the instruction is decoded, a std::string is assembled
	// with the readable output
	while (addr <= (uint32_t)nStop)
	{
		line_addr = addr;

		// Prefix line with instruction address
		std::string sInst = "0x" + hex(addr, 4) + ": ";

		//Read First 4 Byte
		opcode[0] = memRd(line_addr);
		opcode[1] = memRd(line_addr + 1);
		opcode[2] = memRd(line_addr + 2);
		opcode[3] = memRd(line_addr + 3);

		//Select Instruction Group
		// 0 - MAIN Instruction Set
		// 1 - BIT Instruction Set - CB Prefix
		// 2 - IX Instruction Set - DD Prefix
		// 3 - EXT Instruction Set - ED Prefix
		// 4 - IY Instruction Set - FD Prefix
		// 5 - IX BIT Instruction Set - DD CB Prefix
		// 5 - IY BIT Instruction Set - FD CB Prefix
		switch (opcode[0])
		{
		case 0xcb: indexOp = opcode[1] + 0x100;  break;
		case 0xdd: indexOp = (opcode[1] != 0xcb) ? (opcode[1] + 0x200) : (opcode[3] + 0x500); break;
		case 0xed: indexOp = opcode[1] + 0x300; break;
		case 0xfd: indexOp = (opcode[1] != 0xcb) ? (opcode[1] + 0x400) : (opcode[3] + 0x600); break;

		default:
			indexOp = opcode[0];
		}

		instrLen = instSet[indexOp].lenght;
		sInst += instSet[indexOp].operation;
		if ((instrLen == 2) && (opcode[0] != 0xcb) && (opcode[0] != 0xdd) && (opcode[0] != 0xed) && (opcode[0] != 0xfd))
		{
			lo = opcode[1];
			sInst.replace(sInst.find("l"), 2, hex(lo, 2));
		}
		else if ((instrLen == 3) && (opcode[0] != 0xdd) && (opcode[0] != 0xfd))
		{
			lo = opcode[1];
			hi = opcode[2];
			sInst.replace(sInst.find("l"), 2, hex(lo, 2));
			sInst.replace(sInst.find("h"), 2, hex(hi, 2));
		}
		else if ((instrLen == 3) && ((opcode[0] == 0xdd) || (opcode[0] = 0xfd)))
		{
			lo = opcode[2];
			sInst.replace(sInst.find("l"), 2, hex(lo, 2));
		}
		else if ((instrLen == 4) && (opcode[1] != 0xcb))
		{
			lo = opcode[2];
			hi = opcode[3];
			sInst.replace(sInst.find("l"), 2, hex(lo, 2));
			sInst.replace(sInst.find("h"), 2, hex(hi, 2));
		}
		else if ((instrLen == 4) && (opcode[1] == 0xcb))
		{
			lo = opcode[2];
			sInst.replace(sInst.find("l"), 2, hex(lo, 2));
		}

		mapLines[line_addr] = sInst;

		//Skip to next Opcode
		addr += instrLen;
	}

	return mapLines;
}


