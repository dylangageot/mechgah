/**
 * \file mapper.h
 * \brief Header including general structure for mapper
 * \author Dylan Gageot
 * \version 1.0
 * \date 2019-02-20
 */

#ifndef MAPPER_H
#define MAPPER_H

#include "stdint.h"

/**
 * \struct Mapper
 * \brief Generic structure to hold mapper
 */
typedef struct {
	uint8_t* (*get)(void*, uint8_t, uint16_t);		/*! Get data callback	*/
	void (*destroyer)(void*);						/*! Destroyer callback	*/
	void* memoryMap;								/*! Memory map			*/
} Mapper;

/**
 * \typedef RemoteMapper
 * \brief Use to point Mapper pointer in NES structure
 */
typedef RemoteMapper Mapper**;

/**
 * \brief Get mapper pointer from a RemoteMapper
 *
 * \param x RemoteMapper to exploit
 *
 * \return Mapper* to use
 */
#define _GetMapper(x) (*x)

/**
 * \struct PeripheralRegister
 * \brief Register use to communicate with PPU, APU and joystick
 *
 * The structure is composed of two arrays that represent bank in memory map : 
 *  - bank1 : @0x2000, registers for PPU
 *  - bank2 : @0x4000, registers for APU and joystick
 */
typedef struct {
	uint8_t bank1[8];
	uint8_t bank2[24];
} PeripheralRegister;

/**
 * \enum Bank1Register
 * \brief Mnemonic for Bank 1 registers
 */
enum Bank1Register {
	PPUCTRL = 0,
	PPUMASK,
	PPUSTATUS,
	OAMADDR,
	OAMDATA,
	PPUSCROLL,
	PPUDATA
};

/**
 * \enum Bank2Register
 * \brief Mnemonic for Bank 2 registers
 */
enum Bank2Register {
	SQ1_VOL = 0,
	SQ1_SWEEP,
	SQ1_LO,
	SQ1_HI,
	SQ2_VOL,
	SQ2_SWEEP,
	SQ2_LO,
	SQ2_HI,
	TRI_LINEAR,
	BANK2_UNUSED1,
	TRI_LO,
	TRI_HI,
	NOISE_VOL,
	BANK2_UNUSED2,
	NOISE_LO,
	NOISE_HI,
	DMC_FREQ,
	DMC_RAW,
	DMC_START,
	DMC_LEN,
	OAMDMA,
	SND_CHN,
	JOY1,
	JOY2
};

/**
 * \enum AddressSpace
 * \brief Use to specify to the mapper in which address space we want to 
 * retrieve the data.
 */
enum AddressSpace {
	AS_CPU = 0,
	AS_PPU
};

#endif /* MAPPER_H */
