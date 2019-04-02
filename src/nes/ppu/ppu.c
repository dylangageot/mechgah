#include "ppu.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static uint32_t colorPalette[64] = {
	0x007C7C7C, 0x000000FC, 0x000000BC, 0x004428BC, 
	0x00940084, 0x00A80020, 0x00A81000, 0x00881400, 
	0x00503000, 0x00007800, 0x00006800, 0x00005800, 
	0x00004058, 0x00000000, 0x00000000, 0x00000000, 
	0x00BCBCBC, 0x000078F8, 0x000058F8, 0x006844FC, 
	0x00D800CC, 0x00E40058, 0x00F83800, 0x00E45C10, 
	0x00AC7C00, 0x0000B800, 0x0000A800, 0x0000A844, 
	0x00008888, 0x00000000, 0x00000000, 0x00000000, 
	0x00F8F8F8, 0x003CBCFC, 0x006888FC, 0x009878F8, 
	0x00F878F8, 0x00F85898, 0x00F87858, 0x00FCA044, 
	0x00F8B800, 0x00B8F818, 0x0058D854, 0x0058F898, 
	0x0000E8D8, 0x00787878, 0x00000000, 0x00000000, 
	0x00FCFCFC, 0x00A4E4FC, 0x00B8B8F8, 0x00D8B8F8, 
	0x00F8B8F8, 0x00F8A4C0, 0x00F0D0B0, 0x00FCE0A8, 
	0x00F8D878, 0x00D8F878, 0x00B8F8B8, 0x00B8F8D8,
	0x0000FCFC, 0x00F8D8F8, 0x00000000, 0x00000000 
};

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

char* RenderColorPalette(void) {
	uint32_t *tab = (uint32_t*) malloc(64 * sizeof(uint32_t));
	memcpy(tab, colorPalette, 64 * sizeof(uint32_t));
	return (char*) tab;
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
