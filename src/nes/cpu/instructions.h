/**
 * \file instructions.h
 * \brief Header for instructions.h
 * \author Dylan Gageot
 * \version 1.0
 * \date 2019-02-20
 */

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "cpu.h"

/**
 * \struct Instruction
 * \brief Hold instruction function and addressing mode associated
 */
typedef struct {
	uint8_t (*inst)(CPU*, uint16_t);
	uint8_t addressingMode;
} Instruction;

/**
 * \enum AddressingMode
 * \brief Mnemonic for every addressing mode
 */
enum AddressingMode {
	ZER = 0,	/*! Zero page			*/
	ZEX,		/*! Indexed-X zero page	*/
	ZEY,		/*! Indexed-Y zero page	*/
	ABS,		/*! Absolute			*/
	ABX,		/*! Indexed-X absolute	*/
	ABY,		/*! Indexed-Y absolute	*/
	ABI,		/*! Absolute indirect	*/
	IMM,		/*! Immediate			*/
	ACC,		/*! Accumulator			*/
	IMP,		/*! Implied				*/
	INX,		/*! Indexed-X indirect	*/
	INY,		/*! Indexed-Y indirect	*/
	REL,		/*! Relative			*/
	NUL			/*! Undefined			*/
};

/**
 * \fn AddressingMode_Execute
 * \brief Get the data to execute the instruction
 *
 * \param cpu instance of cpu
 * \param addressingMode addressing mode used by the instruction
 *
 * \return data fetch from memory according from instruction's addressing mode 
 */
uint16_t AddressingMode_Execute(CPU* cpu, uint8_t addressingMode);

uint8_t _ADC(CPU* cpu, uint16_t arg);
uint8_t _AND(CPU* cpu, uint16_t arg);
uint8_t _ASL(CPU* cpu, uint16_t arg);
uint8_t _BCC(CPU* cpu, uint16_t arg);
uint8_t _BCS(CPU* cpu, uint16_t arg);
uint8_t _BEQ(CPU* cpu, uint16_t arg);
uint8_t _BIT(CPU* cpu, uint16_t arg);
uint8_t _BMI(CPU* cpu, uint16_t arg);
uint8_t _BNE(CPU* cpu, uint16_t arg);
uint8_t _BPL(CPU* cpu, uint16_t arg);
uint8_t _BRK(CPU* cpu, uint16_t arg);
uint8_t _BVC(CPU* cpu, uint16_t arg);
uint8_t _BVS(CPU* cpu, uint16_t arg);
uint8_t _CLC(CPU* cpu, uint16_t arg);
uint8_t _CLD(CPU* cpu, uint16_t arg);
uint8_t _CLI(CPU* cpu, uint16_t arg);
uint8_t _CLV(CPU* cpu, uint16_t arg);
uint8_t _CMP(CPU* cpu, uint16_t arg);
uint8_t _CMX(CPU* cpu, uint16_t arg);
uint8_t _CMY(CPU* cpu, uint16_t arg);
uint8_t _DEC(CPU* cpu, uint16_t arg);
uint8_t _DEX(CPU* cpu, uint16_t arg);
uint8_t _DEY(CPU* cpu, uint16_t arg);
uint8_t _EOR(CPU* cpu, uint16_t arg);
uint8_t _INC(CPU* cpu, uint16_t arg);
uint8_t _INX(CPU* cpu, uint16_t arg);
uint8_t _INY(CPU* cpu, uint16_t arg);
uint8_t _JMP(CPU* cpu, uint16_t arg);
uint8_t _JSR(CPU* cpu, uint16_t arg);
uint8_t _LDA(CPU* cpu, uint16_t arg);
uint8_t _LDX(CPU* cpu, uint16_t arg);
uint8_t _LDY(CPU* cpu, uint16_t arg);
uint8_t _LSR(CPU* cpu, uint16_t arg);
uint8_t _NOP(CPU* cpu, uint16_t arg);
uint8_t _ORA(CPU* cpu, uint16_t arg);
uint8_t _PHA(CPU* cpu, uint16_t arg);
uint8_t _PHP(CPU* cpu, uint16_t arg);
uint8_t _PLA(CPU* cpu, uint16_t arg);
uint8_t _ROL(CPU* cpu, uint16_t arg);
uint8_t _ROR(CPU* cpu, uint16_t arg);
uint8_t _RTI(CPU* cpu, uint16_t arg);
uint8_t _RTS(CPU* cpu, uint16_t arg);
uint8_t _SBC(CPU* cpu, uint16_t arg);
uint8_t _SEC(CPU* cpu, uint16_t arg);
uint8_t _SED(CPU* cpu, uint16_t arg);
uint8_t _SEI(CPU* cpu, uint16_t arg);
uint8_t _STA(CPU* cpu, uint16_t arg);
uint8_t _STX(CPU* cpu, uint16_t arg);
uint8_t _STY(CPU* cpu, uint16_t arg);
uint8_t _TAX(CPU* cpu, uint16_t arg);
uint8_t _TAY(CPU* cpu, uint16_t arg);
uint8_t _TSX(CPU* cpu, uint16_t arg);
uint8_t _TXA(CPU* cpu, uint16_t arg);
uint8_t _TXS(CPU* cpu, uint16_t arg);
uint8_t _TYA(CPU* cpu, uint16_t arg);

#endif /* INSTRUCTIONS_H */
