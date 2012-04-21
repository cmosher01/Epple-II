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


    Portions Copyright (C) 2000, by Verhille Arnaud, GPLv2 license. (See ADC, SBC below.)
*/
#include "cpu.h"
#include "addressbus.h"



CPU::CPU(AddressBus& addressBus):
	addressBus(addressBus)
{
}

CPU::~CPU()
{
}

void CPU::powerOn()
{
	this->started = false;
	this->pendingReset = false;
	this->pendingIRQ = false;
	this->pendingNMI = false;
	this->p = PMASK_M;
}

void CPU::reset()
{
	this->started = true;
	this->pendingReset = true;
	this->t = 0;
}

void CPU::IRQ()
{
	this->pendingIRQ = true;
}

void CPU::NMI()
{
	this->pendingNMI = true;
}

void CPU::tick()
{
	if (!this->started)
	{
		return;
	}
	if (!this->t)
	{
		firstCycle();
	}
	else
	{
		subsequentCycle();
	}
	++this->t;
}

void CPU::firstCycle()
{
	const bool interrupt = this->pendingNMI || this->pendingReset || (!(this->p & PMASK_I) && this->pendingIRQ);

	if (interrupt)
	{
		this->pc = getInterruptAddress();
	}

	this->address = this->pc++;

	read();

	if (interrupt)
	{
		this->opcode = getInterruptPseudoOpCode();
	}
	else
	{
		this->opcode = this->data;
	}
}

int CPU::getInterruptAddress()
{
	if (this->pendingNMI)
	{
		return NMI_VECTOR-2;
	}
	if (this->pendingReset)
	{
		return RESET_VECTOR-2;
	}
	if (!(this->p & PMASK_I) && this->pendingIRQ)
	{
		return IRQ_VECTOR-2;
	}
	return 0; // can't happen
}

int CPU::getInterruptPseudoOpCode()
{
	if (this->pendingNMI)
	{
		return 0x100;
	}
	if (this->pendingReset)
	{
		return 0x101;
	}
	if (!(this->p & PMASK_I) && this->pendingIRQ)
	{
		return 0x102;
	}
	return 0; // can't happen
}

