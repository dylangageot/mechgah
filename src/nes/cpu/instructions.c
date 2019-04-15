/**
 * \file instructions.c
 * \author Hily Nicolas
 * \version 1.0
 * \date 2019-03-05
 */

#include "instructions.h"
#include <stdlib.h>
#include <stdio.h>
#include "../../common/macro.h"
#include "../mapper/ioreg.h"

/* Opcode LUT */
static Opcode opcode[256] = {
	{_BRK, IMP, 7}, {_ORA, INX, 6}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x00 */
	{NULL, NUL, 0}, {_ORA, ZER, 3}, {_ASL, ZER, 5}, {NULL, NUL, 0}, /* 0x04 */
	{_PHP, IMP, 3}, {_ORA, IMM, 2}, {_ASL, ACC, 2}, {NULL, NUL, 0}, /* 0x08 */
	{NULL, NUL, 0}, {_ORA, ABS, 4}, {_ASL, ABS, 6}, {NULL, NUL, 0}, /* 0x0C */
	{_BPL, REL, 2}, {_ORA, INY, 5}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x10 */
	{NULL, NUL, 0}, {_ORA, ZEX, 4}, {_ASL, ZEX, 6}, {NULL, NUL, 0}, /* 0x14 */
	{_CLC, IMP, 2}, {_ORA, ABY, 4}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x18 */
	{NULL, NUL, 0}, {_ORA, ABX, 4}, {_ASL, ABX, 7}, {NULL, NUL, 0}, /* 0x1C */
	{_JSR, ABS, 6}, {_AND, INX, 6}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x20 */
	{_BIT, ZER, 3}, {_AND, ZER, 3}, {_ROL, ZER, 5}, {NULL, NUL, 0}, /* 0x24 */
	{_PLP, IMP, 4}, {_AND, IMM, 2}, {_ROL, ACC, 2}, {NULL, NUL, 0}, /* 0x28 */
	{_BIT, ABS, 4}, {_AND, ABS, 4}, {_ROL, ABS, 6}, {NULL, NUL, 0}, /* 0x2C */
	{_BMI, REL, 2}, {_AND, INY, 5}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x30 */
	{NULL, NUL, 0}, {_AND, ZEX, 4}, {_ROL, ZEX, 6}, {NULL, NUL, 0}, /* 0x34 */
	{_SEC, IMP, 2}, {_AND, ABY, 4}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x38 */
	{NULL, NUL, 0}, {_AND, ABX, 4}, {_ROL, ABX, 7}, {NULL, NUL, 0}, /* 0x3C */
	{_RTI, IMP, 6}, {_EOR, INX, 6}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x40 */
	{NULL, NUL, 0}, {_EOR, ZER, 3}, {_LSR, ZER, 5}, {NULL, NUL, 0}, /* 0x44 */
	{_PHA, IMP, 3}, {_EOR, IMM, 2}, {_LSR, ACC, 2}, {NULL, NUL, 0}, /* 0x48 */
	{_JMP, ABS, 3}, {_EOR, ABS, 4}, {_LSR, ABS, 6}, {NULL, NUL, 0}, /* 0x4C */
	{_BVC, REL, 2}, {_EOR, INY, 5}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x50 */
	{NULL, NUL, 0}, {_EOR, ZEX, 4}, {_LSR, ZEX, 6}, {NULL, NUL, 0}, /* 0x54 */
	{_CLI, IMP, 2}, {_EOR, ABY, 4}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x58 */
	{NULL, NUL, 0}, {_EOR, ABX, 4}, {_LSR, ABX, 7}, {NULL, NUL, 0}, /* 0x5C */
	{_RTS, IMP, 6}, {_ADC, INX, 6}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x60 */
	{NULL, NUL, 0}, {_ADC, ZER, 3}, {_ROR, ZER, 5}, {NULL, NUL, 0}, /* 0x64 */
	{_PLA, IMP, 4}, {_ADC, IMM, 2}, {_ROR, ACC, 2}, {NULL, NUL, 0}, /* 0x68 */
	{_JMP, ABI, 5}, {_ADC, ABS, 4}, {_ROR, ABS, 6}, {NULL, NUL, 0}, /* 0x6C */
	{_BVS, REL, 2}, {_ADC, INY, 5}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x70 */
	{NULL, NUL, 0}, {_ADC, ZEX, 4}, {_ROR, ZEX, 6}, {NULL, NUL, 0}, /* 0x74 */
	{_SEI, IMP, 2}, {_ADC, ABY, 4}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x78 */
	{NULL, NUL, 0}, {_ADC, ABX, 4}, {_ROR, ABX, 7}, {NULL, NUL, 0}, /* 0x7C */
	{NULL, NUL, 0}, {_STA, INX, 6}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x80 */
	{_STY, ZER, 3}, {_STA, ZER, 3}, {_STX, ZER, 3}, {NULL, NUL, 0}, /* 0x84 */
	{_DEY, IMP, 2}, {NULL, NUL, 0}, {_TXA, IMP, 2}, {NULL, NUL, 0}, /* 0x88 */
	{_STY, ABS, 4}, {_STA, ABS, 4}, {_STX, ABS, 4}, {NULL, NUL, 0}, /* 0x8C */
	{_BCC, REL, 2}, {_STA, INY, 6}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x90 */
	{_STY, ZEX, 4}, {_STA, ZEX, 4}, {_STX, ZEY, 4}, {NULL, NUL, 0}, /* 0x94 */
	{_TYA, IMP, 2}, {_STA, ABY, 5}, {_TXS, IMP, 2}, {NULL, NUL, 0}, /* 0x98 */
	{NULL, NUL, 0}, {_STA, ABX, 5}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0x9C */
	{_LDY, IMM, 2}, {_LDA, INX, 6}, {_LDX, IMM, 2}, {NULL, NUL, 0}, /* 0xA0 */
	{_LDY, ZER, 3}, {_LDA, ZER, 3}, {_LDX, ZER, 3}, {NULL, NUL, 0}, /* 0xA4 */
	{_TAY, IMP, 2}, {_LDA, IMM, 2}, {_TAX, IMP, 2}, {NULL, NUL, 0}, /* 0xA8 */
	{_LDY, ABS, 4}, {_LDA, ABS, 4}, {_LDX, ABS, 4}, {NULL, NUL, 0}, /* 0xAC */
	{_BCS, REL, 2}, {_LDA, INY, 5}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0xB0 */
	{_LDY, ZEX, 4}, {_LDA, ZEX, 4}, {_LDX, ZEY, 4}, {NULL, NUL, 0}, /* 0xB4 */
	{_CLV, IMP, 2}, {_LDA, ABY, 4}, {_TSX, IMP, 2}, {NULL, NUL, 0}, /* 0xB8 */
	{_LDY, ABX, 4}, {_LDA, ABX, 4}, {_LDX, ABY, 4}, {NULL, NUL, 0}, /* 0xBC */
	{_CPY, IMM, 2}, {_CMP, INX, 6}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0xC0 */
	{_CPY, ZER, 3}, {_CMP, ZER, 3}, {_DEC, ZER, 5}, {NULL, NUL, 0}, /* 0xC4 */
	{_INY, IMP, 2}, {_CMP, IMM, 2}, {_DEX, IMP, 2}, {NULL, NUL, 0}, /* 0xC8 */
	{_CPY, ABS, 4}, {_CMP, ABS, 4}, {_DEC, ABS, 6}, {NULL, NUL, 0}, /* 0xCC */
	{_BNE, REL, 2}, {_CMP, INY, 5}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0xD0 */
	{NULL, NUL, 0}, {_CMP, ZEX, 4}, {_DEC, ZEX, 6}, {NULL, NUL, 0}, /* 0xD4 */
	{_CLD, IMP, 2}, {_CMP, ABY, 4}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0xD8 */
	{NULL, NUL, 0}, {_CMP, ABX, 4}, {_DEC, ABX, 7}, {NULL, NUL, 0}, /* 0xDC */
	{_CPX, IMM, 2}, {_SBC, INX, 6}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0xE0 */
	{_CPX, ZER, 3}, {_SBC, ZER, 3}, {_INC, ZER, 5}, {NULL, NUL, 0}, /* 0xE4 */
	{_INX, IMP, 2}, {_SBC, IMM, 2}, {_NOP, IMP, 2}, {NULL, NUL, 0}, /* 0xE8 */
	{_CPX, ABS, 4}, {_SBC, ABS, 4}, {_INC, ABS, 6}, {NULL, NUL, 0}, /* 0xEC */
	{_BEQ, REL, 2}, {_SBC, INY, 5}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0xF0 */
	{NULL, NUL, 0}, {_SBC, ZEX, 4}, {_INC, ZEX, 6}, {NULL, NUL, 0}, /* 0xF4 */
	{_SED, IMP, 2}, {_SBC, ABY, 4}, {NULL, NUL, 0}, {NULL, NUL, 0}, /* 0xF8 */
	{NULL, NUL, 0}, {_SBC, ABX, 4}, {_INC, ABX, 7}, {NULL, NUL, 0}	/* 0xFC */
};

