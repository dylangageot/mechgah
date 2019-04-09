#include "ppu.h"
#include "../../common/macro.h"
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
	/* Allocate PPU structure */
	PPU *self = (PPU*) malloc(sizeof(PPU));
	if (self == NULL) {
		ERROR_MSG("can't allocate memory for PPU structure");
		return NULL;
	}

	/* Connect mapper to PPU */
	self->mapper = mapper;
	return self;
}

char* RenderColorPalette(void) {
	/* Copy Color Palette to a new array */
	uint32_t *tab = (uint32_t*) malloc(64 * sizeof(uint32_t));
	memcpy(tab, colorPalette, 64 * sizeof(uint32_t));
	return (char*) tab;
}

uint8_t PPU_CheckRegister(PPU *self) {
	/* PPUCTRL behavior:
	 * Write	->	Update VRAM.t */
	if (Mapper_Ack(self->mapper, 0x2000)) {
		/* t: ...BA.. ........ = d: ......BA */
		self->vram.t &= ~0x0C00;
		self->vram.t |= (self->PPUCTRL & 0x03) << 10;
		return EXIT_SUCCESS;
	}


	/* PPUSTATUS behavior:
	 * Read		->	Clear Vertical Blank Bit and VRAM.w latch */
	if (Mapper_Ack(self->mapper, 0x2002)) {
		self->PPUCTRL &= ~(1 << 7);	
		self->vram.w = 0;
		return EXIT_SUCCESS;
	}

	/* OAMDATA behavior:
	 * Write	->	Update OAM[OAMADDR] with given value and inc OAMADDR */
	if (Mapper_Ack(self->mapper, 0x2003)) {
		self->OAM[self->OAMADDR++] = self->OAMDATA;
		return EXIT_SUCCESS;
	}

	/* PPUSCROLL behavior:
	 * 2 Write	->	VRAM.w++ and update VRAM.t */
	if (Mapper_Ack(self->mapper, 0x2005)) {
		if (self->vram.w == 0) {
			/* t: ....... ...HGFED = d: HGFED... */
			self->vram.t &= 0x001F;
			self->vram.t |= self->PPUSCROLL >> 3;
			/* x:              CBA = d: .....CBA */
			self->vram.x = self->PPUSCROLL & 0x07;
			self->vram.w++;
		} else {
			/* t: CBA..HG FED..... = d: HGFEDCBA */
			self->vram.t &= ~0xE3E0;
			self->vram.t |= self->PPUSCROLL << 12;
			self->vram.t |= (self->PPUSCROLL & 0xF8) << 2;
			self->vram.w = 0;	
		}
	}

	/* PPUADDR behavior:
	 * 2 Write	->	VRAM.w++ and update VRAM.t */
	if (Mapper_Ack(self->mapper, 0x2006)) {
		if (self->vram.w == 0) {
			/* t: 0FEDCBA ........ = d: ..FEDCBA */
			self->vram.t &= 0x7F00;
			self->vram.t |= self->PPUSCROLL << 8;
			self->vram.w++;
		} else {
			/* t: ....... HGFEDCBA = d: HGFEDCBA */
			self->vram.t &= ~0x00FF;
			self->vram.t |= self->PPUSCROLL;
			/* v = t */
			self->vram.v = self->vram.t;
			self->vram.w = 0;	
		}
	}

	/* PPUDATA behavior:
	 * R/W		->	Update Mapper[VRAM] with given value and VRAM++ */
	if (Mapper_Ack(self->mapper, 0x2007)) {
		if (self->PPUCTRL & 0x04)
			self->vram.v += 32;
		else
			self->vram.v++;
	}

	return EXIT_SUCCESS;
}

uint8_t PPU_Execute(PPU* self, uint8_t *context, uint8_t clock) {

	if (Mapper_Ack(self->mapper, 0x2004)) {
		printf("[PPU] PPUDATA:%02X accessed at CYC:%d\n", self->OAMDATA, clock);
	}

	return EXIT_SUCCESS;
}

void PPU_Destroy(PPU *self) {
	if (self == NULL)
		return;
	free(self);
}