void (CPU::*(CPU::addr[]))() =
{
&CPU::addr_MISC_BREAK,
&CPU::addr_INTERNAL_INDIRECT_X,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ZERO_PAGE,
&CPU::addr_RMW_ZERO_PAGE,
&CPU::addr_SINGLE,
&CPU::addr_MISC_PUSH,
&CPU::addr_INTERNAL_IMMEDIATE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ABSOLUTE,
&CPU::addr_RMW_ABSOLUTE,
&CPU::addr_SINGLE,
&CPU::addr_BRANCH,
&CPU::addr_INTERNAL_INDIRECT_Y,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ZERO_PAGE_XY,
&CPU::addr_RMW_ZERO_PAGE_X,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ABSOLUTE_XY,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ABSOLUTE_XY,
&CPU::addr_RMW_ABSOLUTE_X,
&CPU::addr_SINGLE,
&CPU::addr_MISC_JSR,
&CPU::addr_INTERNAL_INDIRECT_X,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ZERO_PAGE,
&CPU::addr_INTERNAL_ZERO_PAGE,
&CPU::addr_RMW_ZERO_PAGE,
&CPU::addr_SINGLE,
&CPU::addr_MISC_PULL,
&CPU::addr_INTERNAL_IMMEDIATE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ABSOLUTE,
&CPU::addr_INTERNAL_ABSOLUTE,
&CPU::addr_RMW_ABSOLUTE,
&CPU::addr_SINGLE,
&CPU::addr_BRANCH,
&CPU::addr_INTERNAL_INDIRECT_Y,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ZERO_PAGE_XY,
&CPU::addr_RMW_ZERO_PAGE_X,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ABSOLUTE_XY,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ABSOLUTE_XY,
&CPU::addr_RMW_ABSOLUTE_X,
&CPU::addr_SINGLE,
&CPU::addr_MISC_RTI,
&CPU::addr_INTERNAL_INDIRECT_X,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ZERO_PAGE,
&CPU::addr_RMW_ZERO_PAGE,
&CPU::addr_SINGLE,
&CPU::addr_MISC_PUSH,
&CPU::addr_INTERNAL_IMMEDIATE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_JMP_ABSOLUTE,
&CPU::addr_INTERNAL_ABSOLUTE,
&CPU::addr_RMW_ABSOLUTE,
&CPU::addr_SINGLE,
&CPU::addr_BRANCH,
&CPU::addr_INTERNAL_INDIRECT_Y,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ZERO_PAGE_XY,
&CPU::addr_RMW_ZERO_PAGE_X,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ABSOLUTE_XY,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ABSOLUTE_XY,
&CPU::addr_RMW_ABSOLUTE_X,
&CPU::addr_SINGLE,
&CPU::addr_RTS,
&CPU::addr_INTERNAL_INDIRECT_X,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ZERO_PAGE,
&CPU::addr_RMW_ZERO_PAGE,
&CPU::addr_SINGLE,
&CPU::addr_MISC_PULL,
&CPU::addr_INTERNAL_IMMEDIATE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_JMP_INDIRECT,
&CPU::addr_INTERNAL_ABSOLUTE,
&CPU::addr_RMW_ABSOLUTE,
&CPU::addr_SINGLE,
&CPU::addr_BRANCH,
&CPU::addr_INTERNAL_INDIRECT_Y,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ZERO_PAGE_XY,
&CPU::addr_RMW_ZERO_PAGE_X,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ABSOLUTE_XY,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ABSOLUTE_XY,
&CPU::addr_RMW_ABSOLUTE_X,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_STORE_INDIRECT_X,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_STORE_ZERO_PAGE,
&CPU::addr_STORE_ZERO_PAGE,
&CPU::addr_STORE_ZERO_PAGE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_STORE_ABSOLUTE,
&CPU::addr_STORE_ABSOLUTE,
&CPU::addr_STORE_ABSOLUTE,
&CPU::addr_SINGLE,
&CPU::addr_BRANCH,
&CPU::addr_STORE_INDIRECT_Y,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_STORE_ZERO_PAGE_XY,
&CPU::addr_STORE_ZERO_PAGE_XY,
&CPU::addr_STORE_ZERO_PAGE_XY,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_STORE_ABSOLUTE_XY,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_STORE_ABSOLUTE_XY,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_IMMEDIATE,
&CPU::addr_INTERNAL_INDIRECT_X,
&CPU::addr_INTERNAL_IMMEDIATE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ZERO_PAGE,
&CPU::addr_INTERNAL_ZERO_PAGE,
&CPU::addr_INTERNAL_ZERO_PAGE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_IMMEDIATE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ABSOLUTE,
&CPU::addr_INTERNAL_ABSOLUTE,
&CPU::addr_INTERNAL_ABSOLUTE,
&CPU::addr_SINGLE,
&CPU::addr_BRANCH,
&CPU::addr_INTERNAL_INDIRECT_Y,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ZERO_PAGE_XY,
&CPU::addr_INTERNAL_ZERO_PAGE_XY,
&CPU::addr_INTERNAL_ZERO_PAGE_XY,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ABSOLUTE_XY,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ABSOLUTE_XY,
&CPU::addr_INTERNAL_ABSOLUTE_XY,
&CPU::addr_INTERNAL_ABSOLUTE_XY,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_IMMEDIATE,
&CPU::addr_INTERNAL_INDIRECT_X,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ZERO_PAGE,
&CPU::addr_INTERNAL_ZERO_PAGE,
&CPU::addr_RMW_ZERO_PAGE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_IMMEDIATE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ABSOLUTE,
&CPU::addr_INTERNAL_ABSOLUTE,
&CPU::addr_RMW_ABSOLUTE,
&CPU::addr_SINGLE,
&CPU::addr_BRANCH,
&CPU::addr_INTERNAL_INDIRECT_Y,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ZERO_PAGE_XY,
&CPU::addr_RMW_ZERO_PAGE_X,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ABSOLUTE_XY,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ABSOLUTE_XY,
&CPU::addr_RMW_ABSOLUTE_X,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_IMMEDIATE,
&CPU::addr_INTERNAL_INDIRECT_X,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ZERO_PAGE,
&CPU::addr_INTERNAL_ZERO_PAGE,
&CPU::addr_RMW_ZERO_PAGE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_IMMEDIATE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ABSOLUTE,
&CPU::addr_INTERNAL_ABSOLUTE,
&CPU::addr_RMW_ABSOLUTE,
&CPU::addr_SINGLE,
&CPU::addr_BRANCH,
&CPU::addr_INTERNAL_INDIRECT_Y,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ZERO_PAGE_XY,
&CPU::addr_RMW_ZERO_PAGE_X,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ABSOLUTE_XY,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_SINGLE,
&CPU::addr_INTERNAL_ABSOLUTE_XY,
&CPU::addr_RMW_ABSOLUTE_X,
&CPU::addr_SINGLE,
&CPU::addr_NMI,
&CPU::addr_RESET,
&CPU::addr_IRQ,
};