Opcode Opcode_Get(uint8_t index) {
	return opcode[index];
}

/* Macros used by instructions */
#define SET_SIGN(x)		cpu->P = (*x & 0x80) ? cpu->P | 0x80 : cpu->P & (~0x80)
void _SET_SIGN(CPU *cpu, uint8_t *src) {
	if (*src & 0x80)
		cpu->P |= 0x80;
	else
		cpu->P &= ~0x80;
}

#define SET_ZERO(x)		cpu->P = (*x == 0) ? cpu->P | 0x02 : cpu->P & (~0x02)
void _SET_ZERO(CPU *cpu, uint8_t *src) {
	if (*src == 0)
		cpu->P |= 0x02;
	else
		cpu->P &= ~0x02;
}

#define SET_CARRY(x)	cpu->P = (x) ? cpu->P | 0x01 : cpu->P & (~0x01)   
void _SET_CARRY(CPU *cpu, uint8_t cond) {
	if (cond)
		cpu->P |= 0x01;
	else
		cpu->P &= ~0x01;
}

#define SET_OVERFLOW(x)	cpu->P = (x) ? cpu->P | 0x40 : cpu->P & (~0x40)  
void _SET_OVERFLOW(CPU *cpu, uint8_t cond) {
	if (cond)
		cpu->P |= 0x40;
	else
		cpu->P &= ~0x40;
}

