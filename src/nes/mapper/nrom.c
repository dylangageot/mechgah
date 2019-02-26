#include <stdlib.h>
#include <stdio.h>
#include "nrom.h"

void* MapNROM_Create(uint8_t romSize, uint8_t mirroring) {
	MapNROM *self = (MapNROM*) malloc(sizeof(MapNROM));
	
	/*	If allocation failed, return NULL */
	if (self == NULL) {
		fprintf(stderr, "Error: can't allocate MapNROM structure "
				"at %s, line %d.\n", __FILE__, __LINE__);
		return self;
	}

	/*	Save context */
	self->romSize = romSize;
	self->mirroring = mirroring;

	/*	Allocation of ROM space */	
	switch (self->romSize % 2) {
		case NROM_16KIB:
			self->cpu.rom = (uint8_t*) malloc(16384);
			break;
		case NROM_32KIB:
			self->cpu.rom = (uint8_t*) malloc(32768); 
			break;
		default:
			break;
	}

	/*	Allocation of SRAM space */
	self->cpu.sram = (uint8_t*) malloc(8192);
	
	/*	Allocation of RAM space */
	self->cpu.ram = (uint8_t*) malloc(8192);
	
	/*	Allocation of CHR-ROM space */
	self->ppu.chr = (uint8_t*) malloc(8192);

	/*	Allocation of nametable space */
	self->ppu.nametable = (uint8_t*) malloc(2048);

	/*	Allocation of palette space */
	self->ppu.palette = (uint8_t*) malloc(32);

	/*	Test if allocation failed */
	if ((self->cpu.rom == NULL) || (self->cpu.ram == NULL) ||
		(self->cpu.sram == NULL) || (self->ppu.chr == NULL) ||
		(self->ppu.nametable == NULL) || (self->ppu.palette == NULL)) {
		fprintf(stderr, "Error: can't allocate memory for NROM "
				"at %s, line %d.\n", __FILE__, __LINE__);
		MapNROM_Destroy((void*) self);
		return NULL;
	}
	
	return self;
}

void MapNROM_Destroy(void* mapperData) {
	if (mapperData == NULL)
		return;

	MapNROM *self = (MapNROM*) mapperData;

	/*	Free only if it's necessary */
	if (self->cpu.rom != NULL)
		free((void*) self->cpu.rom);
	if (self->cpu.ram != NULL)
		free((void*) self->cpu.ram);
	if (self->cpu.sram != NULL)
		free((void*) self->cpu.sram);
	if (self->ppu.chr != NULL)
		free((void*) self->ppu.chr);
	if (self->ppu.nametable != NULL)
		free((void*) self->ppu.nametable);
	if (self->ppu.palette != NULL)
		free((void*) self->ppu.palette);

	free(mapperData);
	return;
}