void (CPU::*(CPU::exec[]))() =
{
&CPU::BRK,
&CPU::ORA,
&CPU::Hang,
&CPU::Unoff,
&CPU::Unoff2,
&CPU::ORA,
&CPU::ASL,
&CPU::Unoff,
&CPU::PHP,
&CPU::ORA,
&CPU::ASL_A,
&CPU::Unoff,
&CPU::Unoff3,
&CPU::ORA,
&CPU::ASL,
&CPU::Unoff,
&CPU::BPL,
&CPU::ORA,
&CPU::Hang,
&CPU::Unoff,
&CPU::Unoff2,
&CPU::ORA,
&CPU::ASL,
&CPU::Unoff,
&CPU::CLC,
&CPU::ORA,
&CPU::Unoff1,
&CPU::Unoff,
&CPU::Unoff3,
&CPU::ORA,
&CPU::ASL,
&CPU::Unoff,
&CPU::JSR,
&CPU::AND,
&CPU::Hang,
&CPU::Unoff,
&CPU::BIT,
&CPU::AND,
&CPU::ROL,
&CPU::Unoff,
&CPU::PLP,
&CPU::AND,
&CPU::ROL_A,
&CPU::Unoff,
&CPU::BIT,
&CPU::AND,
&CPU::ROL,
&CPU::Unoff,
&CPU::BMI,
&CPU::AND,
&CPU::Hang,
&CPU::Unoff,
&CPU::Unoff2,
&CPU::AND,
&CPU::ROL,
&CPU::Unoff,
&CPU::SEC,
&CPU::AND,
&CPU::Unoff1,
&CPU::Unoff,
&CPU::Unoff3,
&CPU::AND,
&CPU::ROL,
&CPU::Unoff,
&CPU::RTI,
&CPU::EOR,
&CPU::Hang,
&CPU::Unoff,
&CPU::Unoff2,
&CPU::EOR,
&CPU::LSR,
&CPU::Unoff,
&CPU::PHA,
&CPU::EOR,
&CPU::LSR_A,
&CPU::Unoff,
&CPU::JMP,
&CPU::EOR,
&CPU::LSR,
&CPU::Unoff,
&CPU::BVC,
&CPU::EOR,
&CPU::Hang,
&CPU::Unoff,
&CPU::Unoff2,
&CPU::EOR,
&CPU::LSR,
&CPU::Unoff,
&CPU::CLI,
&CPU::EOR,
&CPU::Unoff1,
&CPU::Unoff,
&CPU::Unoff3,
&CPU::EOR,
&CPU::LSR,
&CPU::Unoff,
&CPU::RTS,
&CPU::ADC,
&CPU::Hang,
&CPU::Unoff,
&CPU::Unoff2,
&CPU::ADC,
&CPU::ROR,
&CPU::Unoff,
&CPU::PLA,
&CPU::ADC,
&CPU::ROR_A,
&CPU::Unoff,
&CPU::JMP,
&CPU::ADC,
&CPU::ROR,
&CPU::Unoff,
&CPU::BVS,
&CPU::ADC,
&CPU::Hang,
&CPU::Unoff,
&CPU::Unoff2,
&CPU::ADC,
&CPU::ROR,
&CPU::Unoff,
&CPU::SEI,
&CPU::ADC,
&CPU::Unoff1,
&CPU::Unoff,
&CPU::Unoff3,
&CPU::ADC,
&CPU::ROR,
&CPU::Unoff,
&CPU::Unoff2,
&CPU::STA,
&CPU::Unoff2,
&CPU::Unoff,
&CPU::STY,
&CPU::STA,
&CPU::STX,
&CPU::Unoff,
&CPU::DEY,
&CPU::Unoff2,
&CPU::TXA,
&CPU::Unoff,
&CPU::STY,
&CPU::STA,
&CPU::STX,
&CPU::Unoff,
&CPU::BCC,
&CPU::STA,
&CPU::Hang,
&CPU::Unoff,
&CPU::STY,
&CPU::STA,
&CPU::STX,
&CPU::Unoff,
&CPU::TYA,
&CPU::STA,
&CPU::TXS,
&CPU::Unoff,
&CPU::Unoff,
&CPU::STA,
&CPU::Unoff,
&CPU::Unoff,
&CPU::LDY,
&CPU::LDA,
&CPU::LDX,
&CPU::Unoff,
&CPU::LDY,
&CPU::LDA,
&CPU::LDX,
&CPU::Unoff,
&CPU::TAY,
&CPU::LDA,
&CPU::TAX,
&CPU::Unoff,
&CPU::LDY,
&CPU::LDA,
&CPU::LDX,
&CPU::Unoff,
&CPU::BCS,
&CPU::LDA,
&CPU::Hang,
&CPU::Unoff,
&CPU::LDY,
&CPU::LDA,
&CPU::LDX,
&CPU::Unoff,
&CPU::CLV,
&CPU::LDA,
&CPU::TSX,
&CPU::Unoff,
&CPU::LDY,
&CPU::LDA,
&CPU::LDX,
&CPU::Unoff,
&CPU::CPY,
&CPU::CMP,
&CPU::Unoff2,
&CPU::Unoff,
&CPU::CPY,
&CPU::CMP,
&CPU::DEC,
&CPU::Unoff,
&CPU::INY,
&CPU::CMP,
&CPU::DEX,
&CPU::Unoff,
&CPU::CPY,
&CPU::CMP,
&CPU::DEC,
&CPU::Unoff,
&CPU::BNE,
&CPU::CMP,
&CPU::Hang,
&CPU::Unoff,
&CPU::Unoff2,
&CPU::CMP,
&CPU::DEC,
&CPU::Unoff,
&CPU::CLD,
&CPU::CMP,
&CPU::Unoff1,
&CPU::Unoff,
&CPU::Unoff3,
&CPU::CMP,
&CPU::DEC,
&CPU::Unoff,
&CPU::CPX,
&CPU::SBC,
&CPU::Unoff2,
&CPU::Unoff,
&CPU::CPX,
&CPU::SBC,
&CPU::INC,
&CPU::Unoff,
&CPU::INX,
&CPU::SBC,
&CPU::NOP,
&CPU::Unoff,
&CPU::CPX,
&CPU::SBC,
&CPU::INC,
&CPU::Unoff,
&CPU::BEQ,
&CPU::SBC,
&CPU::Hang,
&CPU::Unoff,
&CPU::Unoff2,
&CPU::SBC,
&CPU::INC,
&CPU::Unoff,
&CPU::SED,
&CPU::SBC,
&CPU::Unoff1,
&CPU::Unoff,
&CPU::Unoff3,
&CPU::SBC,
&CPU::INC,
&CPU::Unoff,
};

