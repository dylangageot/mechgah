/**
 * \file instructions.c
 * \author Hily Nicolas
 * \version 1.0
 * \date 2019-03-05
 */

#include "instructions.h"
#include <stdlib.h>

/* Opcode LUT */
static Opcode opcode[256] = {
	{_BRK, IMP}, {_ORA, INX}, {NULL, NUL}, {NULL, NUL}, /* 0x00 */
	{NULL, NUL}, {_ORA, ZER}, {_ASL, ZER}, {NULL, NUL}, /* 0x04 */
	{_PHP, IMP}, {_ORA, IMM}, {_ASL, ACC}, {NULL, NUL}, /* 0x08 */
	{NULL, NUL}, {_ORA, ABS}, {_ASL, ABS}, {NULL, NUL}, /* 0x0C */
	{_BPL, IMP}, {_ORA, INY}, {NULL, NUL}, {NULL, NUL},	/* 0x10 */
	{NULL, NUL}, {_ORA, ZEX}, {_ASL, ZEX}, {NULL, NUL},	/* 0x14 */
	{_CLC, IMP}, {_ORA, ABY}, {NULL, NUL}, {NULL, NUL},	/* 0x18 */
	{NULL, NUL}, {_ORA, ABX}, {_ASL, ABX}, {NULL, NUL}, /* 0x1C */
	{_JSR, IMP}, {_AND, INX}, {NULL, NUL}, {NULL, NUL},	/* 0x20 */
	{_BIT, ZER}, {_AND, ZER}, {_ROL, ZER}, {NULL, NUL},	/* 0x24 */
	{_PLP, IMP}, {_AND, IMM}, {_ROL, ACC}, {NULL, NUL}, /* 0x28 */
	{_BIT, ABS}, {_AND, ABS}, {_ROL, ABS}, {NULL, NUL},	/* 0x2C */
	{_BMI, IMP}, {_AND, INY}, {NULL, NUL}, {NULL, NUL},	/* 0x30 */
	{NULL, NUL}, {_AND, ZEX}, {_ROL, ZEX}, {NULL, NUL}, /* 0x34 */
	{_SEC, IMP}, {_AND, ABY}, {NULL, NUL}, {NULL, NUL}, /* 0x38 */
	{NULL, NUL}, {_AND, ABX}, {_ROL, ABX}, {NULL, NUL},	/* 0x3C */
	{_RTI, IMP}, {_EOR, INX}, {NULL, NUL}, {NULL, NUL}, /* 0x40 */
	{NULL, NUL}, {_EOR, ZER}, {_LSR, ZER}, {NULL, NUL},	/* 0x44 */
	{_PHA, IMP}, {_EOR, IMM}, {_LSR, ACC}, {NULL, NUL},	/* 0x48 */
	{_JMP, ABS}, {_EOR, ABS}, {_LSR, ABS}, {NULL, NUL}, /* 0x4C */
	{_BVC, IMP}, {_EOR, INY}, {NULL, NUL}, {NULL, NUL}, /* 0x50 */
	{NULL, NUL}, {_EOR, ZEX}, {_LSR, ZEX}, {NULL, NUL}, /* 0x54 */
	{_CLI, IMP}, {_EOR, ABY}, {NULL, NUL}, {NULL, NUL},	/* 0x58 */
	{NULL, NUL}, {_EOR, ABX}, {_LSR, ABX}, {NULL, NUL},	/* 0x5C */
	{_RTS, IMP}, {_ADC, INX}, {NULL, NUL}, {NULL, NUL},	/* 0x60 */
	{NULL, NUL}, {_ADC, ZER}, {_ROR, ZER}, {NULL, NUL}, /* 0x64 */
	{_PLA, IMP}, {_ADC, IMM}, {_ROR, IMM}, {NULL, NUL}, /* 0x68 */
	{_JMP, ABI}, {_ADC, ABS}, {_ROR, ABS}, {NULL, NUL},	/* 0x6C */
	{_BVS, IMP}, {_ADC, INY}, {NULL, NUL}, {NULL, NUL},	/* 0x70 */
	{NULL, NUL}, {_ADC, ZEX}, {_ROR, ZER}, {NULL, NUL}, /* 0x74 */
	{_SEI, IMP}, {_ADC, ABY}, {NULL, NUL}, {NULL, NUL},	/* 0x78 */
	{NULL, NUL}, {_ADC, ABX}, {_ROR, ABX}, {NULL, NUL}, /* 0x7C */
	{NULL, NUL}, {_STA, INX}, {NULL, NUL}, {NULL, NUL}, /* 0x80 */
	{_STY, ZER}, {_STA, ZER}, {_STX, ZER}, {NULL, NUL},	/* 0x84 */
	{_DEY, IMP}, {NULL, NUL}, {_TXA, IMP}, {NULL, NUL}, /* 0x88 */
	{_STY, ABS}, {_STA, ABS}, {_STX, ABS}, {NULL, NUL}, /* 0x8C */
	{_BCC, IMP}, {_STA, INY}, {NULL, NUL}, {NULL, NUL}, /* 0x90 */
	{_STY, ZEX}, {_STA, ZEX}, {_STX, ZEY}, {NULL, NUL}, /* 0x94 */
	{_TYA, IMP}, {_STA, ABY}, {_TXS, IMP}, {NULL, NUL}, /* 0x98 */
	{NULL, NUL}, {_STA, ABX}, {NULL, NUL}, {NULL, NUL}, /* 0x9C */
	{_LDY, IMM}, {_LDA, INX}, {_LDX, IMM}, {NULL, NUL}, /* 0xA0 */
	{_LDY, ZER}, {_LDA, ZER}, {_LDX, ZER}, {NULL, NUL}, /* 0xA4 */
	{_TAY, IMP}, {_LDA, IMM}, {_TAX, IMP}, {NULL, NUL}, /* 0xA8 */
	{_LDY, ABS}, {_LDA, ABS}, {_LDX, ABS}, {NULL, NUL}, /* 0xAC */
	{_BCS, IMP}, {_LDA, INY}, {NULL, NUL}, {NULL, NUL}, /* 0xB0 */
	{_LDY, ZEX}, {_LDA, ZEX}, {_LDX, ZEY}, {NULL, NUL}, /* 0xB4 */
	{_CLV, IMP}, {_LDA, ABY}, {_TSX, IMP}, {NULL, NUL}, /* 0xB8 */
	{_LDY, ABX}, {_LDA, ABX}, {_LDX, ABY}, {NULL, NUL}, /* 0xBC */
	{_CPY, IMM}, {_CMP, INX}, {NULL, NUL}, {NULL, NUL}, /* 0xC0 */
	{_CPY, ZER}, {_CMP, ZER}, {_DEC, ZER}, {NULL, NUL}, /* 0xC4 */
	{_INY, IMP}, {_CMP, IMM}, {_DEX, IMP}, {NULL, NUL}, /* 0xC8 */
	{_CPY, ABS}, {_CMP, ABS}, {_DEC, ABS}, {NULL, NUL}, /* 0xCC */
	{_BNE, IMP}, {_CMP, INY}, {NULL, NUL}, {NULL, NUL}, /* 0xD0 */
	{NULL, NUL}, {_CMP, ZEX}, {_DEC, ZEX}, {NULL, NUL}, /* 0xD4 */
	{_CLD, IMP}, {_CMP, ABY}, {NULL, NUL}, {NULL, NUL}, /* 0xD8 */
	{NULL, NUL}, {_CMP, ABX}, {_DEC, ABX}, {NULL, NUL}, /* 0xDC */
	{_CPX, IMM}, {_SBC, INX}, {NULL, NUL}, {NULL, NUL}, /* 0xE0 */
	{_CPX, ZER}, {_SBC, ZER}, {_INC, ZER}, {NULL, NUL}, /* 0xE4 */
	{_INX, IMP}, {_SBC, IMM}, {_NOP, IMP}, {NULL, NUL}, /* 0xE8 */
	{_CPX, ABS}, {_SBC, ABS}, {_INC, ABS}, {NULL, NUL}, /* 0xEC */
	{_BEQ, IMP}, {_SBC, INY}, {NULL, NUL}, {NULL, NUL}, /* 0xF0 */
	{NULL, NUL}, {_SBC, ZEX}, {_INC, ZEX}, {NULL, NUL}, /* 0xF4 */
	{_SED, IMP}, {_SBC, ABY}, {NULL, NUL}, {NULL, NUL}, /* 0xF8 */
	{NULL, NUL}, {_SBC, ABX}, {_INC, ABX}, {NULL, NUL}	/* 0xFC */
};

