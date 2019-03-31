/**
 * \file ppu.h
 * \brief Header of PPU module
 * \author Dylan Gageot
 * \version 1.0
 * \date 2019-03-31
 */

#ifndef PPU_H
#define PPU_H

typedef struct {
	/* IO Register */
	uint8_t PPUCTRL;
	uint8_t PPUMASK;
	uint8_t PPUSTATUS;
	uint8_t OAMADDR;
	uint8_t OAMDATA;
	uint8_t PPUSCROLL;
	uint8_t PPUADDR;
	uint8_t PPUDATA;
	/* Internal Register */
	uint16_t VRAMADDR;
} PPU;

#endif /* PPU_H */
