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
	{_BRK, IMP, 7}, {_ORA, INX, 6}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x00 */
	{NULL, NUL, 0}, {_ORA, ZER, 3}, {_ASL, ZER, 5}, {NULL, NUL, 0}, /* 0x04 */
	{_PHP, IMP, 3}, {_ORA, IMM, 2}, {_ASL, ACC, 2}, {NULL, NUL, 0}, /* 0x08 */
	{NULL, NUL, 0}, {_ORA, ABS, 4}, {_ASL, ABS, 6}, {NULL, NUL, 0}, /* 0x0C */
	{_BPL, IMP, 2}, {_ORA, INY, 5}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x10 */
	{NULL, NUL, 0}, {_ORA, ZEX, 4}, {_ASL, ZEX, 6}, {NULL, NUL, 0}, /* 0x14 */
	{_CLC, IMP, 2}, {_ORA, ABY, 4}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x18 */
	{NULL, NUL, 0}, {_ORA, ABX, 4}, {_ASL, ABX, 7}, {NULL, NUL, 0}, /* 0x1C */
	{_JSR, IMP, 6}, {_AND, INX, 6}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x20 */
	{_BIT, ZER, 3}, {_AND, ZER, 3}, {_ROL, ZER, 5}, {NULL, NUL, 0}, /* 0x24 */
	{_PLP, IMP, 4}, {_AND, IMM, 2}, {_ROL, ACC, 2}, {NULL, NUL, 0}, /* 0x28 */
	{_BIT, ABS, 4}, {_AND, ABS, 4}, {_ROL, ABS, 6}, {NULL, NUL, 0}, /* 0x2C */
	{_BMI, IMP, 2}, {_AND, INY, 5}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x30 */
	{NULL, NUL, 0}, {_AND, ZEX, 3}, {_ROL, ZEX, 6}, {NULL, NUL, 0}, /* 0x34 */
	{_SEC, IMP, 2}, {_AND, ABY, 4}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x38 */
	{NULL, NUL, 0}, {_AND, ABX, 4}, {_ROL, ABX, 7}, {NULL, NUL, 0}, /* 0x3C */
	{_RTI, IMP, 6}, {_EOR, INX, 6}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x40 */
	{NULL, NUL, 0}, {_EOR, ZER, 3}, {_LSR, ZER, 5}, {NULL, NUL, 0}, /* 0x44 */
	{_PHA, IMP, 3}, {_EOR, IMM, 2}, {_LSR, ACC, 2}, {NULL, NUL, 0}, /* 0x48 */
	{_JMP, ABS, 3}, {_EOR, ABS, 4}, {_LSR, ABS, 6}, {NULL, NUL, 0}, /* 0x4C */
	{_BVC, IMP, 2}, {_EOR, INY, 5}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x50 */
	{NULL, NUL, 0}, {_EOR, ZEX, 4}, {_LSR, ZEX, 6}, {NULL, NUL, 0}, /* 0x54 */
	{_CLI, IMP, 2}, {_EOR, ABY, 4}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x58 */
	{NULL, NUL, 0}, {_EOR, ABX, 4}, {_LSR, ABX, 7}, {NULL, NUL, 0}, /* 0x5C */
	{_RTS, IMP, 6}, {_ADC, INX, 6}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x60 */
	{NULL, NUL, 0}, {_ADC, ZER, 3}, {_ROR, ZER, 5}, {NULL, NUL, 0}, /* 0x64 */
	{_PLA, IMP, 4}, {_ADC, IMM, 2}, {_ROR, ACC, 2}, {NULL, NUL, 0}, /* 0x68 */
	{_JMP, ABI, 5}, {_ADC, ABS, 4}, {_ROR, ABS, 6}, {NULL, NUL, 0}, /* 0x6C */
	{_BVS, IMP, 2}, {_ADC, INY, 5}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x70 */
	{NULL, NUL, 0}, {_ADC, ZEX, 4}, {_ROR, ZEX, 6}, {NULL, NUL, 0}, /* 0x74 */
	{_SEI, IMP, 2}, {_ADC, ABY, 4}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x78 */
	{NULL, NUL, 0}, {_ADC, ABX, 4}, {_ROR, ABX, 7}, {NULL, NUL, 0}, /* 0x7C */
	{NULL, NUL, 0}, {_STA, INX, 6}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x80 */
	{_STY, ZER, 3}, {_STA, ZER, 3}, {_STX, ZER, 3}, {NULL, NUL, 0}, /* 0x84 */
	{_DEY, IMP, 2}, {NULL, NUL, 0}, {_TXA, IMP, 2}, {NULL, NUL, 0}, /* 0x88 */
	{_STY, ABS, 4}, {_STA, ABS, 4}, {_STX, ABS, 4}, {NULL, NUL, 0}, /* 0x8C */
	{_BCC, IMP, 2}, {_STA, INY, 6}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x90 */
	{_STY, ZEX, 4}, {_STA, ZEX, 4}, {_STX, ZEY, 4}, {NULL, NUL, 0}, /* 0x94 */
	{_TYA, IMP, 2}, {_STA, ABY, 5}, {_TXS, IMP, 2}, {NULL, NUL, 0}, /* 0x98 */
	{NULL, NUL, 0}, {_STA, ABX, 5}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x9C */
	{_LDY, IMM, 2}, {_LDA, INX, 6}, {_LDX, IMM, 2}, {NULL, NUL, 0}, /* 0xA0 */
	{_LDY, ZER, 3}, {_LDA, ZER, 3}, {_LDX, ZER, 3}, {NULL, NUL, 0}, /* 0xA4 */
	{_TAY, IMP, 2}, {_LDA, IMM, 2}, {_TAX, IMP, 2}, {NULL, NUL, 0}, /* 0xA8 */
	{_LDY, ABS, 4}, {_LDA, ABS, 4}, {_LDX, ABS, 4}, {NULL, NUL, 0}, /* 0xAC */
	{_BCS, IMP, 2}, {_LDA, INY, 5}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0xB0 */
	{_LDY, ZEX, 4}, {_LDA, ZEX, 4}, {_LDX, ZEY, 4}, {NULL, NUL, 0}, /* 0xB4 */
	{_CLV, IMP, 2}, {_LDA, ABY, 4}, {_TSX, IMP, 2}, {NULL, NUL, 0}, /* 0xB8 */
	{_LDY, ABX, 4}, {_LDA, ABX, 4}, {_LDX, ABY, 4}, {NULL, NUL, 0}, /* 0xBC */
	{_CPY, IMM, 2}, {_CMP, INX, 6}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0xC0 */
	{_CPY, ZER, 3}, {_CMP, ZER, 3}, {_DEC, ZER, 5}, {NULL, NUL, 0}, /* 0xC4 */
	{_INY, IMP, 2}, {_CMP, IMM, 2}, {_DEX, IMP, 2}, {NULL, NUL, 0}, /* 0xC8 */
	{_CPY, ABS, 4}, {_CMP, ABS, 4}, {_DEC, ABS, 6}, {NULL, NUL, 0}, /* 0xCC */
	{_BNE, IMP, 2}, {_CMP, INY, 5}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0xD0 */
	{NULL, NUL, 0}, {_CMP, ZEX, 4}, {_DEC, ZEX, 6}, {NULL, NUL, 0}, /* 0xD4 */
	{_CLD, IMP, 2}, {_CMP, ABY, 4}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0xD8 */
	{NULL, NUL, 0}, {_CMP, ABX, 4}, {_DEC, ABX, 7}, {NULL, NUL, 0}, /* 0xDC */
	{_CPX, IMM, 2}, {_SBC, INX, 6}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0xE0 */
	{_CPX, ZER, 3}, {_SBC, ZER, 3}, {_INC, ZER, 5}, {NULL, NUL, 0}, /* 0xE4 */
	{_INX, IMP, 2}, {_SBC, IMM, 2}, {_NOP, IMP, 2}, {NULL, NUL, 0}, /* 0xE8 */
	{_CPX, ABS, 4}, {_SBC, ABS, 4}, {_INC, ABS, 6}, {NULL, NUL, 0}, /* 0xEC */
	{_BEQ, IMP, 2}, {_SBC, INY, 5}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0xF0 */
	{NULL, NUL, 0}, {_SBC, ZEX, 4}, {_INC, ZEX, 6}, {NULL, NUL, 0}, /* 0xF4 */
	{_SED, IMP, 2}, {_SBC, ABY, 4}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0xF8 */
	{NULL, NUL, 0}, {_SBC, ABX, 4}, {_INC, ABX, 7}, {NULL, NUL, 0}	/* 0xFC */
};