#define SET_INTERRUPT(x)	cpu->P |= 0x04   
void _SET_INTERRUPT(CPU *cpu) {
	cpu->P |= 0x04;
}

#define SET_BREAK(x)		cpu->P |= 0x10   
void _SET_BREAK(CPU *cpu) {
	cpu->P |= 0x10;
}

#define REL_ADDR(x) (cpu->PC + (int16_t) *x)
uint16_t _REL_ADDR(CPU *cpu, int8_t *src) {
	return cpu->PC + (int16_t) *src;
}

#define SET_SR(x)	cpu->P = *x
void _SET_SR(CPU *cpu, uint8_t *src) {
	cpu->P = *src;
}

#define GET_SR()	(cpu->P)
uint8_t _GET_SR(CPU *cpu) {
	return cpu->P;
}

#define PULL()		(*Mapper_Get(cpu->mapper, AS_CPU, 0x0100 | (++cpu->SP)))
uint8_t _PULL(CPU *cpu) {
	return *Mapper_Get(cpu->mapper, AS_CPU, 0x0100 | (++cpu->SP));
}

#define	PUSH(x)		*(Mapper_Get(cpu->mapper, AS_CPU, 0x0100 | (cpu->SP--))) = *x
void _PUSH(CPU *cpu, uint8_t *src) {
	*(Mapper_Get(cpu->mapper, AS_CPU, 0x0100 | (cpu->SP--))) = *src;
}

#define LOAD(x)		Mapper_Get(cpu->mapper, AC_RD | AS_CPU, x)
uint8_t* _LOAD(CPU *cpu, uint16_t address) {
	return Mapper_Get(cpu->mapper, AC_RD | AS_CPU, address);
}

#define STORE(x,y)	*(Mapper_Get(cpu->mapper, AC_WR | AS_CPU, x)) = *y
void _STORE(CPU *cpu, uint16_t address, uint8_t *src) {
	*(Mapper_Get(cpu->mapper, AC_WR | AS_CPU, address)) = *src;
}

#define SET_WR(x)	Mapper_Get(cpu->mapper, AC_WR | AS_CPU, x)
void _SET_WR(CPU *cpu, uint16_t address) {
	Mapper_Get(cpu->mapper, AC_WR | AS_CPU, address);
}

#define IF_CARRY()	((cpu->P & 0x01) == 0x01)
uint8_t _IF_CARRY(CPU *cpu) {
	return (cpu->P & 0x01) == 0x01;
}

#define IF_OVERFLOW()	((cpu->P & 0x40) == 0x40)
uint8_t _IF_OVERFLOW(CPU *cpu) {
	return (cpu->P & 0x40) == 0x40;
}

#define IF_SIGN()	((cpu->P & 0x80) == 0x80)
uint8_t _IF_SIGN(CPU *cpu) {
	return (cpu->P & 0x80) == 0x80;
}

#define IF_ZERO()	((cpu->P & 0x02) == 0x02)
uint8_t _IF_ZERO(CPU *cpu) {
	return (cpu->P & 0x02) == 0x02;
}

#define IF_INTERRUPT()	((cpu->P & 0x04) == 0x04)
uint8_t _IF_INTERRUPT(CPU *cpu) {
	return (cpu->P & 0x04) == 0x04;
}