void CPU::subsequentCycle()
{
	(this->*this->addr[this->opcode])();
}


void CPU::addr_SINGLE()
{
	switch (this->t)
	{
		case 1:
			address = pc;
			read(); // discard
			execute();
			done();
		break;
	}
}

void CPU::addr_INTERNAL_IMMEDIATE()
{
	switch (this->t)
	{
		case 1:
			address = pc++;
			read();
			execute();
			done();
		break;
	}
}

void CPU::addr_INTERNAL_ZERO_PAGE()
{
	switch (this->t)
	{
		case 1:
			address = pc++;
			read();
			adl = data;
		break;
		case 2:
			adh = 0;
			address = ad();
			read();
			execute();
			done();
		break;
	}
}

void CPU::addr_INTERNAL_ABSOLUTE()
{
	switch (this->t)
	{
		case 1:
			address = pc++;
			read();
			adl = data;
		break;
		case 2:
			address = pc++;
			read();
			adh = data;
		break;
		case 3:
			address = ad();
			read();
			execute();
			done();
		break;
	}
}

void CPU::addr_INTERNAL_INDIRECT_X()
{
	switch (this->t)
	{
		case 1:
			address = pc++;
			read();
			bal = data;
		break;
		case 2:
			address = bal;
			read(); // discard
		break;
		case 3:
			address += x;
			address &= 0xFF;
			read();
			adl = data;
		break;
		case 4:
			++address;
			address &= 0xFF;
			read();
			adh = data;
		break;
		case 5:
			address = ad();
			read();
			execute();
			done();
		break;
	}
}

void CPU::addr_INTERNAL_ABSOLUTE_XY()
{
	switch (this->t)
	{
		case 1:
			address = pc++;
			read();
			bal = data;
		break;
		case 2:
			address = pc++;
			read();
			bah = data;
		break;
		case 3:
			idx = getIndex();
			wc = ((unsigned short)bal + (unsigned short)idx) >= 0x100;
			bal += idx;
			address = ba();
			read();
			if (!wc)
			{
				execute();
				done();
			}
		break;
		case 4:
			++bah;
			address = ba();
			read();
			execute();
			done();
		break;
	}
}

void CPU::addr_INTERNAL_ZERO_PAGE_XY()
{
	switch (this->t)
	{
		case 1:
			address = pc++;
			read();
			bal = data;
		break;
		case 2:
			bah = 0;
			address = ba();
			read(); // discard
		break;
		case 3:
			idx = getIndex();
			bal += idx; // doesn't leave page zero
			address = ba();
			read();
			execute();
			done();
		break;
	}
}

void CPU::addr_INTERNAL_INDIRECT_Y()
{
	switch (this->t)
	{
		case 1:
			address = pc++;
			read();
			ial = data;
		break;
		case 2:
			address = ial;
			read();
			bal = data;
		break;
		case 3:
			++address;
			address &= 0xFF; // doesn't leave page zero
			read();
			bah = data;
		break;
		case 4:
			wc = ((unsigned short)bal + (unsigned short)y) >= 0x100;
			bal += y;
			address = ba();
			read();
			if (!wc)
			{
				execute();
				done();
			}
		break;
		case 5:
			++bah;
			address = ba();
			read();
			execute();
			done();
		break;
	}
}

void CPU::addr_STORE_ZERO_PAGE()
{
	switch (this->t)
	{
		case 1:
			address = pc++;
			read();
			adl = data;
			execute();
		break;
		case 2:
			adh = 0;
			address = ad();
			write();
			done();
		break;
	}
}

void CPU::addr_STORE_ABSOLUTE()
{
	switch (this->t)
	{
		case 1:
			address = pc++;
			read();
			adl = data;
		break;
		case 2:
			address = pc++;
			read();
			adh = data;
			execute();
		break;
		case 3:
			address = ad();
			write();
			done();
		break;
	}
}

void CPU::addr_STORE_INDIRECT_X()
{
	switch (this->t)
	{
		case 1:
			address = pc++;
			read();
			bal = data;
		break;
		case 2:
			address = bal;
			address &= 0xFF;
			read(); // discard
		break;
		case 3:
			address += x;
			address &= 0xFF;
			read();
			adl = data;
		break;
		case 4:
			++address;
			address &= 0xFF;
			read();
			adh = data;
			execute();
		break;
		case 5:
			address = ad();
			write();
			done();
		break;
	}
}