Opcode Opcode_Get(uint8_t index) {
	return opcode[index];
}

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

uint8_t _BRANCH(CPU* cpu, Instruction *arg, uint8_t cond) {
	uint16_t newPC;
	if (cond) {
		newPC = _REL_ADDR(cpu, (int8_t*) arg->dataMem);
		/* Check if the branch occurs to same page */
		arg->opcode.cycle += ((cpu->PC & 0xFF00) != (newPC & 0xFF00) ? 2 : 1);
		cpu->PC = newPC;
    }
	return arg->opcode.cycle;
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
	Mapper *mapper = cpu->rmap;
	switch (self->opcode.addressingMode) {
		case IMP : /* Implied : Nothing to do*/
			return 1;
			break;

		case ACC :
			self->dataMem = &cpu->A;
			break;

		case ZEX :
			address = (self->opcodeArg[0] + cpu->X) & 0xFF;
			self->dataMem = mapper->get(mapper->memoryMap, AS_CPU, address);
			break;

		case ZEY :
			address = (self->opcodeArg[0] + cpu->Y) & 0xFF;
			self->dataMem = mapper->get(mapper->memoryMap, AS_CPU, address);
			break;

		case INX :
			address = (self->opcodeArg[0] + cpu->X) & 0xFF;
			lWeight = *(mapper->get(mapper->memoryMap, AS_CPU, address));
			hWeight = *(mapper->get(mapper->memoryMap, AS_CPU, address + 1));
			address = (hWeight << 8) + lWeight;
			self->dataMem = mapper->get(mapper->memoryMap, AS_CPU, address);
			break;

		case INY :
			address = self->opcodeArg[0] & 0xFF;
			lWeight = *(mapper->get(mapper->memoryMap, AS_CPU, address));
			hWeight = *(mapper->get(mapper->memoryMap, AS_CPU, address + 1));
			address = (hWeight << 8) + lWeight;
			if ((address & 0xFF00) != ((address + cpu->Y) & 0xFF00))
				self->pageCrossed = 1;
			address = (hWeight <<8 ) + lWeight + cpu->Y;
			self->dataMem = mapper->get(mapper->memoryMap, AS_CPU, address);
			break;

		case IMM : /* Immediate : Nothing to do*/
			self->dataMem = self->opcodeArg;
			break;

		case ZER :
			address = self->opcodeArg[0] & 0xFF;
			self->dataMem = mapper->get(mapper->memoryMap, AS_CPU, address);
			break;

		case REL : /* Immediate : Nothing to do*/
			self->dataMem = self->opcodeArg;
			break;

		case ABS :
			address = (self->opcodeArg[1] << 8) + self->opcodeArg[0];
			self->dataMem = mapper->get(mapper->memoryMap, AS_CPU, address);
			break;

		case ABX :
			address = (self->opcodeArg[1] << 8) + self->opcodeArg[0];
			if ((address & 0xFF00) != ((address + cpu->X) & 0xFF00))
				self->pageCrossed = 1;
			address = (self->opcodeArg[1] << 8) + self->opcodeArg[0] + cpu->X;
			self->dataMem = mapper->get(mapper->memoryMap, AS_CPU, address);
			break;

		case ABY :
			address = (self->opcodeArg[1] << 8) + self->opcodeArg[0];
			if ((address & 0xFF00) != ((address + cpu->X) & 0xFF00))
				self->pageCrossed = 1;
			address = (self->opcodeArg[1] << 8) + self->opcodeArg[0] + cpu->Y;
			self->dataMem = mapper->get(mapper->memoryMap, AS_CPU, address);
			break;

		case ABI :
			address = (self->opcodeArg[1] << 8) + self->opcodeArg[0];
			lWeight = *(mapper->get(mapper->memoryMap, AS_CPU, address));
			hWeight = *(mapper->get(mapper->memoryMap, AS_CPU, address + 1));
			address = (hWeight << 8) + lWeight;
			self->dataMem = mapper->get(mapper->memoryMap, AS_CPU, address);
			break;

		default :
			return 0;
			break;
	}
	return 1;
}