#define IF_BREAK()	((cpu->P & 0x10) == 0x10)
uint8_t _IF_BREAK(CPU *cpu) {
	return (cpu->P & 0x10) == 0x10;
}

uint8_t _BRANCH(CPU* cpu, Instruction *arg, uint8_t cond) {
	uint16_t newPC;
	if (cond) {
		/* Compute address to jump on */
		newPC = REL_ADDR((int8_t*) arg->dataMem);
		/* Check if the branch occurs to same page */
		arg->opcode.cycle += ((cpu->PC & 0xFF00) != (newPC & 0xFF00) ? 2 : 1);
		cpu->PC = newPC;
	}
	return arg->opcode.cycle;
}

/* Instructions management */

uint8_t Instruction_DMA(Instruction *self, CPU *cpu, uint32_t *clockCycle) {
	uint8_t data;

	/* Start DMA operation if needed */
	if (Mapper_Ack(cpu->mapper, 0x4014)) {
		cpu->cntDMA = 0;
		/* Add 1 (+1 if odd) to clock cycle 1 */
		*clockCycle += 1 + (*clockCycle % 2);
	}

	/* DMA on-going */
	if ((cpu->cntDMA > -1) && (cpu->cntDMA < 256)) {
		/* Setup Instruction structure with NOP operation (2 cycle) */
		self->dataAddr = (cpu->OAMDMA << 8) + (cpu->cntDMA & 0xFF);
		self->nbArg = NBARG_DMA;
		self->lastPC = cpu->PC;
		self->opcode.cycle = 2;
		self->pageCrossed = 0;
		self->opcode.inst = _NOP;
		/* Load and store */
		data = *(LOAD(self->dataAddr));
		STORE(0x2004, &data); 
		/* Increment DMA counter */
		cpu->cntDMA++;
		return DMA_ON;
	} else if (cpu->cntDMA == 256) {
		cpu->cntDMA = -1;
	}

	/* If no DMA operation on-going, return 0 */
	return DMA_OFF;
}