void CPU::addr_STORE_ABSOLUTE_XY()
{
	switch (this->t)
	{
		case 1:
			address = pc++;
			read();
			bal = data;
		break;
		case 2:
			address = pc++;
			read();
			bah = data;
		break;
		case 3:
			idx = getIndex();
			address = ba();
			address += idx;
			read(); // discard (assume this is the right address, manual is ambiguous)
			execute();
		break;
		case 4:
			write();
			done();
		break;
	}
}

void CPU::addr_STORE_ZERO_PAGE_XY()
{
	switch (this->t)
	{
		case 1:
			address = pc++;
			read();
			bal = data;
		break;
		case 2:
			bah = 0;
			address = ba();
			read(); // discard
			execute();
		break;
		case 3:
			idx = getIndex();
			bal += idx; // doesn't leave page zero
			address = ba();
			write();
			done();
		break;
	}
}

void CPU::addr_STORE_INDIRECT_Y()
{
	switch (this->t)
	{
		case 1:
			address = pc++;
			read();
			ial = data;
		break;
		case 2:
			address = ial;
			read();
			bal = data;
		break;
		case 3:
			++address;
			address &= 0xFF;
			read();
			bah = data;
		break;
		case 4:
			address = ba();
			address += y;
			read(); // discard
			execute();
		break;
		case 5:
			write();
			done();
		break;
	}
}

void CPU::addr_RMW_ZERO_PAGE()
{
	switch (this->t)
	{
		case 1:
			address = pc++;
			read();
			adl = data;
		break;
		case 2:
			adh = 0;
			address = ad();
			read();
		break;
		case 3:
			write();
			execute();
		break;
		case 4:
			write();
			done();
		break;
	}
}

void CPU::addr_RMW_ABSOLUTE()
{
	switch (this->t)
	{
		case 1:
			address = pc++;
			read();
			adl = data;
		break;
		case 2:
			address = pc++;
			read();
			adh = data;
		break;
		case 3:
			address = ad();
			read();
		break;
		case 4:
			write();
			execute();
		break;
		case 5:
			write();
			done();
		break;
	}
}

void CPU::addr_RMW_ZERO_PAGE_X()
{
	switch (this->t)
	{
		case 1:
			address = pc++;
			read();
			bal = data;
		break;
		case 2:
			bah = 0;
			address = ba();
			read(); // discard
		break;
		case 3:
			bal += x; // doesn't leave page zero
			address = ba();
			read();
		break;
		case 4:
			write();
			execute();
		break;
		case 5:
			write();
			done();
		break;
	}
}

void CPU::addr_RMW_ABSOLUTE_X()
{
	switch (this->t)
	{
		case 1:
			address = pc++;
			read();
			bal = data;
		break;
		case 2:
			address = pc++;
			read();
			bah = data;
		break;
		case 3:
			address = ba();
			address += x;
			read(); // discard
		break;
		case 4:
			read();
		break;
		case 5:
			write();
			execute();
		break;
		case 6:
			write();
			done();
		break;
	}
}

void CPU::addr_MISC_PUSH()
{
	switch (this->t)
	{
		case 1:
			address = pc;
			read(); // discard
			execute();
		break;
		case 2:
			address = push();
			write();
			done();
		break;
	}
}

void CPU::addr_MISC_PULL()
{
	switch (this->t)
	{
		case 1:
			address = pc;
			read(); // discard
		break;
		case 2:
			address = sp();
			read(); // discard
		break;
		case 3:
			address = pull();
			read();
			execute();
			done();
		break;
	}
}

void CPU::addr_MISC_JSR()
{
	switch (this->t)
	{
		case 1:
			address = pc++;
			read();
			adl = data;
		break;
		case 2:
			address = push();
			read(); // discard
		break;
		case 3:
			data = pch();
			write();
			address = push();
		break;
		case 4:
			data = pcl();
			write();
		break;
		case 5:
			address = pc;
			read();
			adh = data;
			pc = ad();
			done();
		break;
	}
}

void CPU::addr_MISC_BREAK()
{
	switch (this->t)
	{
		case 1:
			address = pc;
			read(); // discard
		break;
		case 2:
			address = push();
			data = pch();
			write();
		break;
		case 3:
			address = push();
			data = pcl();
			write();
		break;
		case 4:
			address = push();
			p |= PMASK_B;
			data = p;
			write();
		break;
		case 5:
			address = IRQ_VECTOR;
			read();
			adl = data;
		break;
		case 6:
			++address;
			read();
			adh = data;
			pc = ad();
			done();
		break;
	}
}

void CPU::addr_MISC_RTI()
{
	switch (this->t)
	{
		case 1:
			address = pc;
			read(); // discard
		break;
		case 2:
			address = sp();
			read(); // discard
		break;
		case 3:
			address = pull();
			read();
			p = data; p |= PMASK_M;
		break;
		case 4:
			address = pull();
			read();
			adl = data;
		break;
		case 5:
			address = pull();
			read();
			adh = data;
			pc = ad();
			done();
		break;
	}
}

void CPU::addr_JMP_ABSOLUTE()
{
	switch (this->t)
	{
		case 1:
			address = pc++;
			read();
			adl = data;
		break;
		case 2:
			address = pc;
			read();
			adh = data;
			pc = ad();
			done();
		break;
	}
}

