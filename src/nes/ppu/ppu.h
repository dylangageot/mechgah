/**
 * \file ppu.h
 * \brief header file of PPU module
 * \author Dylan Gageot
 * \version 1.0
 * \date 2019-03-31
 */

#ifndef PPU_H
#define PPU_H

#include "../mapper/mapper.h"
#include "../../common/stack.h"

/**
 * \brief Hold pointer that is used to address VRAM
 */
typedef struct {
	uint16_t v;		/*!< Current address pointer	*/
	uint16_t t;		/*!< Temporary address pointer	*/
	uint8_t x;		/*!< Fine X component			*/
	uint8_t w;		/*!< Write toggle				*/
} VRAM;

/**
 * \brief Hold registers for sprite rendering
 */
typedef struct {
	uint8_t patternL;		/*!< Sprite bitmap low shift-reg	*/
	uint8_t patternH;		/*!< Sprite bitmap high shift-reg	*/
	uint8_t attribute;		/*!< Attribute byte from OAM		*/
	uint8_t x;				/*!< X-coordonate down-counter		*/
	uint8_t isSpriteZero;	/*!< Sprite is Sprite 0 flag		*/
} Sprite;

/**
 * \brief Hold every variable needed to run PPU
 */
typedef struct {
	/* IO Register */
	uint8_t PPUCTRL;		/*!< PPU control register	*/
	uint8_t PPUMASK;		/*!< PPU mask register		*/
	uint8_t PPUSTATUS;		/*!< PPU status register	*/
	uint8_t OAMADDR;		/*!< OAM address register	*/
	uint8_t OAMDATA;		/*!< OAM data register		*/
	uint8_t PPUSCROLL;		/*!< PPU scroll register	*/
	uint8_t PPUADDR;		/*!< PPU address register	*/
	uint8_t PPUDATA;		/*!< PPU data register		*/
	/* Internal Register */
	Mapper *mapper;			/*!< Mapper to get data from*/
	VRAM vram;				/*!< VRAM address			*/
	/* Timing */
	uint16_t cycle;			/*!< Cycle counter			*/
	int16_t scanline;		/*!< Scanline counter		*/
	/* Flags and informations */
	uint8_t nbFrame;		/*!< Odd/even frame counter	*/
	uint8_t nmiSent;		/*!< NMI sent flag			*/
	uint8_t pictureDrawn;	/*!< Picture drawn flag		*/
	/* Sprite evaluation */
	uint8_t OAM[256];		/*!< OAM array				*/
	uint8_t SOAM[32];		/*!< Secondary OAM array	*/
	uint8_t SOAMADDR;		/*!< SOAM address			*/
	uint8_t spriteState;	/*!< Sprite evaluation state*/
	uint8_t spriteData;		/*!< Sprite evaluation data */
	uint8_t spriteZero;		/*!< Sprite zero on scanline*/
	/* Graphic memory */
	uint32_t *image;		/*!< Pixel array			*/
	/* shift registers filled with values from the pattern table */
	uint16_t bitmapL;		/*!< Tile bitmap low shift-reg	*/
	uint16_t bitmapH;		/*!< Tile bitmap high shift-reg */
	/* shift registers for nametable and attribute table values */
	uint16_t attributeL;	/*!< Tile attribute low shift-reg	*/
	uint16_t attributeH;	/*!< Tile attribute high shift-reg	*/
	/* Sprites array for rendering */
	Sprite sprite[8];		/*!< Sprite rendering registers		*/

} PPU;

/**
 * \brief Render 64 pixel with color palette of NES
 *
 * \return allocate array (don't forger to free)
 */
char* RenderColorPalette(void);

/**
 * \brief Create instance of PPU
 * \param mapper instance of Mapper
 *
 * \return instance of PPU
 */
PPU* PPU_Create(Mapper *mapper);

