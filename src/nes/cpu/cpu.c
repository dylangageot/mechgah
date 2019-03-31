#include "cpu.h"
#include "instructions.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

CPU* CPU_Create(Mapper* mapper){

	CPU* self = (CPU *)malloc(sizeof(CPU));

	/*	If allocation failed, return NULL */
	if (self == NULL) {
		fprintf(stderr, "Error: can't allocate CPU structure "
				"at %s, line %d.\n", __FILE__, __LINE__);
		return self;
	}

	/* mapper used by the NES */
	self->mapper = mapper;

	return self;
}

uint8_t CPU_Init(CPU* self) {

	if (self == NULL)
		return 1;

	/* 8-bit registers */
	self->A = 0;
	self->X = 0;
	self->Y = 0;
	self->SP = 0x00;
	self->P = 0;

	/* 16-bit program counter */
	self->PC = 0x0000;
	/* Remove debug log */
	remove("cpu.log");

	return 0;
}

uint8_t CPU_InterruptManager(CPU* self, uint8_t* context){

	uint8_t cycleCount = 0;
	uint16_t jump_address = 0; /* address of the interrupt routine */
	uint8_t* ptr; /* pointer used to fetch and write in memory */

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
		ptr = Mapper_Get(self->mapper, AS_CPU, (0x0100+self->SP));
		*ptr = (uint8_t)(self->PC >> 8);
		self->SP --;

		/* push PC LSByte on stack */
		ptr = Mapper_Get(self->mapper, AS_CPU, (0x0100+self->SP));
		*ptr = (uint8_t)(self->PC);
		self->SP --;

		/* push P on stack */
		ptr = Mapper_Get(self->mapper, AS_CPU, (0x0100+self->SP));
		*ptr = (self->P & ~(1UL << 4)); /* clear B bit on stack */
		self->SP --;
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
	ptr = Mapper_Get(self->mapper, AS_CPU, jump_address);
	self->PC = (uint16_t)(*ptr);

	/* fetch PC MSByte */
	ptr = Mapper_Get(self->mapper, AS_CPU, jump_address+1);
	self->PC |= (uint16_t)(*ptr) << 8;

	/* set I flag to disable further IRQs */
	self->P |= (1UL << 2);

	/* add 7 cycles to timing */
	cycleCount += 7;

	return cycleCount;
}

uint32_t CPU_Execute(CPU* self, uint8_t* context, uint32_t *clockCycle) {
	if (self == NULL)
		return 0;

	Instruction inst;

	*clockCycle += CPU_InterruptManager(self, context); 

	/* Fetch instruction information from opcode and arg */
	if (Instruction_Fetch(&inst, self) == 0)
		return 0;

	/* Log execution information */
	Instruction_PrintLog(&inst, self, *clockCycle); 

	/* Resolve addressing mode from instruction information */
	if (Instruction_Resolve(&inst, self) == 0)
		return 0;

	/* If no instruction is coded for this opcode, exit */
	if (inst.opcode.inst == NULL)
		return 0;

	/* Execute instruction */
	*clockCycle += inst.opcode.inst(self, &inst);	

	return *clockCycle;
}

void CPU_Destroy(CPU* self){

	if (self == NULL)
		return;

	free(self);
	return;
}