uint8_t _ADC(CPU *cpu, Instruction *arg) {
	/* If page crossed in ABX, ABY and INY, add +1 to cycle */
	arg->opcode.cycle += arg->pageCrossed;
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
	return arg->opcode.cycle;
}

uint8_t _AND(CPU *cpu, Instruction *arg){return 0;}

uint8_t _ASL(CPU *cpu, Instruction *arg) {
	/* Execute */
	_SET_CARRY(cpu, *arg->dataMem & 0x80);
	*arg->dataMem <<= 1;
	_SET_SIGN(cpu, arg->dataMem);
	_SET_ZERO(cpu, arg->dataMem);
	/* Manage CPU cycle */
	return arg->opcode.cycle;
}

uint8_t _BCC(CPU *cpu, Instruction *arg) {
	return _BRANCH(cpu, arg, !_IF_CARRY(cpu));
}

uint8_t _BCS(CPU *cpu, Instruction *arg) {
	return _BRANCH(cpu, arg, _IF_CARRY(cpu));
}

uint8_t _BEQ(CPU *cpu, Instruction *arg) {
	return _BRANCH(cpu, arg, _IF_ZERO(cpu));
}

uint8_t _BIT(CPU *cpu, Instruction *arg) {
	uint8_t temp = *arg->dataMem & cpu->A;
	_SET_SIGN(cpu, arg->dataMem);
	_SET_OVERFLOW(cpu, 0x40 & *arg->dataMem);
	_SET_ZERO(cpu, &temp);
	return arg->opcode.cycle;
}

