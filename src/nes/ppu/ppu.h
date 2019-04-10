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
#include "../../common/stack.h"

typedef struct {
	uint16_t v;
	uint16_t t;
	uint8_t x;
	uint8_t w;
} VRAM;

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
	VRAM vram;
	uint8_t OAM[256];
	uint16_t cycle;
	int16_t scanline;
	uint8_t nbFrame;
	Mapper *mapper;
} PPU;

char* RenderColorPalette(void);

/**
 * \fn PPU_Create
 *
 * \param mapper instance of Mapper
 *
 * \return instance of PPU
 */
PPU* PPU_Create(Mapper *mapper);

/**
 * \fn PPU_Init
 * \brief Initialize PPU structure
 *
 * \param self instance of PPU
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t PPU_Init(PPU *self);

/**
 * \fn PPU_Execute
 *
 * \param self instance of PPU
 * \param context wire through every component
 * \param clock number of PPU cycle to consumme
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t PPU_Execute(PPU *self, uint8_t *context, uint8_t clock);

/**
 * \fn PPU_CheckRegister 
 *
 * \param self instance of PPU
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise 
 */
uint8_t PPU_CheckRegister(PPU *self); 

/**
 * \fn PPU_ManageTiming
 *
 * \param self instance of PPU
 * \param taskList instance of Stack to schedule task to execute
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t PPU_ManageTiming(PPU *self, Stack *taskList);

/**
 * \fn PPU_ClearFlag
 * \brief Clear Vertical Blank and Sprite 0 flag
 *
 * \param self instance of PPU
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t PPU_ClearFlag(PPU *self);

/**
 * \fn PPU_SetFlag
 * \brief Set Vertical Blank flag
 *
 * \param self instance of PPU
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t PPU_SetFlag(PPU *self);

/**
 * \fn PPU_ManageV
 * \brief Manage increment and boundaries of VRAM.v
 *
 * \param self instance of PPU
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t PPU_ManageV(PPU *self);


/**
 * \fn PPU_ClearSecondaryOAM
 * \brief Clear Secondary OAM
 *
 * \param self instance of PPU
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t PPU_ClearSecondaryOAM(PPU *self);

/**
 * \fn PPU_RefreshRegister
 *
 * \param self instance of PPU
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t PPU_RefreshRegister(PPU *self);

/**
 * \fn PPU_FetchTile
 *
 * \param self instance of PPU
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t PPU_FetchTile(PPU *self);

/**
 * \fn PPU_SpriteEvaluation
 *
 * \param self instance of PPU
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t PPU_SpriteEvaluation(PPU *self);

/**
 * \fn PPU_FetchSprite
 *
 * \param self instance of PPU
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t PPU_FetchSprite(PPU *self);

/**
 * \fn PPU_Draw
 *
 * \param self instance of PPU
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t PPU_Draw(PPU *self);

/**
 * \brief PPU_Destroy
 *
 * \param self instance of PPU
 */
void PPU_Destroy(PPU *self);

#endif /* PPU_H */
