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
	
	/*	Allocation of IOReg space */
	self->cpu.ioReg = IOReg_Create();

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
		(self->cpu.ioReg == NULL) || (self->cpu.sram == NULL) || 
		(self->ppu.chr == NULL) || (self->ppu.nametable == NULL) || 
		(self->ppu.palette == NULL)) {
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
	
	IOReg_Destroy(self->cpu.ioReg);

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

uint8_t* MapNROM_Get(void* mapperData, uint8_t space, uint16_t address) {
	/* If no mapperData has been given, return NULL */
	if (mapperData == NULL)
		return NULL;
	
	/* Cast to MapNROM */
	MapNROM *map = (MapNROM*) mapperData;
	
	if (space == AS_CPU) {
		
		MapNROM_CPU *cpu = &map->cpu;
		/* Which memory is addressed? */
		/* 0x0000 -> 0x1FFF : RAM */
		if (_ADDRESS_INF(address, 0x1FFF)) {
			return cpu->ram + (address & 0x07FF);
		/* 0x2000 -> 0x4017 : IO bank 1 */
		} else if (_ADDRESS_IN(address, 0x2000, 0x401F)) {
			return IOReg_Get(cpu->ioReg, address);
		/* 0x6000 -> 0x7FFF : SRAM */
		} else if (_ADDRESS_IN(address, 0x6000, 0x7FFF)) {
			return cpu->sram + (address & 0x1FFF);
		/* 0x8000 -> 0xBFFF : PRGROM 1 */
		} else if (_ADDRESS_IN(address, 0x8000, 0xBFFF)) {
			return cpu->rom + (address & 0x3FFF);
		/* 0xC000 -> 0xFFFF : PRGROM 2 */
		} else if (_ADDRESS_SUP(address, 0xC000)) {
			/* Function of ROM size, map twice or following memory */
			switch (map->romSize % 2) {
				case NROM_16KIB:
					return cpu->rom + (address & 0x3FFF);
				case NROM_32KIB:
					return cpu->rom + (address & 0x7FFF);
				default:
					break;
			}
		}
	}
	/* else if (space == AS_PPU) {*/

	return NULL;
}

uint8_t MapNROM_Ack(void *mapperData, uint16_t address) {
	if (mapperData == NULL)
		return 0;
	MapNROM *self = (MapNROM*) mapperData;
	return IOReg_Ack(self->cpu.ioReg, address);
}
