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
#include "ioreg.h"
#include "../loader/loader.h"

#define NROM_RAM_SIZE 2048

/**
 * \struct MapNROM_CPU
 * \brief Memory map of CPU when using NROM mapper
 */
typedef struct {
	uint8_t *ram;
	IOReg * ioReg;
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
	uint8_t dummy;
	/*	Mapper data */
	uint8_t romSize;
	uint8_t mirroring;
} MapNROM;

/**
 * \enum NROMSize
 * \brief Describe if the loaded ROM use 16 or 32kiB
 */
enum NROMSize {
	NROM_32KIB = 0,
	NROM_16KIB,
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
 * \param header containing all ROM informations
 *
 * \return pointer to the new allocated mapper
 */
Mapper* MapNROM_Create(Header * header);

/**
 * \fn MapNROM_Get
 * \brief Give access to the data addressed in argument
 *
 * \param mapperData Memory map pointer
 * \param space CPU or PPU address space ?
 * \param address Address of the data to fetch
 *
 * \return void* pointer of the data addressed
 */
void* MapNROM_Get(void* mapperData, uint8_t space, uint16_t address);

/**
 * \fn MapNROM_Ack
 * \brief Acknowledge IOReg from MapNROM
 *
 * \param mapperData instance of MapNROM
 * \param address address to check if it was accessed
 *
 * \return 1 if it was accessed, 0 otherwise
 */
uint8_t MapNROM_Ack(void *mapperData, uint16_t address);

/**
 * \fn MapNROM_Destroy
 * \brief Destroy/free the mapper
 *
 * \param mapperData Memory map pointer
 */
void MapNROM_Destroy(void* mapperData);

#endif /* NROM_H */
