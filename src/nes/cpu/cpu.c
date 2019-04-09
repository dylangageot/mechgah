#include "cpu.h"
#include "instructions.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../common/macro.h"


CPU* CPU_Create(Mapper *mapper){

	CPU* self = (CPU*) malloc(sizeof(CPU));

	/*	If allocation failed, return NULL */
	if (self == NULL) {
		ERROR_MSG("can't allocate CPU structure");
		return self;
	}

	/* mapper used by the NES */
	self->mapper = mapper;

	return self;
}

uint8_t CPU_Init(CPU* self) {

	if (self == NULL)
		return EXIT_FAILURE;

	/* 8-bit registers */
	self->A = 0;
	self->X = 0;
	self->Y = 0;
	self->SP = 0x00;
	self->P = 0;
	self->cntDMA = -1;
	self->OAMDMA = 0;
	/* 16-bit program counter */
	self->PC = 0x0000;
	/* Remove debug log */
	remove("cpu.log");

	return EXIT_SUCCESS;
}

uint8_t CPU_InterruptManager(CPU* self, uint8_t* context){

	uint8_t cycleCount = 0;
	uint16_t jump_address = 0; /* address of the interrupt routine */
	uint8_t tmp; /* temporary variable to get data from memory */

	const uint8_t R = *context & 1UL;
	const uint8_t N = (*context >> 1) & 1UL;
	const uint8_t I = ((*context >> 2) & 1UL) && !((self->P >> 2) & 1UL);

	if (self == NULL)
		return cycleCount;

	if (*context == 0)
		return cycleCount;

	/* INTERRUPT HANDLING */

	/* if R bit of context is set */
	if (R) {
		self->SP -= 3;
	}
	/* if hardware interrupt occured */
	else if (N || I) {

		/* push PC MSByte on stack */
		tmp = self->PC >> 8;
		_PUSH(self, &tmp); 

		/* push PC LSByte on stack */
		tmp = self->PC & 0xFF;
		_PUSH(self, &tmp); 

		/* push P on stack */
		tmp = self->P & ~(1UL << 4); /* Clear B flag on stack */
		_PUSH(self, &tmp); 
	}
	else { /* if IRQ occured but they are disabled */
		return cycleCount;
	}

	/* if the R bit of context is set */
	if (R){
		jump_address = RES_JMP_ADD;
		*context &= ~(1UL);
		self->P |= 0x20;
	}
	/* if the N bit of context is set */
	else if (N){
		jump_address = NMI_JMP_ADD;
		*context &= ~(1UL << 1);
	}
	/* if the I bit of context is set and IRQ are enabled*/
	else if (I){
		jump_address = IRQ_JMP_ADD;
		*context &= ~(1UL << 2);
	}

	/* fetch PC LSByte */
	self->PC = (uint16_t) *(_LOAD(self, jump_address));

	/* fetch PC MSByte */
	self->PC |= (uint16_t) (*(_LOAD(self, jump_address + 1)) << 8);

	/* set I flag to disable further IRQs */
	self->P |= (1UL << 2);

	/* add 7 cycles to timing */
	cycleCount += 7;

	return cycleCount;
}

uint8_t CPU_Execute(CPU* self, uint8_t* context, uint32_t *clockCycle) {
	/* Instruction that will be initialize for execution */
	Instruction inst;

	*clockCycle += CPU_InterruptManager(self, context); 

	/* If no DMA operation is on-going, execute program */
	if (Instruction_DMA(&inst, self, clockCycle) == DMA_OFF) {
		/* Fetch instruction information from opcode and arg */
		if (Instruction_Fetch(&inst, self) == EXIT_FAILURE)
			return EXIT_FAILURE;

		/* Resolve addressing mode from instruction information */
		if (Instruction_Resolve(&inst, self) == EXIT_FAILURE) 
			return EXIT_FAILURE;
	}

#ifdef DEBUG_CPU
	/* Log execution information
	 * This operation will drastically impact execution time */
	Instruction_PrintLog(&inst, self, *clockCycle); 
#endif

	/* If no instruction is coded for this opcode, exit */
	if (inst.opcode.inst == NULL) {
		ERROR_MSG("instruction fetched is not described");	
		return EXIT_FAILURE;
	}

	/* Execute instruction */
	*clockCycle += inst.opcode.inst(self, &inst);	

	return EXIT_SUCCESS;
}

void CPU_Destroy(CPU* self){
	if (self == NULL)
		return;
	/* Free CPU */
	free(self);
	return;
}
