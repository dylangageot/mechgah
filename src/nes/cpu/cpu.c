#include "cpu.h"
#include "instructions.h"
#include <unistd.h>
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

uint8_t CPU_InstructionFetch(CPU *self, Instruction *inst) {
	if ((self == NULL) || (inst == NULL))
		return 0;

	/* Fetch data in memory space with PC value */
	uint8_t i;
	uint8_t *opc = self->rmap->get(self->rmap->mapperData, AS_CPU, self->PC);
	self->PC++;
	inst->opcode = opcode[*(opc++)];
	/* Decode and update PC */
	if((inst->opcode.addressingMode >= 2) && (inst->opcode.addressingMode <= 8)){
		inst->opcodeArg[0] = *opc;
		self->PC++;
	}
	else if((inst->opcode.addressingMode >= 9) && (inst->opcode.addressingMode <= 12)){
		for (i = 0; i < 2; i++)
			inst->opcodeArg[i] = *(opc + i);
		self->PC += 2;
	}
	else
		return 0;

	return 1;
}