uint8_t Instruction_Fetch(Instruction *self, CPU *cpu) {
	/* Fetch data in memory space with PC value */
	uint8_t *opc = LOAD(cpu->PC);
	/* Save information before fetching */
	self->rawOpcode = *opc;
	self->lastPC = cpu->PC++;
	self->opcode = opcode[*(opc++)];

	/* Decode and update PC */
	if (self->opcode.addressingMode <= 1) {
		self->nbArg = 0;
	} else if ((self->opcode.addressingMode >= 2) &&
			(self->opcode.addressingMode <= 8)) {
		self->opcodeArg[0] = *opc;
		cpu->PC++;
		self->nbArg = 1;
	} else if ((self->opcode.addressingMode >= 9) &&
			(self->opcode.addressingMode <= 12)) {
		uint8_t i;
		for (i = 0; i < 2; i++)
			self->opcodeArg[i] = *(opc + i);
		cpu->PC += 2;
		self->nbArg = 2;
	} else {
		self->nbArg = 0;
		ERROR_MSG("can't fetch the next instruction");	
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

uint8_t Instruction_Resolve(Instruction *self, CPU *cpu) {
	uint8_t lWeight, hWeight;
	uint16_t address = 0;
	
	/* Init pageCrossed at 0 */
	self->pageCrossed = 0;

	switch (self->opcode.addressingMode) {
		case IMP :
			/* Nothing to do */
			break;

		case ACC :
			self->dataMem = &cpu->A;
			break;

		case ZEX :
			address = (self->opcodeArg[0] + cpu->X) & 0xFF;
			self->dataMem = LOAD(address);
			break;

		case ZEY :
			address = (self->opcodeArg[0] + cpu->Y) & 0xFF;
			self->dataMem = LOAD(address);
			break;

		case INX :
			address = (self->opcodeArg[0] + cpu->X) & 0xFF;
			lWeight = *(LOAD(address));
			hWeight = *(LOAD((address + 1) & 0xFF));
			address = (hWeight << 8) + lWeight;
			self->dataMem = LOAD(address);
			break;

		case INY :
			address = self->opcodeArg[0] & 0xFF;
			lWeight = *(LOAD(address));
			hWeight = *(LOAD((address + 1) & 0xFF));
			address = (hWeight << 8) + lWeight;
			if ((address & 0xFF00) != ((address + cpu->Y) & 0xFF00))
				self->pageCrossed = 1;
			address = (hWeight <<8 ) + lWeight + cpu->Y;
			self->dataMem = LOAD(address);
			break;

		case IMM :
			self->dataMem = self->opcodeArg;
			break;

		case ZER :
			address = self->opcodeArg[0] & 0xFF;
			self->dataMem = LOAD(address);
			break;

		case REL :
			self->dataMem = self->opcodeArg;
			break;

		case ABS :
			address = (self->opcodeArg[1] << 8) + self->opcodeArg[0];
			self->dataMem = LOAD(address);
			break;

		case ABX :
			address = (self->opcodeArg[1] << 8) + self->opcodeArg[0];
			if ((address & 0xFF00) != ((address + cpu->X) & 0xFF00))
				self->pageCrossed = 1;
			address = (self->opcodeArg[1] << 8) + self->opcodeArg[0] + cpu->X;
			self->dataMem = LOAD(address);
			break;

		case ABY :
			address = (self->opcodeArg[1] << 8) + self->opcodeArg[0];
			if ((address & 0xFF00) != ((address + cpu->Y) & 0xFF00))
				self->pageCrossed = 1;
			address = (self->opcodeArg[1] << 8) + self->opcodeArg[0] + cpu->Y;
			self->dataMem = LOAD(address);
			break;

		case ABI :
			address = (self->opcodeArg[1] << 8) + self->opcodeArg[0];
			lWeight = *(LOAD(address));
			hWeight = *(LOAD((address & 0xFF00) | ((address + 1) & 0xFF)));
			address = (hWeight << 8) + lWeight;
			self->dataMem = LOAD(address);
			break;

		default :
			ERROR_MSG("can't resolve addressing mode of the next instruction");	
			return EXIT_FAILURE;
	}
	self->dataAddr = address;
	return EXIT_SUCCESS;
}

void Instruction_PrintLog(Instruction *self, CPU *cpu, uint32_t clockCycle) {
	FILE* fLog = NULL;
	int i;
	/* Open log file for append line into */
	fLog = fopen("cpu.log", "a+");
	if (fLog == NULL) {
		ERROR_MSG("can't access cpu.log");
		return;
	}

	/* Print to cpu.log */
	fprintf(fLog, "%04X ", self->lastPC);
	if (self->nbArg == NBARG_DMA) {
		/* If instruction is a DMA */
		fprintf(fLog, "DMA @%04X   ", self->dataAddr);
	} else {
		/* If instruction is not a DMA */
		/* Print opcode and args */
		fprintf(fLog, "%02X ", self->rawOpcode);
		for (i = 0; i < 3; i++) {
			if (i < self->nbArg)
				fprintf(fLog, "%02X ", self->opcodeArg[i]);
			else
				fprintf(fLog, "   ");
		}
	}
	fprintf(fLog, "A:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%-d\n",
			cpu->A, cpu->X, cpu->Y, cpu->P,cpu->SP, clockCycle);

	/* Close file */
	fclose(fLog);
}

/* Instructions */

uint8_t _ADC(CPU *cpu, Instruction *arg) {
	/* If page crossed in ABX, ABY and INY, add +1 to cycle */
	arg->opcode.cycle += arg->pageCrossed;
	uint16_t temp = *arg->dataMem + cpu->A + (IF_CARRY() ? 1 : 0);
	/* Set bit flag */
	SET_ZERO((uint8_t*) &temp);
	SET_SIGN((uint8_t*) &temp);
	SET_OVERFLOW(!((cpu->A ^ *arg->dataMem) & 0x80)&&((cpu->A ^ temp) & 0x80));
	SET_CARRY(temp > 0xFF);
	/* Save in Accumulator */
	cpu->A = ((uint8_t) temp & 0xFF);
	/* Manage CPU cycle */
	return arg->opcode.cycle;
}

uint8_t _AND(CPU *cpu, Instruction *arg){
	/* "AND" memory with accumulator*/
	arg->opcode.cycle += arg->pageCrossed;
	uint8_t m = *(arg->dataMem);
	m = (cpu->A)&m;
	SET_SIGN(&m);
	SET_ZERO(&m);
	cpu->A = m;
	return arg->opcode.cycle;
}

uint8_t _ASL(CPU *cpu, Instruction *arg) {
	/* Execute */
	SET_CARRY(*arg->dataMem & 0x80);
	*arg->dataMem <<= 1;
	SET_WR(arg->dataAddr);
	SET_SIGN(arg->dataMem);
	SET_ZERO(arg->dataMem);
	/* Manage CPU cycle */
	return arg->opcode.cycle;
}

uint8_t _BCC(CPU *cpu, Instruction *arg) {
	return _BRANCH(cpu, arg, !IF_CARRY());
}

uint8_t _BCS(CPU *cpu, Instruction *arg) {
	return _BRANCH(cpu, arg, IF_CARRY());
}

uint8_t _BEQ(CPU *cpu, Instruction *arg) {
	return _BRANCH(cpu, arg, IF_ZERO());
}

uint8_t _BIT(CPU *cpu, Instruction *arg) {
	uint8_t temp = *arg->dataMem & cpu->A;
	SET_SIGN(arg->dataMem);
	SET_OVERFLOW(0x40 & *arg->dataMem);
	SET_ZERO(&temp);
	return arg->opcode.cycle;
}

uint8_t _BMI(CPU *cpu, Instruction *arg) {
	return _BRANCH(cpu, arg, IF_SIGN());
}

uint8_t _BNE(CPU *cpu, Instruction *arg) {
	return _BRANCH(cpu, arg, !IF_ZERO());
}

uint8_t _BPL(CPU *cpu, Instruction *arg) {
	return _BRANCH(cpu, arg, !IF_SIGN());
}

uint8_t _BRK(CPU *cpu, Instruction *arg) {
	cpu->PC++; /* Increment PC */
	/* Push return address to stack */
	uint8_t temp = (cpu->PC >> 8) & 0xFF;
	PUSH(&temp);
	temp = cpu->PC & 0xFF;
	PUSH(&temp);
	temp = cpu->P | 0x10;
	PUSH(&temp);
	SET_INTERRUPT();
	/* Set program counter from memory */
	cpu->PC = *(LOAD(0xFFFE)) | (*(LOAD(0xFFFF)) << 8);
	return arg->opcode.cycle;
}

uint8_t _BVC(CPU *cpu, Instruction *arg) {
	return _BRANCH(cpu, arg, !IF_OVERFLOW());
}

uint8_t _BVS(CPU *cpu, Instruction *arg) {
	return _BRANCH(cpu, arg, IF_OVERFLOW());
}

uint8_t _CLC(CPU *cpu, Instruction *arg){
	/*Carry flag -> 0 */
	cpu->P&=(0xFE);
	return arg->opcode.cycle;
}

uint8_t _CLD(CPU *cpu, Instruction *arg){
	/*Decimal flag -> 0 */
	cpu->P&=(0xF7);
	return arg->opcode.cycle;
}

uint8_t _CLI(CPU *cpu, Instruction *arg){
	/*_interrupt flag -> 0 */
	cpu->P&=(0xFB);
	return arg->opcode.cycle;
}

uint8_t _CLV(CPU *cpu, Instruction *arg){
	/* Overflow flag -> 0 */
	cpu->P&=(0xBF);
	return arg->opcode.cycle;
}

uint8_t _CMP(CPU *cpu, Instruction *arg){
	/* CMP Compare memory and accumulator */
	uint16_t temp = (uint16_t)*(arg->dataMem);
	arg->opcode.cycle += arg->pageCrossed;
	temp = cpu->A - temp;
	SET_CARRY(temp < 0x100);
	SET_SIGN((uint8_t*)&temp);
	SET_ZERO((uint8_t*)&temp);
	return arg->opcode.cycle;
}

uint8_t _CPX(CPU *cpu, Instruction *arg){
	/*CPX Compare Memory and Index X  */
	uint16_t temp = (uint16_t)*(arg->dataMem);
	temp = cpu->X - temp;
	SET_CARRY(temp < 0x100);
	SET_SIGN((uint8_t*)&temp);
	SET_ZERO((uint8_t*)&temp);
	return arg->opcode.cycle;
}

uint8_t _CPY(CPU *cpu, Instruction *arg){
	/*CPY Compare Memory and Index Y  */
	uint16_t temp = (uint16_t)*(arg->dataMem);
	temp = cpu->Y - temp;
	SET_CARRY(temp < 0x100);
	SET_SIGN((uint8_t*)&temp);
	SET_ZERO((uint8_t*)&temp);
	return arg->opcode.cycle;
}

uint8_t _DEC(CPU *cpu, Instruction *arg){
	/*DEC Decrement memory by one  */
	uint8_t m = *(arg->dataMem);
	m = (m -1)%256;
	SET_SIGN(&m);
	SET_ZERO(&m);
	*(arg->dataMem) = m;
	SET_WR(arg->dataAddr);
	return arg->opcode.cycle;
}

uint8_t _DEX(CPU *cpu, Instruction *arg){
	/*DEX Decrement index X by one */
	uint8_t m = cpu->X;
	m = (m -1)%256;
	SET_SIGN(&m);
	SET_ZERO(&m);
	cpu->X = m;
	return arg->opcode.cycle;
}

uint8_t _DEY(CPU *cpu, Instruction *arg){
	/*DEY Decrement index Y by one */
	uint8_t m = cpu->Y;
	m = (m -1)%256;
	SET_SIGN(&m);
	SET_ZERO(&m);
	cpu->Y = m;
	return arg->opcode.cycle;
}

uint8_t _EOR(CPU *cpu, Instruction *arg){
	/*EOR "Exclusive-Or" memory with accumulator */
	arg->opcode.cycle += arg->pageCrossed;
	uint8_t m = *(arg->dataMem);
	m = (cpu->A)^m;
	SET_SIGN(&m);
	SET_ZERO(&m);
	cpu->A = m;
	return arg->opcode.cycle;
}

uint8_t _INC(CPU *cpu, Instruction *arg){
	/* INC Increment memory by one */
	uint8_t m = *(arg->dataMem);
	m = (m + 1)%256;
	SET_SIGN(&m);
	SET_ZERO(&m);
	*(arg->dataMem) = m;
	SET_WR(arg->dataAddr);
	return arg->opcode.cycle;
}

uint8_t _INX(CPU *cpu, Instruction *arg){
	/*INX increment index X by one */
	uint8_t m = cpu->X;
	m = (m +1)%256;
	SET_SIGN(&m);
	SET_ZERO(&m);
	cpu->X = m;
	return arg->opcode.cycle;
}

uint8_t _INY(CPU *cpu, Instruction *arg){
	/*INY increment index Y by one */
	uint8_t m = cpu->Y;
	m = (m +1)%256;
	SET_SIGN(&m);
	SET_ZERO(&m);
	cpu->Y = m;
	return arg->opcode.cycle;
}

uint8_t _JMP(CPU *cpu, Instruction *arg) {
	/* Set program counter from memory */
	cpu->PC = arg->dataAddr;
	return arg->opcode.cycle;
}

uint8_t _JSR(CPU *cpu, Instruction *arg) {
	cpu->PC--; /* Decrement PC */
	/* Push return address to stack */
	uint8_t temp = (cpu->PC >> 8) & 0xFF;
	PUSH(&temp);
	temp = cpu->PC & 0xFF;
	PUSH(&temp);
	/* Set program counter from memory */
	cpu->PC = arg->dataAddr;
	return arg->opcode.cycle;
}

uint8_t _LDA(CPU *cpu, Instruction *arg){
	SET_SIGN(arg->dataMem);
	SET_ZERO(arg->dataMem);
	cpu->A = *(arg->dataMem);
	return arg->opcode.cycle + arg->pageCrossed;
}

uint8_t _LDX(CPU *cpu, Instruction *arg){
	SET_SIGN(arg->dataMem);
	SET_ZERO(arg->dataMem);
	cpu->X = *(arg->dataMem);
	return arg->opcode.cycle + arg->pageCrossed;
}

uint8_t _LDY(CPU *cpu, Instruction *arg){
	SET_SIGN(arg->dataMem);
	SET_ZERO(arg->dataMem);
	cpu->Y = *(arg->dataMem);
	return arg->opcode.cycle + arg->pageCrossed;
}

uint8_t _LSR(CPU *cpu, Instruction *arg){
	SET_CARRY((*(arg->dataMem) & 0x01));
	*arg->dataMem >>= 1;
	SET_WR(arg->dataAddr);
	SET_SIGN(arg->dataMem);
	SET_ZERO(arg->dataMem);
	return arg->opcode.cycle;
}

uint8_t _NOP(CPU *cpu, Instruction *arg){
	/* Nothing to do */
	return arg->opcode.cycle + arg->pageCrossed;
}

uint8_t _ORA(CPU *cpu, Instruction *arg){
	cpu->A = cpu->A | *arg->dataMem;
	SET_SIGN(&cpu->A);
	SET_ZERO(&cpu->A);
	return arg->opcode.cycle + arg->pageCrossed;
}

uint8_t _PHA(CPU *cpu, Instruction *arg) {
	uint8_t src = cpu->A;
	PUSH(&src);
	return arg->opcode.cycle;
}

uint8_t _PHP(CPU *cpu, Instruction *arg) {
	uint8_t src = GET_SR() | 0x30;
	PUSH(&src);
	return arg->opcode.cycle;
}

uint8_t _PLA(CPU *cpu, Instruction *arg) {
	uint8_t src = PULL();
	cpu->A = src;
	SET_SIGN(&src);
	SET_ZERO(&src);
	return arg->opcode.cycle;
}

uint8_t _PLP(CPU *cpu, Instruction *arg) {
	uint8_t src = (PULL() | 0x20) & ~0x10;
	SET_SR(&src);
	return arg->opcode.cycle;
}
uint8_t _ROL(CPU *cpu, Instruction *arg){
	uint8_t newCarry = *arg->dataMem & 0x80;
	*arg->dataMem <<= 1;
	if (IF_CARRY()){
		*arg->dataMem |= 0x1;
	}
	SET_WR(arg->dataAddr);
	SET_CARRY(newCarry == 0x80);
	SET_SIGN(arg->dataMem);
	SET_ZERO(arg->dataMem);
	return arg->opcode.cycle;
}

uint8_t _ROR(CPU *cpu, Instruction *arg){
	uint8_t newCarry = *arg->dataMem & 0x01;
	*arg->dataMem >>= 1;
	if (IF_CARRY()){
		*arg->dataMem |= 0x80;
	}
	SET_WR(arg->dataAddr);
	SET_CARRY(newCarry == 0x01);
	SET_SIGN(arg->dataMem);
	SET_ZERO(arg->dataMem);
	return arg->opcode.cycle;
}

uint8_t _RTI(CPU *cpu, Instruction *arg) {
	uint8_t temp = (PULL() | 0x20) & ~0x10;
	SET_SR(&temp);

	uint16_t pc = PULL();
	pc |= (PULL() << 8);

	cpu->PC = pc;
	return arg->opcode.cycle;
}
uint8_t _RTS(CPU *cpu, Instruction *arg) {
	uint16_t pc = PULL();
	pc |= ((PULL() << 8));
	cpu->PC = pc + 1;
	return arg->opcode.cycle;
}

uint8_t _SBC(CPU *cpu, Instruction *arg){
	uint16_t temp = cpu->A - *arg->dataMem - (IF_CARRY() ? 0 : 1);
	SET_SIGN((uint8_t*) &temp);
	SET_ZERO((uint8_t*) &temp);
	SET_OVERFLOW(((cpu->A ^ temp) & 0x80) && ((cpu->A ^ *arg->dataMem) & 0x80));
	SET_CARRY(temp < 0x100);
	cpu->A = (uint8_t)(temp & 0xff);
	return arg->opcode.cycle + arg->pageCrossed;
}

uint8_t _SEC(CPU *cpu, Instruction *arg){
	SET_CARRY(1);
	return arg->opcode.cycle;
}

uint8_t _SED(CPU *cpu, Instruction *arg){
	cpu->P |= (0x08);
	return arg->opcode.cycle;
}

uint8_t _SEI(CPU *cpu, Instruction *arg) {
	SET_INTERRUPT();
	return arg->opcode.cycle;
}

uint8_t _STA(CPU *cpu, Instruction *arg){
	*(arg->dataMem) = cpu->A;
	SET_WR(arg->dataAddr);
	return arg->opcode.cycle;
}

uint8_t _STX(CPU *cpu, Instruction *arg){
	*(arg->dataMem) = cpu->X;
	SET_WR(arg->dataAddr);
	return arg->opcode.cycle;
}

uint8_t _STY(CPU *cpu, Instruction *arg){
	*(arg->dataMem) = cpu->Y;
	SET_WR(arg->dataAddr);
	return arg->opcode.cycle;
}

uint8_t _TAX(CPU *cpu, Instruction *arg) {
	uint8_t src = cpu->A;
	SET_SIGN(&src);
	SET_ZERO(&src);
	cpu->X = src;
	return arg->opcode.cycle;
}

uint8_t _TAY(CPU *cpu, Instruction *arg) {
	uint8_t src = cpu->A;
	SET_SIGN(&src);
	SET_ZERO(&src);
	cpu->Y = src;
	return arg->opcode.cycle;
}

uint8_t _TSX(CPU *cpu, Instruction *arg) {
	uint8_t src = cpu->SP;
	SET_SIGN(&src);
	SET_ZERO(&src);
	cpu->X = src;
	return arg->opcode.cycle;
}

uint8_t _TXA(CPU *cpu, Instruction *arg) {
	uint8_t src = cpu->X;
	SET_SIGN(&src);
	SET_SIGN(&src);
	SET_ZERO(&src);
	cpu->A = src;
	return arg->opcode.cycle;
}

uint8_t _TXS(CPU *cpu, Instruction *arg) {
	cpu->SP = cpu->X;
	return arg->opcode.cycle;
}

uint8_t _TYA(CPU *cpu, Instruction *arg) {
	uint8_t src = cpu->Y;
	SET_SIGN(&src);
	SET_ZERO(&src);
	cpu->A = src;
	return arg->opcode.cycle;
}
