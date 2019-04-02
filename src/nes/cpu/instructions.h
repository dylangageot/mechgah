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

#define NBARG_DMA 0xFF

typedef struct Instruction Instruction;

/**
 * \struct Opcode
 * \brief Hold instruction function and addressing mode associated
 */
typedef struct {
	uint8_t (*inst)(CPU*, Instruction*);
	uint8_t addressingMode;
	uint8_t cycle;
} Opcode;

/**
 * \struct Instruction
 * \brief Hold Opcode and arguments associated
 */
struct Instruction {
	Opcode opcode;
	uint8_t opcodeArg[2];
	uint8_t *dataMem;
	uint16_t dataAddr;
	uint8_t pageCrossed;
	/* Logger useful-data */
	uint16_t lastPC;
	uint8_t rawOpcode;
	uint8_t nbArg;
};

/**
 * \enum AddressingMode
 * \brief Mnemonic for every addressing mode
 */
enum AddressingMode {
	IMP = 0,      /*! Implied	 		        0 byte is read in ROM   */
	ACC,		  /*! Accumulator	            0 byte is read in ROM   */
	ZEX,		  /*! Zero-page Indexed X       1 bytes is read in ROM  */
	ZEY,          /*! Zero-page Indexed Y       1 bytes is read in ROM  */
	INX,          /*! Pre-indexed indirect X	1 bytes is read in ROM  */
	INY,          /*! Post-indexed indirect Y	1 bytes is read in ROM  */
	IMM,		  /*! Immediate			        1 bytes is read in ROM  */
	ZER,		  /*! Zero page		          	1 bytes is read in ROM  */
	REL,		  /*! Relative            		1 bytes is read in ROM  */
	ABS,		  /*! Absolute                  2 bytes is read in ROM  */
	ABX,		  /*! Indexed X                 2 bytes is read in ROM  */
	ABY,		  /*! Indexed Y         	    2 bytes is read in ROM  */
	ABI,		  /*! Absolute indirect	        2 bytes is read in ROM  */
	NUL           /*! Undefined			        0 bytes is read in ROM  */
};

/**
 * \fn Instruction_DMA
 * \brief Manage DMA request
 *
 * \param self instance of Instruction
 * \param cpu instance of CPU
 *
 * \return 0 if it failed, 1 otherwise 
 */
uint8_t Instruction_DMA(Instruction *self, CPU *cpu, uint32_t *clockCycle);

/**
 * \fn Instruction_Fetch
 * \brief Fetch and decode instruction from PGR-ROM
 *
 * \param self instance of Instruction
 * \param cpu instance of CPU
 *
 * \return 0 if it failed, 1 otherwise
 */
uint8_t Instruction_Fetch(Instruction *self, CPU *cpu);

/**
 * \fn Instruction_Resolve
 * \brief Get the data to execute the instruction
 *
 * \param self instance of Instruction
 * \param cpu instance of CPU
 *
 * \return 0 if it failed, 1 otherwise
 */
uint8_t Instruction_Resolve(Instruction *self, CPU *cpu);

/**
 * \brief Print instruction 
 *
 * \param self instance of CPU
 * \param inst instance of Instruction
 * \param clockCycle number of clock cycle needed
 */
void Instruction_PrintLog(Instruction *self, CPU *cpu, uint32_t clockCycle);

/**
 * \fn Opcode_Get
 * \brief Function to use opcode LUT in unit-test
 *
 * \param index
 *
 * \return Opcode struct 
 */
Opcode Opcode_Get(uint8_t index);

/**
 * \brief Useful function to ease implementation of the following instructions 
 */
void _SET_SIGN(CPU *cpu, uint8_t *src);
void _SET_ZERO(CPU *cpu, uint8_t *src);
void _SET_CARRY(CPU *cpu, uint8_t cond);
void _SET_OVERFLOW(CPU *cpu, uint8_t cond);
void _SET_INTERRUPT(CPU *cpu);
void _SET_BREAK(CPU *cpu);
uint16_t _REL_ADDR(CPU *cpu, int8_t *src);
void _SET_SR(CPU *cpu, uint8_t *src);
uint8_t _GET_SR(CPU *cpu);
uint8_t _PULL(CPU *cpu);
void _PUSH(CPU *cpu, uint8_t *src);
uint8_t _LOAD(CPU *cpu, uint16_t address);
void _STORE(CPU *cpu, uint16_t address, uint8_t *src);
uint8_t _IF_CARRY(CPU *cpu);
uint8_t _IF_OVERFLOW(CPU *cpu);
uint8_t _IF_SIGN(CPU *cpu);
uint8_t _IF_ZERO(CPU *cpu);
uint8_t _IF_INTERRUPT(CPU *cpu);
uint8_t _IF_BREAK(CPU *cpu);
uint8_t _BRANCH(CPU* cpu, Instruction *arg, uint8_t cond); 