void CPU::addr_JMP_INDIRECT()
{
	switch (this->t)
	{
		case 1:
			address = pc++;
			read();
			ial = data;
		break;
		case 2:
			address = pc;
			read();
			iah = data;
		break;
		case 3:
			address = ia();
			read();
			adl = data;
		break;
		case 4:
			/* Interactive [newsletter] (Rockwell Intl., 1980), Issue 2,
			currently available here:
			http://www.6502.org/documents/publications/interactive/aim_interactive_2.pdf
			on page 12, documents a bug where JMP absolute does NOT
			leave the page. This causes problems for
			example for JMP ($08FF), where high byte
			is read from $800 instead of $900.
			*/
			++ial;  // emulate the bug here (don't touch iah)
			address = ia();
			read();
			adh = data;
			pc = ad();
			done();
		break;
	}
}

void CPU::addr_RTS()
{
	switch (this->t)
	{
		case 1:
			address = pc;
			read(); // discard
		break;
		case 2:
			address = sp();
			read(); // discard
		break;
		case 3:
			address = pull();
			read();
			adl = data;
		break;
		case 4:
			address = pull();
			read();
			adh = data;
		break;
		case 5:
			pc = ad();
			address = pc;
			read(); // discard
			++pc;
			done();
		break;
	}
}

void CPU::addr_BRANCH()
{
	signed short lo;
	switch (this->t)
	{
		case 1:
			address = pc++;
			read();
			offset = (signed char)data;
			execute();
			if (!branch)
			{
				done();
			}
		break;
		case 2:
			lo = pcl()+offset;
			sc = 0;
			if (lo < 0)
			{
				lo += 0x100;
				sc = -1;
			}
			else if (lo >= 0x100)
			{
				lo -= 0x100;
				sc = 1;
			}
			pc = (pc & 0xFF00) | lo;
			address = pc;
			read();
			if (sc == 0)
			{
				done();
			}
		break;
		case 3:
			unsigned short hi = pch() + sc;
			pc = (hi << 8) | pcl();
			read();
			done();
		break;
	}
}

void CPU::addr_NMI()
{
	switch (this->t)
	{
		case 1:
			address = pc;
			read(); // discard
		break;
		case 2:
			address = push();
			data = pch();
			write();
		break;
		case 3:
			address = push();
			data = pcl();
			write();
		break;
		case 4:
			address = push();
			p |= PMASK_I;
			p &= ~PMASK_B; // ???
			data = p;
			write();
		break;
		case 5:
			++address;
			read();
			adl = data;
		break;
		case 6:
			++address;
			read();
			adh = data;
			pc = ad();
			pendingNMI = false;
			done();
		break;
	}
}

void CPU::addr_RESET()
{
	switch (this->t)
	{
		case 1:
			address = pc++;
			read(); // discard
		break;
		case 2:
			s = 0xFF; // real CPU doesn't do this ???
			address = push();
			data = pch();
			read(); // discard
		break;
		case 3:
			address = push();
			data = pcl();
			read(); // discard
		break;
		case 4:
			address = push();
			p |= PMASK_I;
			data = p;
			read(); // discard
		break;
		case 5:
			address = pc++;
			read();
			adl = data;
		break;
		case 6:
			address = pc;
			read();
			adh = data;
			pc = ad();
			pendingReset = false;
			done();
		break;
	}
}

void CPU::addr_IRQ()
{
	switch (this->t)
	{
		case 1:
			address = pc;
			read(); // discard
		break;
		case 2:
			address = push();
			data = pch();
			write();
		break;
		case 3:
			address = push();
			data = pcl(); // ???
			write();
		break;
		case 4:
			address = push();
			p |= PMASK_I;
			p &= ~PMASK_B; // ???
			data = p;
			write();
		break;
		case 5:
			address = pc++;
			read();
			adl = data;
		break;
		case 6:
			address = pc;
			read();
			adh = data;
			pc = ad();
			pendingIRQ = false;
			done();
		break;
	}
}

void CPU::read()
{
	this->data = this->addressBus.read(this->address);
}

void CPU::write()
{
	this->addressBus.write(this->address,this->data);
}

void CPU::execute()
{
	(this->*this->exec[this->opcode])();
}

void CPU::done()
{
	this->t = -1;
}


unsigned char CPU::pch()
{
	return (unsigned char)(this->pc >> 8);
}

unsigned char CPU::pcl()
{
	return (unsigned char)(this->pc);
}

unsigned short CPU::sp()
{
	return 0x100+this->s;
}

unsigned short CPU::push()
{
	const unsigned short psp = sp();
	--this->s;
	return psp;
}

unsigned short CPU::pull()
{
	++this->s;
	return sp();
}

unsigned char CPU::getIndex()
{
	// opcode: aaabbbcc
	const int aaa = (this->opcode & 0xE0) >> 5;
	int bbb = (this->opcode & 0x1C) >> 2;
	int cc = (this->opcode & 0x03);
	if (bbb == 0)
	{
		return this->x;
	}
	if (bbb == 4 || bbb == 6)
	{
		return this->y;
	}
	if (bbb == 5 || bbb == 7)
	{
		if (cc == 2 && (aaa == 4 || aaa == 5))
		{
			return this->y;
		}
		return this->x;
	}
	return 0;
}

