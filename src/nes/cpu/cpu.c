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
	uint16_t jump_address = 0; /* address of the interrupt routine */

	const uint8_t N = *context & 1UL;
	const uint8_t I = ((*context >> 1) & 1UL) && !((self->P >> 2) & 1UL);

	if (self == NULL)
		return cycleCount;

	if (*context == 0)
		return cycleCount;

	/* INTERRUPT HANDLING */

	/* if any bit of context is set */
	if (N || I) {

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
		*ptr = (self->P & ~(1UL << 4)); /* clear B bit on stack */
		self->SP --;

		/* if the N bit of context is set */
		if (N){
			jump_address = NMI_JMP_ADD;
			*context &= ~(1UL);
		}

		/* if the I bit of context is set */
		else if (I){
			jump_address = IRQ_JMP_ADD;
			*context &= ~(1UL << 1);
		}

		/* fetch PC LSByte */
		ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, jump_address);
		self->PC = (uint16_t)(*ptr);

		/* fetch PC MSByte */
		ptr = (self->rmap->get)(self->rmap->memoryMap, AS_CPU, jump_address+1);
		self->PC |= (uint16_t)(*ptr) << 8;

		/* set I flag to disable further IRQs */
		self->P |= (1UL << 2);

		/* add 7 cycles to timing */
		cycleCount += 7;
	}

	return cycleCount;
}

void CPU_Destroy(CPU* self){

	if (self == NULL)
		return;

	free(self);
	return;
}