void _SET_SIGN(CPU *cpu, uint8_t *src) {
	if (*src & 0x80)
		cpu->P |= 0x80;
	else 
		cpu->P &= ~0x80;
}

void _SET_ZERO(CPU *cpu, uint8_t *src) {
	if (*src == 0)
		cpu->P |= 0x02;
	else
		cpu->P &= ~0x02;
}

void _SET_CARRY(CPU *cpu, uint8_t cond) {
	if (cond)
		cpu->P |= 0x01;
	else
		cpu->P &= ~0x01;
}

void _SET_OVERFLOW(CPU *cpu, uint8_t cond) {
	if (cond)
		cpu->P |= 0x40;
	else
		cpu->P &= ~0x40;
}

void _SET_INTERRUPT(CPU *cpu) {
	cpu->P |= 0x04;
}

void _SET_BREAK(CPU *cpu) {
	cpu->P |= 0x10;
}

uint16_t _REL_ADDR(CPU *cpu, int8_t *src) {
	return cpu->PC + (int16_t) *src;
}

void _SET_SR(CPU *cpu, uint8_t *src) {
	cpu->P = *src;
}

uint8_t _GET_SR(CPU *cpu) {
	return cpu->P;
}

uint8_t _PULL(CPU *cpu) {
	return *cpu->rmap->get(cpu->rmap->memoryMap, AS_CPU, 0x0100 | (++cpu->SP));
}