/**
 * \brief Initialize PPU structure
 *
 * \param self instance of PPU
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t PPU_Init(PPU *self);

/**
 * \brief Execute PPU from a given timestamp
 *
 * \param self instance of PPU
 * \param context wire through every component
 * \param clock number of PPU cycle to consumme
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t PPU_Execute(PPU *self, uint8_t *context, uint8_t clock);

/**
 * \brief Draw a specified nametable into an array
 *
 * \param self instance of PPU
 * \param image array to draw in
 * \param index specified nametable
 */
void PPU_RenderNametable(PPU *self, uint32_t *image, uint8_t index);

/**
 * \brief Draw sprite into an array
 *
 * \param self instance of PPU
 * \param image array to draw in
 */
void PPU_RenderSprites(PPU *self, uint32_t *image);

/**
 * \brief Does the picture has been drawn by the component ?
 *
 * \param self instance of PPU
 *
 * \return 1 if picture is availaible, 0 otherwise
 */
uint8_t PPU_PictureDrawn(PPU *self);

/**
 * \brief Increment cycle and scanline
 *
 * \param self instance of PPU
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t PPU_UpdateCycle(PPU *self);

/**
 * \brief Update registers in case of access from CPU
 *
 * \param self instance of PPU
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise 
 */
uint8_t PPU_CheckRegister(PPU *self); 

/**
 * \brief Fill a stack FILO with tasks to execute at a specific cycle
 *
 * \param self instance of PPU
 * \param taskList instance of Stack to schedule task to execute
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t PPU_ManageTiming(PPU *self, Stack *taskList);

/**
 * \brief Clear Vertical Blank and Sprite 0 flag
 *
 * \param self instance of PPU
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t PPU_ClearFlag(PPU *self);

/**
 * \brief Set Vertical Blank flag
 *
 * \param self instance of PPU
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t PPU_SetFlag(PPU *self);

/**
 * \brief Increment Corse X component in VRAM.v
 *
 * \param self instance of PPU
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t PPU_IncrementCorseX(PPU *self);

/**
 * \brief Increment Y component in VRAM.v
 *
 * \param self instance of PPU
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t PPU_IncrementY(PPU *self);

/**
 * \brief Manage increment and boundaries of VRAM.v
 *
 * \param self instance of PPU
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t PPU_ManageV(PPU *self);

/**
 * \brief Clear Secondary OAM
 *
 * \param self instance of PPU
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t PPU_ClearSecondaryOAM(PPU *self);

/**
 * \brief Refresh registers after PPU execution
 *
 * \param self instance of PPU
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t PPU_RefreshRegister(PPU *self, uint8_t *context);

/**
 * \brief Fill internal shift registers for background rendering
 *
 * \param self instance of PPU
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t PPU_FetchTile(PPU *self);

/**
 * \brief Fill secondary OAM with sprite to be rendered on the next scanline
 *
 * \param self instance of PPU
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t PPU_SpriteEvaluation(PPU *self);

/**
 * \brief Fill internal shift registers for sprites rendering of next scanline
 *
 * \param self instance of PPU
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t PPU_FetchSprite(PPU *self);

/**
 * \brief Draw at a specific pixel directed by scanline and cycle counter
 *
 * \param self instance of PPU
 *
 * \return EXIT_SUCCESS if succeed, EXIT_FAILURE otherwise
 */
uint8_t PPU_Draw(PPU *self);

/**
 * \brief Destroy instance of PPU
 *
 * \param self instance of PPU
 */
void PPU_Destroy(PPU *self);

/**
 * \brief State for Sprite Evaluation algorithm
 */
enum StateSpriteEvaluation {
	STATE_COPY_Y = 0,			/*!< Y-coordonate copy from OAM to SOAM */
	STATE_COPY_REMAINING,		/*!< Copy remaining 3 byte from OAM to SOAM */
	STATE_OVERFLOW,				/*!< 8 sprites has been written in SOAM */
	STATE_OVERFLOW_REMAINING,	/*!< Sprite overflow has occured */
	STATE_WAIT,					/*!< All sprites has been evaluated */
};


#endif /* PPU_H */
