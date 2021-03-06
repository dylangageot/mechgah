/**
 * \file const.h
 * \brief header file of constants for NES emulation
 * \author Dylan Gageot
 * \version 1.0
 * \date 2019-05-11
 */
#ifndef CONST_H
#define CONST_H

/* Screen specification */
#define NES_SCREEN_WIDTH			256
#define NES_SCREEN_HEIGTH			240

/* IO Register address */
#define ADDR_PPUCTRL				0x2000
#define ADDR_PPUMASK				0x2001
#define ADDR_PPUSTATUS				0x2002
#define ADDR_OAMADDR				0x2003
#define ADDR_OAMDATA				0x2004
#define ADDR_PPUSCROLL				0x2005
#define ADDR_PPUADDR				0x2006
#define ADDR_PPUDATA				0x2007
#define ADDR_SQ1_VOL				0x4000
#define ADDR_SQ1_SWEEP				0x4001	
#define ADDR_SQ1_LO					0x4002
#define ADDR_SQ1_HI					0x4003
#define ADDR_SQ2_VOL				0x4004
#define ADDR_SQ2_SWEEP				0x4005
#define ADDR_SQ2_LO					0x4006
#define ADDR_TRI_LINEAR				0x4008
#define ADDR_TRI_LO					0x400A
#define ADDR_TRI_HI					0x400B
#define ADDR_NOISE_VOL				0x400C
#define ADDR_NOISE_LO				0x400E
#define ADDR_NOISE_HI				0x400F
#define ADDR_DMC_FREQ				0x4010
#define ADDR_DMC_RAW				0x4011
#define ADDR_DMC_START				0x4012
#define ADDR_DMC_LEN				0x4013
#define ADDR_OAMDMA					0x4014
#define ADDR_SND_CHN				0x4015
#define ADDR_JOY1					0x4016
#define ADDR_JOY2					0x4017

/* CPU definition */
/* Status Register */
#define P_CARRY						0x01
#define	P_ZERO						0x02
#define P_INTERRUPT					0x04
#define P_DECIMAL					0x08
#define P_BRK						0x10
#define P_OVERFLOW					0x40
#define P_SIGN						0x80
/* CPU Address space */
#define ADDR_STACK					0x0100

/* PPU definition */
#define	SIZE_OAM					256
#define	SIZE_SOAM					32
#define SIZE_COLOR_PALETTE			64
#define SIZE_NAMETABLE				0x03C0
#define SIZE_ATTRIBUTE				0x0040
#define SIZE_PATTERN				0x1000
#define SIZE_PALETTE				0x0010
#define SIZE_TILE_LAYER				8
#define SIZE_TILE					16
#define	PRERENDER_SCANLINE			-1
#define SIZE_TILE_PIXEL				8
#define	TILE_X_CNT					(NES_SCREEN_WIDTH/SIZE_TILE_PIXEL)	
#define	TILE_Y_CNT					(NES_SCREEN_HEIGTH/SIZE_TILE_PIXEL)
#define SPR_SOAM_CNT				8
/* PPUCTRL */
#define PPUCTRL_BASE_NT				0x03
#define PPUCTRL_VRAM_INC			0x04
#define PPUCTRL_SPR_PT				0x08
#define PPUCTRL_BG_PT				0x10
#define PPUCTRL_SPR_SIZE			0x20
#define PPUCTRL_MASTER				0x40
#define PPUCTRL_NMI					0x80
/* PPUMASK */
#define PPUMASK_GREY				0x01
#define PPUMASK_SHOW_BG_8			0x02
#define PPUMASK_SHOW_SPR_8			0x04
#define PPUMASK_SHOW_BG				0x08
#define PPUMASK_SHOW_SPR			0x10
#define PPUMASK_EMPH_RED			0x20
#define PPUMASK_SHOW_GRN			0x40
#define PPUMASK_SHOW_BLU			0x80
/* PPUSTATUS */
#define PPUSTATUS_SPR_OVF			0x20
#define PPUSTATUS_SPR_ZERO			0x40
#define PPUSTATUS_VBL				0x80
/* OAM index */
#define INDEX_OAM_Y_COORD			0
#define INDEX_OAM_TILE				1
#define INDEX_OAM_ATTRIBUTE			2
#define INDEX_OAM_X_COORD			3
/* OAM Attribute */
#define OAM_ATTRIBUTE_PALETTE		0x03
#define OAM_ATTRIBUTE_PRIOTIY		0x20
#define OAM_ATTRIBUTE_FLIP_H		0x40
#define OAM_ATTRIBUTE_FLIP_V		0x80
#define OAM_ATTRIBUTE_FLIP			0xC0
/* PPU Address space */
#define	ADDR_PATTERN_1				0x0000
#define	ADDR_PATTERN_2				(ADDR_PATTERN_1 + SIZE_PATTERN)
#define	ADDR_NAMETABLE_1			(ADDR_PATTERN_2 + SIZE_PATTERN)
#define ADDR_ATTRIBUTE_1			(ADDR_NAMETABLE_1 + SIZE_NAMETABLE)
#define	ADDR_NAMETABLE_2			(ADDR_ATTRIBUTE_1 + SIZE_ATTRIBUTE)
#define ADDR_ATTRIBUTE_2			(ADDR_NAMETABLE_2 + SIZE_NAMETABLE)
#define	ADDR_NAMETABLE_3			(ADDR_ATTRIBUTE_2 + SIZE_ATTRIBUTE)
#define ADDR_ATTRIBUTE_3			(ADDR_NAMETABLE_3 + SIZE_NAMETABLE)
#define	ADDR_NAMETABLE_4			(ADDR_ATTRIBUTE_3 + SIZE_ATTRIBUTE)
#define ADDR_ATTRIBUTE_4			(ADDR_NAMETABLE_4 + SIZE_NAMETABLE)
#define	ADDR_PALETTE_BG				0x3F00
#define	ADDR_PALETTE_SPR			(ADDR_PALETTE_BG + SIZE_PALETTE)

#endif /* CONST_H */
