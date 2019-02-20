/**
 * \file peripheralRegister.h
 * \brief Header including struct for peripheral registers
 * \author Dylan Gageot
 * \version 1.0
 * \date 2019-02-20
 */

#ifndef PERIPHERALREGISTER_H
#define PERIPHERALREGISTER_H

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
	CPU = 0,
	PPU
};

#endif /* PERIPHERALREGISTER_H */