unsigned short CPU::ad()
{
	return combine(this->adl,this->adh);
}

unsigned short CPU::ia()
{
	return combine(this->ial,this->iah);
}

unsigned short CPU::ba()
{
	return combine(this->bal,this->bah);
}

unsigned short CPU::combine(const unsigned char lo, const unsigned char hi)
{
	return hi << 8 | lo;
}

void CPU::setStatusRegisterNZ(const unsigned char val)
{
	setP(PMASK_N,val & 0x80);
	setP(PMASK_Z,!val);
}

void CPU::LDA()
{
	this->a = this->data;
	setStatusRegisterNZ(this->a);
}

void CPU::LDX()
{
	this->x = this->data;
	setStatusRegisterNZ(this->x);
}

void CPU::LDY()
{
	this->y = this->data;
	setStatusRegisterNZ(this->y);
}

void CPU::STA()
{
	this->data = this->a;
}

void CPU::STX()
{
	this->data = this->x;
}

void CPU::STY()
{
	this->data = this->y;
}

void CPU::compare(const unsigned char r)
{
	const signed short tmp = r - this->data;
	setP(PMASK_C,0 <= tmp && tmp < 0x100);
	setStatusRegisterNZ((const signed char)tmp);
}

void CPU::CMP()
{
	compare(this->a);
}

void CPU::CPX()
{
	compare(this->x);
}

void CPU::CPY()
{
	compare(this->y);
}

void CPU::AND()
{
	this->a &= this->data;
	setStatusRegisterNZ(this->a);
}

void CPU::ORA()
{
	this->a |= this->data;
	setStatusRegisterNZ(this->a);
}

void CPU::EOR()
{
	this->a ^= this->data;
	setStatusRegisterNZ(this->a);
}






void CPU::ASL()
{
	this->data = shiftLeft(this->data);
}

void CPU::ASL_A()
{
	this->a = shiftLeft(this->a);
}

void CPU::LSR()
{
	this->data = shiftRight(this->data);
}

void CPU::LSR_A()
{
	this->a = shiftRight(this->a);
}

void CPU::ROL()
{
	this->data = rotateLeft(this->data);
}

void CPU::ROL_A()
{
	this->a = rotateLeft(this->a);
}

void CPU::ROR()
{
	this->data = rotateRight(this->data);
}

void CPU::ROR_A()
{
	this->a = rotateRight(this->a);
}

unsigned char CPU::shiftLeft(unsigned char byt)
{
	setP(PMASK_C,byt & 0x80);
	byt <<= 1;
	setStatusRegisterNZ(byt);
	return byt;
}

unsigned char CPU::shiftRight(unsigned char byt)
{
	setP(PMASK_C,byt & 0x01);
	byt >>= 1;
	setStatusRegisterNZ(byt);
	return byt;
}

unsigned char CPU::rotateLeft(unsigned char byt)
{
	const bool newCarry = (byt & 0x80);

	byt <<= 1;

	if (this->p & PMASK_C)
	{
		byt |= 0x01;
	}

	setP(PMASK_C,newCarry);
	setStatusRegisterNZ(byt);

	return byt;
}

unsigned char CPU::rotateRight(unsigned char byt)
{
	const bool newCarry = (byt & 0x01);

	byt >>= 1;

	if (this->p & PMASK_C)
	{
		byt |= 0x80;
	}

	setP(PMASK_C,newCarry);
	setStatusRegisterNZ(byt);

	return byt;
}






void CPU::ADC()
{
	/*
	This method based on ADC from the POM1 Apple 1 emulator.
	Copyright (C) 2000, by Verhille Arnaud, GPLv2 license.
	*/
	int Op1 = this->a;
	int Op2 = this->data;
	if (this->p & PMASK_D)
	{
		setP(PMASK_Z,!(Op1 + Op2 + !!(this->p & PMASK_C) & 0xff));
		int tmp = (Op1 & 0xf) + (Op2 & 0xf) + !!(this->p & PMASK_C);
		tmp = tmp >= 10 ? tmp + 6 : tmp;
		this->a = tmp;
		tmp = (Op1 & 0xf0) + (Op2 & 0xf0) + (tmp & 0xf0);
		setP(PMASK_N,tmp < 0);
		setP(PMASK_V,((Op1 ^ tmp) & ~(Op1 ^ Op2) & 0x80));
		tmp = this->a & 0xf | (tmp >= 160 ? tmp + 96 : tmp);
		setP(PMASK_C,tmp >= 0x100);
		this->a = tmp & 0xff;
	}
	else
	{
		int tmp = Op1 + Op2 + !!(this->p & PMASK_C);
		this->a = tmp & 0xFF;
		setP(PMASK_V,((Op1 ^ this->a) & ~(Op1 ^ Op2) & 0x80));
		setP(PMASK_C,tmp >= 0x100);
		setStatusRegisterNZ(this->a);
	}
}

