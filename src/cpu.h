/*
    epple2
    Copyright (C) 2008 by Chris Mosher <chris@mosher.mine.nu>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY, without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef CPU_H
#define CPU_H

class AddressBus;

class CPU
{
private:
	enum { MEMORY_LIM = 1 << 0x10 };
	enum { IRQ_VECTOR = MEMORY_LIM-2 }; // or BRK
	enum { RESET_VECTOR = IRQ_VECTOR-2 }; // or power-on
	enum { NMI_VECTOR = RESET_VECTOR-2 };

	unsigned char adl;
	unsigned char adh;
	unsigned char bal;
	unsigned char bah;
	unsigned char ial;
	unsigned char iah;
	unsigned char idx;
	signed char offset;
	bool branch;
	signed char sc;
	bool wc;

	bool pendingIRQ;
	bool pendingNMI;
	bool pendingReset;
	
	bool started;

	unsigned char a;
	unsigned char x;
	unsigned char y;

	unsigned char s;

	//p = NVMBDIZC
	enum { PMASK_C = 1<<0 };
	enum { PMASK_Z = 1<<1 };
	enum { PMASK_I = 1<<2 };
	enum { PMASK_D = 1<<3 };
	enum { PMASK_B = 1<<4 };
	enum { PMASK_M = 1<<5 };
	enum { PMASK_V = 1<<6 };
	enum { PMASK_N = 1<<7 };
	unsigned char p;

	unsigned short pc;

	AddressBus& addressBus;

	unsigned short address;
	unsigned char data;

	unsigned short opcode;

	signed char t;


	static void (CPU::*addr[])();
	static void (CPU::*exec[])();

	void firstCycle();
	int getInterruptAddress();
	int getInterruptPseudoOpCode();
	void subsequentCycle();

	void read();
	void write();
	void execute();
	void done();

	unsigned char pch();
	unsigned char pcl();
	unsigned short sp();
	unsigned short push();
	unsigned short pull();
	unsigned char getIndex();
	unsigned short ad();
	unsigned short ia();
	unsigned short ba();
	unsigned short combine(const unsigned char lo, const unsigned char hi);
	void setP(const unsigned char mask, const unsigned char val);
	void setStatusRegisterNZ(const unsigned char val);
	unsigned char shiftLeft(unsigned char byt);
	unsigned char shiftRight(unsigned char byt);
	unsigned char rotateLeft(unsigned char byt);
	unsigned char rotateRight(unsigned char byt);
	void compare(const unsigned char r);

	void addr_SINGLE();
	void addr_INTERNAL_IMMEDIATE();
	void addr_INTERNAL_ZERO_PAGE();
	void addr_INTERNAL_ABSOLUTE();
	void addr_INTERNAL_INDIRECT_X();
	void addr_INTERNAL_ABSOLUTE_XY();
	void addr_INTERNAL_ZERO_PAGE_XY();
	void addr_INTERNAL_INDIRECT_Y();
	void addr_STORE_ZERO_PAGE();
	void addr_STORE_ABSOLUTE();
	void addr_STORE_INDIRECT_X();
	void addr_STORE_ABSOLUTE_XY();
	void addr_STORE_ZERO_PAGE_XY();
	void addr_STORE_INDIRECT_Y();
	void addr_RMW_ZERO_PAGE();
	void addr_RMW_ABSOLUTE();
	void addr_RMW_ZERO_PAGE_X();
	void addr_RMW_ABSOLUTE_X();
	void addr_MISC_PUSH();
	void addr_MISC_PULL();
	void addr_MISC_JSR();
	void addr_MISC_BREAK();
	void addr_MISC_RTI();
	void addr_JMP_ABSOLUTE();
	void addr_JMP_INDIRECT();
	void addr_RTS();
	void addr_BRANCH();
	void addr_NMI();
	void addr_RESET();
	void addr_IRQ();

	void LDA();
	void LDX();
	void LDY();
	void STA();
	void STX();
	void STY();
	void CMP();
	void CPX();
	void CPY();
	void AND();
	void ORA();
	void EOR();
	void ASL();
	void ASL_A();
	void LSR();
	void LSR_A();
	void ROL();
	void ROL_A();
	void ROR();
	void ROR_A();
	void ADC();
	void SBC();
	void INC();
	void DEC();
	void INX();
	void INY();
	void DEX();
	void DEY();
	void BIT();
	void PHA();
	void PHP();
	void PLA();
	void PLP();
	void BRK();
	void RTI();
	void JMP();
	void RTS();
	void JSR();
	void BNE();
	void BEQ();
	void BVC();
	void BVS();
	void BCC();
	void BCS();
	void BPL();
	void BMI();
	void TAX();
	void TXA();
	void TAY();
	void TYA();
	void TXS();
	void TSX();
	void CLC();
	void SEC();
	void CLI();
	void SEI();
	void CLV();
	void CLD();
	void SED();
	void NOP();
	void Unoff();
	void Unoff1();
	void Unoff2();
	void Unoff3();
	void Hang();

public:
	CPU(AddressBus& addressBus);
	~CPU();

	void powerOn();
	void reset();
	void IRQ();
	void NMI();
	void tick();
};

#endif
