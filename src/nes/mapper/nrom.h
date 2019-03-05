/**
 * \file nrom.h
 * \brief Header for NROM mapper
 * \author Dylan Gageot
 * \version 1.0
 * \date 2019-02-20
 */

#ifndef NROM_H
#define NROM_H

#include "mapper.h"

#define NROM_RAM_SIZE 2048

/**
 * \struct MapNROM_CPU
 * \brief Memory map of CPU when using NROM mapper
 */
typedef struct {
	uint8_t *ram;
	PeripheralRegister ioReg;
	uint8_t *sram;
	uint8_t *rom;
} MapNROM_CPU;

/**
 * \struct MapNROM_PPU
 * \brief Memory map of PPU when using NROM mapper
 */
typedef struct {
	uint8_t *chr;
	uint8_t *nametable;
	uint8_t *palette;
} MapNROM_PPU;

/**
 * \struct MapNROM
 * \brief Memory map used when using NROM mapper
 */
typedef struct {
	/*	Accessible data */
	MapNROM_CPU cpu;
	MapNROM_PPU ppu;
	/*	Mapper data */
	uint8_t romSize;
	uint8_t mirroring;
} MapNROM;

/**
 * \enum NROMSize
 * \brief Describe if the loaded ROM use 16 or 32kiB
 */
enum NROMSize {
	NROM_16KIB = 0,
	NROM_32KIB,
	NROM_UNDEFINED
};

/**
 * \enum NROMMirroring
 * \brief Describe the mirroring mecanism to use
 */
enum NROMMirroring {
	NROM_HORIZONTAL = 0,
	NROM_VERTICAL
};

/**
 * \fn MapNROM_Create
 * \brief Allocate memory for NROM mapper
 *
 * \param romSize size of PGR-ROM
 * \param mirroring PPU's mirroring mecanism
 *
 * \return pointer to the new allocated mapper
 */
void* MapNROM_Create(uint8_t romSize, uint8_t mirroring);

/**
 * \fn MapNROM_Get
 * \brief Give access to the data addressed in argument
 *
 * \param mapperData Memory map pointer
 * \param space CPU or PPU address space ?
 * \param address Address of the data to fetch
 *
 * \return uint8_t pointer of the data addressed
 */
uint8_t* MapNROM_Get(void* mapperData, uint8_t space, uint16_t address);

/**
 * \fn MapNROM_Destroy
 * \brief Destroy/free the mapper
 *
 * \param mapperData Memory map pointer
 */
void MapNROM_Destroy(void* mapperData);

#endif /* NROM_H */
