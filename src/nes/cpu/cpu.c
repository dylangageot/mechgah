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
	self->rmap = mapper;

	return self;
}

uint8_t* CPU_Init(CPU* self) {

	if (self == NULL)
		return 1;

	/* 8-bit registers */
	self->A = 0;
	self->X = 0;
	self->Y = 0;
	self->SP = 0xFF;
	self->P = 0;

	/* 16-bit program counter register */
	self->PC = 0;

	return 0;
}

uint8_t CPU_InterruptManager(CPU* self, uint8_t* context){

	uint8_t cycleCount = 0;

	if (self == NULL)
		return cycleCount;

	/* INTERRUPT HANDLING */

	/* if the N bit of context is set */
	if (*context & 0x01) {

		/* push PC MSByte on stack */
		uint8_t* ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, (0x0100+self->SP));
		*ptr = (uint8_t)(self->PC >> 8);
		self->SP --;

		/* push PC LSByte on stack */
		ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, (0x0100+self->SP));
		*ptr = (uint8_t)(self->PC);
		self->SP --;

		/* push P on stack */
		ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, (0x0100+self->SP));
		*ptr = self->P;
		self->SP --;

		/* fetch PC LSByte @ 0xFFFA*/
		ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, 0xFFFA);
		self->PC = (uint16_t)(*ptr);

		/* fetch PC MSByte @ 0xFFFB*/
		ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, 0xFFFB);
		self->PC |= (uint16_t)(*ptr) << 8;

		/* set I flag to disable further IRQs */
		self->P |= (0x01 << 2);

		/* clear context N byte */
		*context &= 0xFE;

		/* add 7 cycles to timing */
		cycleCount += 7;
	}

	/* if the I bit of context is set */
	else if ((*context >> 1) & 0x01) {
		/* same behavior as BRK but does not set B flag */

		/* add 7 cycles to timing */
		cycleCount += 7;
	}


	/*
	 * Adressing mode handling
	 * Check for Interrupt
	 * DMA management
	*/

	return cycleCount;
}


void CPU_Destroy(CPU* self){

	if (self == NULL)
		return;

	free(self);
	return;
}
