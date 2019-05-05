#include <stdlib.h>
#include <stdio.h>
#include "nrom.h"
#include "../../common/macro.h"

Mapper* MapNROM_Create(Header * header) {
	MapNROM *mapperData = (MapNROM*) malloc(sizeof(MapNROM));
	/*	If allocation failed, return NULL */
	if (mapperData == NULL) {
		ERROR_MSG("can't allocate MapNROM structure");
		return NULL;
	}

	/* Allocate Mapper structure */
	Mapper *self = Mapper_Create(MapNROM_Get,
								 MapNROM_Destroy,
								 MapNROM_Ack,
								 mapperData);
	if (self == NULL) {
		MapNROM_Destroy(mapperData);
		return self;
	}

	/*	Save context */
	mapperData->romSize = header->romSize;
	mapperData->mirroring = header->mirroring;

	/*	Allocation of ROM space */
	switch (mapperData->romSize % 2) {
		case NROM_16KIB:
			mapperData->cpu.rom = (uint8_t*) malloc(16384);
			break;
		case NROM_32KIB:
			mapperData->cpu.rom = (uint8_t*) malloc(32768);
			break;
		default:
			break;
	}

	/*	Allocation of SRAM space */
	mapperData->cpu.sram = (uint8_t*) malloc(8192);

	/*	Allocation of IOReg space */
	mapperData->cpu.ioReg = IOReg_Create();

	/*	Allocation of RAM space */
	mapperData->cpu.ram = (uint8_t*) malloc(8192);

	/*	Allocation of CHR-ROM space */
	mapperData->ppu.chr = (uint8_t*) malloc(8192);

	/*	Allocation of nametable space */
	mapperData->ppu.nametable = (uint8_t*) malloc(2048);

	/*	Allocation of palette space */
	mapperData->ppu.palette = (uint8_t*) malloc(256);


	/*	Test if allocation failed */
	if ((mapperData->cpu.rom == NULL) || (mapperData->cpu.ram == NULL) ||
		(mapperData->cpu.ioReg == NULL) || (mapperData->cpu.sram == NULL) ||
		(mapperData->ppu.chr == NULL) || (mapperData->ppu.nametable == NULL) ||
		(mapperData->ppu.palette == NULL)) {
		ERROR_MSG("can't allocate memory for NROM");
		Mapper_Destroy(self);
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

void* MapNROM_Get(void* mapperData, uint8_t space, uint16_t address) {
	/* If no mapperData has been given, return NULL */
	if (mapperData == NULL)
		return NULL;

	/* Cast to MapNROM */
	MapNROM *map = (MapNROM*) mapperData;

	/* Retrieve type of access and address space to fetch from */
	uint8_t accessType = space & 0xF0;
	space &= 0x0F;

	if (space == AS_CPU) {

		MapNROM_CPU *cpu = &map->cpu;
		/* Which memory is addressed? */
		/* 0x0000 -> 0x1FFF : RAM */
		if (VALUE_INF(address, 0x1FFF)) {
			return cpu->ram + (address & 0x07FF);
		/* 0x2000 -> 0x4017 : IO bank 1 */
		} else if (VALUE_IN(address, 0x2000, 0x401F)) {
			return IOReg_Get(cpu->ioReg, accessType, address);
		/* 0x6000 -> 0x7FFF : SRAM */
		} else if (VALUE_IN(address, 0x6000, 0x7FFF)) {
			return cpu->sram + (address & 0x1FFF);
		/* 0x8000 -> 0xBFFF : PRGROM 1 */
		} else if (VALUE_IN(address, 0x8000, 0xBFFF)) {
			return cpu->rom + (address & 0x3FFF);
		/* 0xC000 -> 0xFFFF : PRGROM 2 */
		} else if (VALUE_SUP(address, 0xC000)) {
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

	} else if (space == AS_PPU) {

		MapNROM_PPU *ppu = &map->ppu;
		address &= 0x3FFF;
		/* Which memory is addressed? */
		/* 0x0000 -> 0x1FFF : Pattern Table */
		if (VALUE_INF(address, 0x1FFF)) {
			return ppu->chr + (address & 0x1FFF);
		/* 0x2000 -> 0x3EFF : Nametable and Attribute Table */
		} else if (VALUE_IN(address, 0x2000, 0x3EFF)) {
			/* Nametable Mirroring */
			switch (map->mirroring % 2) {
				case NROM_HORIZONTAL:
					if (address & 0x0800)
						return ppu->nametable + 0x400 + (address & 0x03FF);
					else
						return ppu->nametable + (address & 0x03FF);
				case NROM_VERTICAL:
					if (address & 0x0400)
						return ppu->nametable + 0x400 + (address & 0x03FF);
					else
						return ppu->nametable + (address & 0x03FF);
				default:
					break;
			}
		/* 0x3F00 -> 0x3FFF : Palette */
		} else if (VALUE_IN(address, 0x3F00, 0x3FFF)) {
			if ((address & 0x0003) != 0)
				return ppu->palette + (address & 0x00FF);
			else
				return ppu->palette + (address & 0x000C);;
		}

	} else if (space == AS_LDR) {

		switch (address) {
			case LDR_PRG:
				return map->cpu.rom;
			case LDR_CHR:
				return map->ppu.chr;
			case LDR_IOR:
				return map->cpu.ioReg;
			default:
				break;
		}

	}

	return NULL;
}

uint8_t MapNROM_Ack(void *mapperData, uint16_t address) {
	if (mapperData == NULL)
		return 0;
	MapNROM *self = (MapNROM*) mapperData;
	return IOReg_Ack(self->cpu.ioReg, address);
}