/**
 * \brief Instruction set of 6502
 *
 * \param cpu instance of CPU
 * \param arg instance of Instruction
 *
 * \return number of CPU cycle consumed
 */
uint8_t _ADC(CPU *cpu, Instruction *arg);
uint8_t _AND(CPU *cpu, Instruction *arg);
uint8_t _ASL(CPU *cpu, Instruction *arg);
uint8_t _BCC(CPU *cpu, Instruction *arg);
uint8_t _BCS(CPU *cpu, Instruction *arg);
uint8_t _BEQ(CPU *cpu, Instruction *arg);
uint8_t _BIT(CPU *cpu, Instruction *arg);
uint8_t _BMI(CPU *cpu, Instruction *arg);
uint8_t _BNE(CPU *cpu, Instruction *arg);
uint8_t _BPL(CPU *cpu, Instruction *arg);
uint8_t _BRK(CPU *cpu, Instruction *arg);
uint8_t _BVC(CPU *cpu, Instruction *arg);
uint8_t _BVS(CPU *cpu, Instruction *arg);
uint8_t _CLC(CPU *cpu, Instruction *arg);
uint8_t _CLD(CPU *cpu, Instruction *arg);
uint8_t _CLI(CPU *cpu, Instruction *arg);
uint8_t _CLV(CPU *cpu, Instruction *arg);
uint8_t _CMP(CPU *cpu, Instruction *arg);
uint8_t _CPX(CPU *cpu, Instruction *arg);
uint8_t _CPY(CPU *cpu, Instruction *arg);
uint8_t _DEC(CPU *cpu, Instruction *arg);
uint8_t _DEX(CPU *cpu, Instruction *arg);
uint8_t _DEY(CPU *cpu, Instruction *arg);
uint8_t _EOR(CPU *cpu, Instruction *arg);
uint8_t _INC(CPU *cpu, Instruction *arg);
uint8_t _INX(CPU *cpu, Instruction *arg);
uint8_t _INY(CPU *cpu, Instruction *arg);
uint8_t _JMP(CPU *cpu, Instruction *arg);
uint8_t _JSR(CPU *cpu, Instruction *arg);
uint8_t _LDA(CPU *cpu, Instruction *arg);
uint8_t _LDX(CPU *cpu, Instruction *arg);
uint8_t _LDY(CPU *cpu, Instruction *arg);
uint8_t _LSR(CPU *cpu, Instruction *arg);
uint8_t _NOP(CPU *cpu, Instruction *arg);
uint8_t _ORA(CPU *cpu, Instruction *arg);
uint8_t _PHA(CPU *cpu, Instruction *arg);
uint8_t _PHP(CPU *cpu, Instruction *arg);
uint8_t _PLA(CPU *cpu, Instruction *arg);
uint8_t _PLP(CPU *cpu, Instruction *arg);
uint8_t _ROL(CPU *cpu, Instruction *arg);
uint8_t _ROR(CPU *cpu, Instruction *arg);
uint8_t _RTI(CPU *cpu, Instruction *arg);
uint8_t _RTS(CPU *cpu, Instruction *arg);
uint8_t _SBC(CPU *cpu, Instruction *arg);
uint8_t _SEC(CPU *cpu, Instruction *arg);
uint8_t _SED(CPU *cpu, Instruction *arg);
uint8_t _SEI(CPU *cpu, Instruction *arg);
uint8_t _STA(CPU *cpu, Instruction *arg);
uint8_t _STX(CPU *cpu, Instruction *arg);
uint8_t _STY(CPU *cpu, Instruction *arg);
uint8_t _TAX(CPU *cpu, Instruction *arg);
uint8_t _TAY(CPU *cpu, Instruction *arg);
uint8_t _TSX(CPU *cpu, Instruction *arg);
uint8_t _TXA(CPU *cpu, Instruction *arg);
uint8_t _TXS(CPU *cpu, Instruction *arg);
uint8_t _TYA(CPU *cpu, Instruction *arg);

#endif /* INSTRUCTIONS_H */