void CPU::SBC()
{
	/*
	This method based on SBC from the POM1 Apple 1 emulator.
	Copyright (C) 2000, by Verhille Arnaud, GPLv2 license.
	*/
	int Op1 = this->a;
	int Op2 = this->data;
	if (this->p & PMASK_D)
	{
		int tmp = (Op1 & 0xf) - (Op2 & 0xf) - !(this->p & PMASK_C);
		tmp = (tmp & 0x10) != 0 ? tmp - 6 : tmp;
		this->a = tmp;
		tmp = (Op1 & 0xf0) - (Op2 & 0xf0) - (this->a & 0x10);
		this->a = this->a & 0xf | ((tmp & 0x100) != 0 ? tmp - 96 : tmp);
		tmp = Op1 - Op2 - !(this->p & PMASK_C);
		setP(PMASK_C,0 <= tmp && tmp < 0x100);
		setStatusRegisterNZ(tmp);
	}
	else
	{
		int tmp = Op1 - Op2 - !(this->p & PMASK_C);
		this->a = tmp & 0xff;
		setP(PMASK_V,((Op1 ^ Op2) & (Op1 ^ this->a) & 0x80));
		setP(PMASK_C,0 <= tmp && tmp < 0x100);
		setStatusRegisterNZ(this->a);
	}
}




void CPU::INC()
{
	++this->data;
	setStatusRegisterNZ(this->data);
}

void CPU::DEC()
{
	--this->data;
	setStatusRegisterNZ(this->data);
}

void CPU::INX()
{
	++this->x;
	setStatusRegisterNZ(this->x);
}

void CPU::INY()
{
	++this->y;
	setStatusRegisterNZ(this->y);
}

void CPU::DEX()
{
	--this->x;
	setStatusRegisterNZ(this->x);
}

void CPU::DEY()
{
	--this->y;
	setStatusRegisterNZ(this->y);
}

void CPU::setP(const unsigned char mask, const unsigned char val)
{
	if (val)
		this->p |= mask;
	else
		this->p &= ~mask;
}

void CPU::BIT()
{
	setP(PMASK_V,this->data & 0x40);
	setP(PMASK_N,this->data & 0x80);
	setP(PMASK_Z,!(this->data & this->a));
}

void CPU::PHA()
{
	this->data = this->a;
}

void CPU::PHP()
{
	this->data = p;
}

void CPU::PLA()
{
	this->a = this->data;
	setStatusRegisterNZ(this->a);
}

void CPU::PLP()
{
	this->p = this->data;
	this->p |= PMASK_M;
}

void CPU::BRK()
{
}

void CPU::RTI()
{
}

void CPU::JMP()
{
}

void CPU::RTS()
{
}

void CPU::JSR()
{
}

void CPU::BNE()
{
	this->branch = !(this->p & PMASK_Z);
}

void CPU::BEQ()
{
	this->branch = this->p & PMASK_Z;
}

void CPU::BVC()
{
	this->branch = !(this->p & PMASK_V);
}

void CPU::BVS()
{
	this->branch = this->p & PMASK_V;
}

void CPU::BCC()
{
	this->branch = !(this->p & PMASK_C);
}

void CPU::BCS()
{
	this->branch = this->p & PMASK_C;
}

void CPU::BPL()
{
	this->branch = !(this->p & PMASK_N);
}

void CPU::BMI()
{
	this->branch = this->p & PMASK_N;
}

void CPU::TAX()
{
	this->x = this->a;
	setStatusRegisterNZ(this->x);
}

void CPU::TXA()
{
	this->a = this->x;
	setStatusRegisterNZ(this->a);
}

void CPU::TAY()
{
	this->y = this->a;
	setStatusRegisterNZ(this->y);
}

void CPU::TYA()
{
	this->a = this->y;
	setStatusRegisterNZ(this->a);
}

void CPU::TXS()
{
	this->s = this->x;
	// make sure this doesn't affect status register
	// it doesn't
}

void CPU::TSX()
{
	this->x = this->s;
	setStatusRegisterNZ(this->x);
	// make sure this does affect status register
	// it does
}

void CPU::CLC()
{
	this->p &= ~PMASK_C;
}

void CPU::SEC()
{
	this->p |= PMASK_C;
}

void CPU::CLI()
{
	this->p &= ~PMASK_I;
}

void CPU::SEI()
{
	this->p |= PMASK_I;
}

void CPU::CLV()
{
	this->p &= ~PMASK_V;
}

void CPU::CLD()
{
	this->p &= ~PMASK_D;
}

void CPU::SED()
{
	this->p |= PMASK_D;
}

void CPU::NOP()
{
}

// TODO implement 6502 undocumented instructions
// See:
// "The 6502/65C02/65C816 Instruction Set Decoded," by Neil Parker, http://axis.llx.com/~nparker/a2/opcodes.html
// "Extra Instructions Of The 65XX Series CPU," by Adam Vardy, http://www.ffd2.com/fridge/docs/6502-NMOS.extra.opcodes
void CPU::Unoff()
{
}

void CPU::Unoff1()
{
}

void CPU::Unoff2()
{
	this->pc++;
}

void CPU::Unoff3()
{
	this->pc += 2;
}

void CPU::Hang()
{
	this->pc--;
}