void _PUSH(CPU *cpu, uint8_t *src) {
	uint8_t *ptr = NULL;
	ptr = cpu->rmap->get(cpu->rmap->memoryMap, AS_CPU, 0x0100 | (cpu->SP--));
	*ptr = *src;
}

uint8_t _LOAD(CPU *cpu, uint16_t address) {
	return *cpu->rmap->get(cpu->rmap->memoryMap, AS_CPU, address);
}

void _STORE(CPU *cpu, uint16_t address, uint8_t *src) {
	uint8_t *ptr = NULL;
	ptr = cpu->rmap->get(cpu->rmap->memoryMap, AS_CPU, address);
	*ptr = *src;
}

uint8_t _IF_CARRY(CPU *cpu) {
	return (cpu->P & 0x01) == 0x01;
}

uint8_t _IF_OVERFLOW(CPU *cpu) {
	return (cpu->P & 0x40) == 0x40;
}

uint8_t _IF_SIGN(CPU *cpu) {
	return (cpu->P & 0x80) == 0x80;
}

uint8_t _IF_ZERO(CPU *cpu) {
	return (cpu->P & 0x02) == 0x02;
}

uint8_t _IF_INTERRUPT(CPU *cpu) {
	return (cpu->P & 0x04) == 0x04;
}

uint8_t _IF_BREAK(CPU *cpu) {
	return (cpu->P & 0x10) == 0x10;
}

uint8_t Instruction_Fetch(Instruction *self, CPU *cpu) {
	if ((self == NULL) || (cpu == NULL))
		return 0;

	/* Fetch data in memory space with PC value */
	uint8_t *opc = cpu->rmap->get(cpu->rmap->memoryMap, AS_CPU, cpu->PC);
	cpu->PC++;
	self->opcode = opcode[*(opc++)];

	/* Decode and update PC */
	if (self->opcode.addressingMode <= 1) {
		return 1;
	} else if ((self->opcode.addressingMode >= 2) &&
			(self->opcode.addressingMode <= 8)) {
		self->opcodeArg[0] = *opc;
		cpu->PC++;
		return 1;
	} else if ((self->opcode.addressingMode >= 9) &&
			(self->opcode.addressingMode <= 12)) {
		uint8_t i;
		for (i = 0; i < 2; i++)
			self->opcodeArg[i] = *(opc + i);
		cpu->PC += 2;
		return 1;
	}

	return 0;
}

