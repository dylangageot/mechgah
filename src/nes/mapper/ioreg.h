/**
 * \file ioreg.h
 * \brief Header of IOReg module
 * \author Dylan Gageot
 * \version 1.0
 * \date 2019-03-31
 */

#ifndef IOREG_H
#define IOREG_H

#include "mapper.h"
#include "../cpu/cpu.h"
#include "../ppu/ppu.h"

/**
 * \struct IOReg
 * \brief Register use to communicate with PPU, APU and joystick
 *
 * The structure is composed of two arrays that represent bank in memory map : 
 *  - bank1 : @0x2000, registers for PPU
 *  - bank2 : @0x4000, registers for APU and joystick
 * Flags array are used to know if data was read or written
 */
typedef struct {
	uint8_t *bank1[8];
	uint8_t *bank2[32];
	uint8_t acknowledge[40];
	uint8_t dummy;
} IOReg;

/**
 * \fn IOReg_Create
 * \brief Instanciation of IOReg
 *
 * \return instance of IOReg if succeed
 */
IOReg* IOReg_Create(void);

/**
 * \brief IOReg_Connect
 *
 * \param self instance of IOReg
 * \param cpu instance of CPU
 * \param ppu instance of PPU
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise 
 */
uint8_t IOReg_Connect(IOReg *self, CPU *cpu, PPU *ppu);

/**
 * \brief Get pointer to access IO register
 *
 * \param self instance of IOReg
 * \param accessType which address type and which type of access (rd,wr) ?
 * \param address address to access
 *
 * \return pointer to selected IO register 
 */
uint8_t* IOReg_Get(IOReg *self, uint8_t accessType, uint16_t address);

/**
 * \brief Check if the selected IO register was accessed before and acknowledge
 * it
 *
 * \param self instance of IOReg
 * \param address address to check
 *
 * \return 1 if accessed, 0 otherwise 
 */
uint8_t IOReg_Ack(IOReg *self, uint16_t address);

/**
 * \fn IOReg_Destroy
 * \brief Free instance of IOReg
 *
 * \param self instance of IOReg
 */
void IOReg_Destroy(IOReg *self);


/**
 * \brief IOReg_Extract
 *
 * \param mapper instance of Mapper
 *
 * \return instance of IOReg
 */
IOReg* IOReg_Extract(Mapper *mapper);

/**
 * \enum Acknowledge
 * \brief Type of access
 */
enum Acknowledge {
	AC_NO = 0,
	AC_RD = 16,
	AC_WR = 32
};

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
	PPUADDR,
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

#endif /* IOREG_H */
