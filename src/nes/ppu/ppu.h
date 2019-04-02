/**
 * \file ppu.h
 * \brief Header of PPU module
 * \author Dylan Gageot
 * \version 1.0
 * \date 2019-03-31
 */

#ifndef PPU_H
#define PPU_H

#include "../mapper/mapper.h"

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
	uint16_t cycle;
	int16_t scanline;
	Mapper *mapper;
} PPU;

char* RenderColorPalette(void);

/**
 * \brief PPU_Create
 *
 * \param mapper instance of Mapper
 *
 * \return instance of PPU
 */
PPU* PPU_Create(Mapper *mapper);

/**
 * \brief PPU_Execute
 *
 * \param self instance of PPU
 * \param context wire through every component
 * \param clockCycle number of clock cycle consummed
 */
void PPU_Execute(PPU* self, uint8_t *context, uint32_t clockCycle);

/**
 * \brief PPU_Destroy
 *
 * \param self instance of PPU
 */
void PPU_Destroy(PPU *self);

#endif /* PPU_H */