uint8_t _BMI(CPU *cpu, Instruction *arg) {
	return _BRANCH(cpu, arg, _IF_SIGN(cpu));
}

uint8_t _BNE(CPU *cpu, Instruction *arg) {
	return _BRANCH(cpu, arg, !_IF_ZERO(cpu));
}

uint8_t _BPL(CPU *cpu, Instruction *arg) {
	return _BRANCH(cpu, arg, !_IF_SIGN(cpu));
}

uint8_t _BRK(CPU *cpu, Instruction *arg) {
	cpu->PC++; /* Increment PC */
	/* Push return address to stack */
	uint8_t temp = (cpu->PC >> 8) & 0xFF;
	_PUSH(cpu, &temp);
	temp = cpu->PC & 0xFF;
	_PUSH(cpu, &temp);
	_SET_BREAK(cpu);
	_PUSH(cpu, &cpu->P);
	_SET_INTERRUPT(cpu);
	/* Set program counter from memory */
	cpu->PC = _LOAD(cpu, 0xFFFE) | (_LOAD(cpu, 0xFFFF) << 8);
	return arg->opcode.cycle;
}

uint8_t _BVC(CPU *cpu, Instruction *arg) {
	return _BRANCH(cpu, arg, !_IF_OVERFLOW(cpu));
}

uint8_t _BVS(CPU *cpu, Instruction *arg) {
	return _BRANCH(cpu, arg, _IF_OVERFLOW(cpu));
}

uint8_t _CLC(CPU *cpu, Instruction *arg){
	/*Carry flag -> 0 */
	cpu->P=(cpu->P)&(0xFE);
	return arg->opcode.cycle;
}

uint8_t _CLD(CPU *cpu, Instruction *arg){
	/*Decimal flag -> 0 */
	cpu->P=(cpu->P)&(0xF7);
	return arg->opcode.cycle;
}

uint8_t _CLI(CPU *cpu, Instruction *arg){
	/*_interrupt flag -> 0 */
	cpu->P=(cpu->P)&(0xFB);
	return arg->opcode.cycle;
}

uint8_t _CLV(CPU *cpu, Instruction *arg){
	/* Overflow flag -> 0 */
	cpu->P=(cpu->P)&(0xBF);
	return arg->opcode.cycle;
}

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

uint8_t _JMP(CPU *cpu, Instruction *arg) {
	/* Set program counter from memory */
	uint16_t newPC = (*(arg->dataMem+1) << 8) + *arg->dataMem;
	cpu->PC = newPC;
	return arg->opcode.cycle;
}

uint8_t _JSR(CPU *cpu, Instruction *arg) {
	cpu->PC--; /* Decrement PC */
	/* Push return address to stack */
	uint8_t temp = (cpu->PC >> 8) & 0xFF;
	_PUSH(cpu, &temp);
	temp = cpu->PC & 0xFF;
	_PUSH(cpu, &temp);
	/* Set program counter from memory */
	uint16_t newPC = (*(arg->dataMem+1) << 8) + *arg->dataMem;
	cpu->PC = newPC;
	return arg->opcode.cycle;
}

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