uint8_t Instruction_Resolve(Instruction *self, CPU *cpu) {
	if ((self == NULL) || (cpu == NULL))
		return 0;

	uint8_t lWeight, hWeight;
	uint16_t address = 0;
	self->pageCrossed = 0;
	Mapper *mapper = mapper;
	switch (self->opcode.addressingMode) {
		case IMP : /* Implied : Nothing to do*/
			break;

		case ACC : /*Accumulator : Nothing to do */
			break;

		case ZEX :
			address = (0x00FF) & (uint16_t)(self->opcodeArg[0] + cpu->X);
			self->dataMem = mapper->get(mapper->memoryMap, AS_CPU, address);
			break;

		case ZEY :
			address = (0x00FF) & (uint16_t)(self->opcodeArg[0] + cpu->Y);
			self->dataMem = mapper->get(mapper->memoryMap, AS_CPU, address);
			break;

		case INX :
			address = (0x00FF) & (uint16_t)(self->opcodeArg[0] + cpu->X);
			lWeight = *(mapper->get(mapper->memoryMap, AS_CPU, address));
			hWeight = *(mapper->get(mapper->memoryMap, AS_CPU, address + 1));
			address = (uint16_t)(hWeight<<8 + lWeight);
			self->dataMem = mapper->get(mapper->memoryMap, AS_CPU, address);
			break;

		case INY :
			address = (0x00FF) & (uint16_t)(self->opcodeArg[0]);
			lWeight = *(mapper->get(mapper->memoryMap, AS_CPU, address));
			hWeight = *(mapper->get(mapper->memoryMap, AS_CPU, address + 1));
			address = (uint16_t)(hWeight<<8 + lWeight);
			if ((address & 0xFF00) != ((address + cpu->Y) & 0xFF00))
				self->pageCrossed = 1;
			address = (uint16_t)(hWeight<<8 + lWeight + cpu->Y);
			self->dataMem = mapper->get(mapper->memoryMap, AS_CPU, address);
			break;

		case IMM : /* Immediate : Nothing to do*/
			break;

		case ZER :
			address = (0x00FF) & (uint16_t)(self->opcodeArg[0]);
			self->dataMem = mapper->get(mapper->memoryMap, AS_CPU, address);
			break;

		case REL : /* Immediate : Nothing to do*/
			break;

		case ABS :
			address = (uint16_t)(self->opcodeArg[1]<<8 + self->opcodeArg[0]);
			self->dataMem = mapper->get(mapper->memoryMap, AS_CPU, address);
			break;

		case ABX :
			address = (uint16_t)(self->opcodeArg[1]<<8 + self->opcodeArg[0]);
			if ((address & 0xFF00) != ((address + cpu->X) & 0xFF00))
				self->pageCrossed = 1;
			address = (uint16_t)(self->opcodeArg[1]<<8 + self->opcodeArg[0] + cpu->X);
			self->dataMem = mapper->get(mapper->memoryMap, AS_CPU, address);
			break;

		case ABY :
			address = (uint16_t)(self->opcodeArg[1]<<8 + self->opcodeArg[0]);
			if ((address & 0xFF00) != ((address + cpu->X) & 0xFF00))
				self->pageCrossed = 1;
			address = (uint16_t)(self->opcodeArg[1]<<8 + self->opcodeArg[0] + cpu->Y);
			self->dataMem = mapper->get(mapper->memoryMap, AS_CPU, address);
			break;

		case ABI :
			address = (uint16_t)(self->opcodeArg[1]<<8 + self->opcodeArg[0]);
			lWeight = *(mapper->get(mapper->memoryMap, AS_CPU, address));
			hWeight = *(mapper->get(mapper->memoryMap, AS_CPU, address + 1));
			address = (uint16_t)(hWeight<<8 + lWeight);
			self->dataMem = mapper->get(mapper->memoryMap, AS_CPU, address);
			break;

		default :
			return 0;
			break;
	}
	return 1;
}

uint8_t _ADC(CPU *cpu, Instruction *arg) {
	uint8_t clk[] = {0, 0, 4, 0, 6, 5, 2, 3, 0, 4, 4, 4, 0};
	uint16_t temp = *arg->dataMem + cpu->A + (_IF_CARRY(cpu) ? 1 : 0);
	/* Set bit flag */
	_SET_ZERO(cpu, (uint8_t*) &temp);
	_SET_SIGN(cpu, (uint8_t*) &temp);
	_SET_OVERFLOW(cpu, !((cpu->A ^ *arg->dataMem) & 0x80) && 
			((cpu->A ^ temp) & 0x80));
	_SET_CARRY(cpu, temp > 0xFF);
	/* Save in Accumulator */
	cpu->A = ((uint8_t) temp & 0xFF);
	/* Manage CPU cycle */
	if ((arg->opcode.addressingMode == ABX) ||
		(arg->opcode.addressingMode == ABY) ||
		(arg->opcode.addressingMode == INY))
		return clk[arg->opcode.addressingMode] + arg->pageCrossed;
	else
		return clk[arg->opcode.addressingMode];
}

uint8_t _AND(CPU *cpu, Instruction *arg){return 0;}

uint8_t _ASL(CPU *cpu, Instruction *arg) {
	uint8_t clk[] = {0, 2, 6, 0, 0, 0, 0, 5, 0, 6, 7, 0, 0};
	/* Execute */
	_SET_CARRY(cpu, *arg->dataMem & 0x80);
	*arg->dataMem <<= 1;
	_SET_SIGN(cpu, arg->dataMem);
	_SET_ZERO(cpu, arg->dataMem);
	/* Manage CPU cycle */
	return clk[arg->opcode.addressingMode];
}

