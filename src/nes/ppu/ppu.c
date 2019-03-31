#include "ppu.h"
#include <stdlib.h>
#include <stdio.h>

PPU* PPU_Create(Mapper *mapper) {
	if (mapper == NULL)
		return NULL;

	/* Allocate PPU structure */
	PPU *self = (PPU*) malloc(sizeof(PPU));
	if (self != NULL) {
		self->mapper = mapper;
	}

	return self;
}

void PPU_Execute(PPU* self, uint8_t *context, uint32_t clockCycle) {
	if ((self == NULL) || (context == NULL))
		return;

	if (Mapper_Ack(self->mapper, 0x2004)) {
		printf("[PPU] PPUDATA:%02X accessed at CYC:%d\n", self->OAMDATA, clockCycle);
	}
}

void PPU_Destroy(PPU *self) {
	if (self == NULL)
		return;
	free(self);
}
