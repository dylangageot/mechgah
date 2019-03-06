/**
 * \file instructions.c
 * \author Hily Nicolas
 * \version 1.0
 * \date 2019-03-05
 */

#include <stdlib.h>
#include "instructions.h"
#include "cpu.h"

uint8_t AddressingMode_Execute(CPU *cpu, Instruction *inst){
	if ((cpu == NULL) || (inst == NULL))
		return 0;


	uint8_t lWeight, hWeight;
	uint16_t address = 0;
	inst->pageCrossed = 0;
	Mapper *mapper = mapper;
	switch (inst->opcode.addressingMode) {
		case IMP : /* Implied : Nothing to do*/
			break;

		case ACC : /*Accumulator : Nothing to do */
			break;

		case ZEX :
			address = (0x00FF) & (uint16_t)(inst->opcodeArg[0] + cpu->X);
			inst->dataMem = mapper->get(mapper->memoryMap, AS_CPU, address);
			break;

		case ZEY :
			address = (0x00FF) & (uint16_t)(inst->opcodeArg[0] + cpu->Y);
			inst->dataMem = mapper->get(mapper->memoryMap, AS_CPU, address);
			break;

		case INX :
			address = (0x00FF) & (uint16_t)(inst->opcodeArg[0] + cpu->X);
			lWeight = *(mapper->get(mapper->memoryMap, AS_CPU, address));
			hWeight = *(mapper->get(mapper->memoryMap, AS_CPU, address + 1));
			address = (uint16_t)(hWeight<<8 + lWeight);
			inst->dataMem = mapper->get(mapper->memoryMap, AS_CPU, address);
			break;

		case INY :
			address = (0x00FF) & (uint16_t)(inst->opcodeArg[0]);
			lWeight = *(mapper->get(mapper->memoryMap, AS_CPU, address));
			hWeight = *(mapper->get(mapper->memoryMap, AS_CPU, address + 1));
			address = (uint16_t)(hWeight<<8 + lWeight);
			if ((address & 0xFF00) != ((address + cpu->Y) & 0xFF00))
				inst->pageCrossed = 1;
			address = (uint16_t)(hWeight<<8 + lWeight + cpu->Y);
			inst->dataMem = mapper->get(mapper->memoryMap, AS_CPU, address);
			break;

		case IMM : /* Immediate : Nothing to do*/
			break;

		case ZER :
			address = (0x00FF) & (uint16_t)(inst->opcodeArg[0]);
			inst->dataMem = mapper->get(mapper->memoryMap, AS_CPU, address);
			break;

		case REL : /* Immediate : Nothing to do*/
			break;

		case ABS :
			address = (uint16_t)(inst->opcodeArg[1]<<8 + inst->opcodeArg[0]);
			inst->dataMem = mapper->get(mapper->memoryMap, AS_CPU, address);
			break;

		case ABX :
			address = (uint16_t)(inst->opcodeArg[1]<<8 + inst->opcodeArg[0]);
			if ((address & 0xFF00) != ((address + cpu->X) & 0xFF00))
				inst->pageCrossed = 1;
			address = (uint16_t)(inst->opcodeArg[1]<<8 + inst->opcodeArg[0] + cpu->X);
			inst->dataMem = mapper->get(mapper->memoryMap, AS_CPU, address);
			break;

		case ABY :
			address = (uint16_t)(inst->opcodeArg[1]<<8 + inst->opcodeArg[0]);
			if ((address & 0xFF00) != ((address + cpu->X) & 0xFF00))
				inst->pageCrossed = 1;
			address = (uint16_t)(inst->opcodeArg[1]<<8 + inst->opcodeArg[0] + cpu->Y);
			inst->dataMem = mapper->get(mapper->memoryMap, AS_CPU, address);
			break;

		case ABI :
			address = (uint16_t)(inst->opcodeArg[1]<<8 + inst->opcodeArg[0]);
			lWeight = *(mapper->get(mapper->memoryMap, AS_CPU, address));
			hWeight = *(mapper->get(mapper->memoryMap, AS_CPU, address + 1));
			address = (uint16_t)(hWeight<<8 + lWeight);
			inst->dataMem = mapper->get(mapper->memoryMap, AS_CPU, address);
			break;

		default :
			return 0;
			break;
	}
	return 1;
}

uint8_t _ADC(CPU *cpu, Instruction *arg){return 0;}
uint8_t _AND(CPU *cpu, Instruction *arg){return 0;}
uint8_t _ASL(CPU *cpu, Instruction *arg){return 0;}
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