uint8_t _BCC(CPU *cpu, Instruction *arg){return 0;}
uint8_t _BCS(CPU *cpu, Instruction *arg){return 0;}
uint8_t _BEQ(CPU *cpu, Instruction *arg){return 0;}
uint8_t _BIT(CPU *cpu, Instruction *arg){return 0;}
uint8_t _BMI(CPU *cpu, Instruction *arg){return 0;}
uint8_t _BNE(CPU *cpu, Instruction *arg){return 0;}
uint8_t _BPL(CPU *cpu, Instruction *arg){return 0;}
uint8_t _BRK(CPU *cpu, Instruction *arg){return 0;}
uint8_t _BVC(CPU *cpu, Instruction *arg){return 0;}
uint8_t _BVS(CPU *cpu, Instruction *arg){return 0;}
uint8_t _CLC(CPU *cpu, Instruction *arg){return 0;}
uint8_t _CLD(CPU *cpu, Instruction *arg){return 0;}
uint8_t _CLI(CPU *cpu, Instruction *arg){return 0;}
uint8_t _CLV(CPU *cpu, Instruction *arg){return 0;}
uint8_t _CMP(CPU *cpu, Instruction *arg){return 0;}
uint8_t _CPX(CPU *cpu, Instruction *arg){return 0;}
uint8_t _CPY(CPU *cpu, Instruction *arg){return 0;}
uint8_t _DEC(CPU *cpu, Instruction *arg){return 0;}
uint8_t _DEX(CPU *cpu, Instruction *arg){return 0;}
uint8_t _DEY(CPU *cpu, Instruction *arg){return 0;}
uint8_t _EOR(CPU *cpu, Instruction *arg){return 0;}
uint8_t _INC(CPU *cpu, Instruction *arg){return 0;}
uint8_t _INX(CPU *cpu, Instruction *arg){return 0;}
uint8_t _INY(CPU *cpu, Instruction *arg){return 0;}
uint8_t _JMP(CPU *cpu, Instruction *arg){return 0;}
uint8_t _JSR(CPU *cpu, Instruction *arg){return 0;}
uint8_t _LDA(CPU *cpu, Instruction *arg){return 0;}
uint8_t _LDX(CPU *cpu, Instruction *arg){return 0;}
uint8_t _LDY(CPU *cpu, Instruction *arg){return 0;}
uint8_t _LSR(CPU *cpu, Instruction *arg){return 0;}
uint8_t _NOP(CPU *cpu, Instruction *arg){return 0;}
uint8_t _ORA(CPU *cpu, Instruction *arg){return 0;}
uint8_t _PHA(CPU *cpu, Instruction *arg){return 0;}
uint8_t _PHP(CPU *cpu, Instruction *arg){return 0;}
uint8_t _PLA(CPU *cpu, Instruction *arg){return 0;}
uint8_t _PLP(CPU *cpu, Instruction *arg){return 0;}
uint8_t _ROL(CPU *cpu, Instruction *arg){return 0;}
uint8_t _ROR(CPU *cpu, Instruction *arg){return 0;}
uint8_t _RTI(CPU *cpu, Instruction *arg){return 0;}
uint8_t _RTS(CPU *cpu, Instruction *arg){return 0;}
uint8_t _SBC(CPU *cpu, Instruction *arg){return 0;}
uint8_t _SEC(CPU *cpu, Instruction *arg){return 0;}
uint8_t _SED(CPU *cpu, Instruction *arg){return 0;}
uint8_t _SEI(CPU *cpu, Instruction *arg){return 0;}
uint8_t _STA(CPU *cpu, Instruction *arg){return 0;}
uint8_t _STX(CPU *cpu, Instruction *arg){return 0;}
uint8_t _STY(CPU *cpu, Instruction *arg){return 0;}
uint8_t _TAX(CPU *cpu, Instruction *arg){return 0;}
uint8_t _TAY(CPU *cpu, Instruction *arg){return 0;}
uint8_t _TSX(CPU *cpu, Instruction *arg){return 0;}
uint8_t _TXA(CPU *cpu, Instruction *arg){return 0;}
uint8_t _TXS(CPU *cpu, Instruction *arg){return 0;}
uint8_t _TYA(CPU *cpu, Instruction *arg){return 0;}
