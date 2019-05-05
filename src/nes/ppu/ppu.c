#include "ppu.h"
#include "../../common/macro.h"
#include "../mapper/ioreg.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define IS_RENDERING_ON() ((self->PPUMASK & 0x18) != 0)

static uint32_t colorPalette[64] = {
	0x007C7C7C, 0x000000FC, 0x000000BC, 0x004428BC,
	0x00940084, 0x00A80020, 0x00A81000, 0x00881400,
	0x00503000, 0x00007800, 0x00006800, 0x00005800,
	0x00004058, 0x00000000, 0x00000000, 0x00000000,
	0x00BCBCBC, 0x000078F8, 0x000058F8, 0x006844FC,
	0x00D800CC, 0x00E40058, 0x00F83800, 0x00E45C10,
	0x00AC7C00, 0x0000B800, 0x0000A800, 0x0000A844,
	0x00008888, 0x00000000, 0x00000000, 0x00000000,
	0x00F8F8F8, 0x003CBCFC, 0x006888FC, 0x009878F8,
	0x00F878F8, 0x00F85898, 0x00F87858, 0x00FCA044,
	0x00F8B800, 0x00B8F818, 0x0058D854, 0x0058F898,
	0x0000E8D8, 0x00787878, 0x00000000, 0x00000000,
	0x00FCFCFC, 0x00A4E4FC, 0x00B8B8F8, 0x00D8B8F8,
	0x00F8B8F8, 0x00F8A4C0, 0x00F0D0B0, 0x00FCE0A8,
	0x00F8D878, 0x00D8F878, 0x00B8F8B8, 0x00B8F8D8,
	0x0000FCFC, 0x00F8D8F8, 0x00000000, 0x00000000
};

static unsigned char reverse_byte(unsigned char x) {
	static const unsigned char table[] = {
		0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
		0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
		0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
		0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
		0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
		0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
		0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
		0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
		0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
		0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
		0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
		0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
		0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
		0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
		0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
		0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
		0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
		0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
		0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
		0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
		0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
		0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
		0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
		0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
		0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
		0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
		0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
		0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
		0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
		0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
		0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
		0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
	};
	return table[x];
}

PPU* PPU_Create(Mapper *mapper) {
	/* Allocate PPU structure */
	PPU *self = (PPU*) malloc(sizeof(PPU));
	if (self == NULL) {
		ERROR_MSG("can't allocate memory for PPU structure");
		return NULL;
	}

	self->image = (uint32_t*) malloc(256 * 240 * sizeof(uint32_t));
	if (self->image == NULL) {
		ERROR_MSG("can't allocate memory for graphics array in PPU");
		PPU_Destroy(self);
		return NULL;
	}

	/* Connect mapper to PPU */
	self->mapper = mapper;
	return self;
}

uint8_t PPU_Init(PPU *self) {
	int i;

	/* Init registers */
	self->scanline = -1;
	self->cycle = 0;
	self->vram.w = 0;
	self->vram.x = 0;
	self->vram.v = 0;
	self->vram.t = 0;
	self->PPUADDR = 0;
	self->PPUDATA = 0;
	self->PPUCTRL = 0;
	self->PPUMASK = 0;
	self->PPUSTATUS = 0;
	self->OAMADDR = 0;
	self->OAMDATA = 0;
	self->PPUSCROLL = 0;
	self->nbFrame = 0;
	self->nmiSent = 0;
	self->pictureDrawn = 0;

	for (i = 0; i < 256; i++)
		self->OAM[i] = 0;

	return EXIT_SUCCESS;
}

char* RenderColorPalette(void) {
	/* Copy Color Palette to a new array */
	uint32_t *tab = (uint32_t*) malloc(64 * sizeof(uint32_t));
	memcpy(tab, colorPalette, 64 * sizeof(uint32_t));
	return (char*) tab;
}

void PPU_RenderNametable(PPU *self, uint32_t *image, uint8_t index) {
	uint8_t *nametable = Mapper_Get(self->mapper, AS_PPU,
			0x2000 | (index << 10));
	uint8_t *attribute = nametable + 0x3C0;
	uint8_t *palette = Mapper_Get(self->mapper, AS_PPU, 0x3F00);
	uint8_t *pattern = Mapper_Get(self->mapper, AS_LDR, LDR_CHR) +
		((self->PPUCTRL & 0x10) ? 0x1000 : 0);
	uint8_t *tile = NULL, *color = NULL;
	uint8_t temp = 0, shift = 0;
	uint32_t index_image = 0;
	int x, y, i, j;

	for (y = 0; y < 30; y++) {
		for (x = 0; x < 32; x++) {
			/* Tile to get pixels data from */
			tile = pattern + (nametable[x + (y << 5)] << 4);
			/* Retrieve attribute and decode */
			temp = attribute[(x >> 2) + ((y & 0xFC) << 1)];
			shift = (x & 0x02) | ((y & 0x02) << 1);
			temp = (temp >> shift) & 0x03;
			/* Retrive palette color */
			color = palette + (temp << 2);
			/* Draw on screen */
			for (j = 0; j < 8; j++) {
				for (i = 0; i < 8; i++) {
					temp = (reverse_byte(tile[j]) >> i) & 0x01;
					temp |= ((reverse_byte(tile[j | 0x08 ]) >> i) & 0x01) << 1;
					index_image = (x << 3) + (y << 11) + (j << 8) + i;
					image[index_image] = colorPalette[color[temp]];
				}
			}
		}
	}
}

void PPU_RenderSprites(PPU *self, uint32_t *image) {

	if ((self->PPUMASK & 0x10) == 0)
		return;

	uint8_t *palette = Mapper_Get(self->mapper, AS_PPU, 0x3F00);
	uint8_t *pattern = Mapper_Get(self->mapper, AS_LDR, LDR_CHR) +
		((self->PPUCTRL & 0x08) ? 0x1000 : 0);
	uint8_t *tile = NULL, *color = NULL;
	uint8_t temp = 0;
	int oamaddr, x, y, x_start, y_start, attribute;
	uint32_t index_image = 0;

	/* Evaluate sprite per sprite */
	for (oamaddr = 0; oamaddr < 0x100; oamaddr += 0x4) {
		/* Tile to get pixels data from */
		tile = pattern + (self->OAM[oamaddr + 1] << 4);
		x_start = self->OAM[oamaddr + 3];
		y_start = self->OAM[oamaddr] + 1;
		attribute = self->OAM[oamaddr + 2];
		color = palette + ((attribute & 0x03) << 2) + 0x10;
		for (y = 0; y < 8; y++) {
			for (x = 0; x < 8; x++) {
				/* Flip both orientation */
				if ((attribute & 0xC0) == 0xC0) {
					temp = (tile[7-y] >> x) & 0x01;
					temp |= ((tile[(7-y) | 0x08 ] >> x) & 0x01) << 1;
					/* Flip vertical */
				} else if ((attribute & 0xC0) == 0x80) {
					temp = (reverse_byte(tile[7-y]) >> x) & 0x01;
					temp |= ((reverse_byte(tile[(7-y) | 0x08 ]) >> x) & 0x01) << 1;
					/* Flip horizontal */
				} else if ((attribute & 0xC0) == 0x40) {
					temp = (tile[y] >> x) & 0x01;
					temp |= ((tile[y | 0x08 ] >> x) & 0x01) << 1;
					/* Don't flip */
				} else {
					temp = (reverse_byte(tile[y]) >> x) & 0x01;
					temp |= ((reverse_byte(tile[y | 0x08 ]) >> x) & 0x01) << 1;
				}
				index_image = x_start + (y_start << 8) + (y << 8) + x;
				if ((temp & 0x3) != 0)
					image[index_image] = colorPalette[color[temp]];
			}
		}
	}
}

uint8_t PPU_CheckRegister(PPU *self) {
	uint8_t ack = 0;

	/* PPUCTRL behavior:
	 * Write	->	Update VRAM.t with nametable content */
	if (Mapper_Ack(self->mapper, 0x2000) & AC_WR) {
		/* t: ...BA.. ........ = d: ......BA */
		self->vram.t &= ~0x0C00;
		self->vram.t |= (self->PPUCTRL & 0x03) << 10;
		return EXIT_SUCCESS;
	}

	/* PPUSTATUS behavior:
	 * Read		->	Clear Vertical Blank Bit and VRAM.w latch */
	if (Mapper_Ack(self->mapper, 0x2002)) {
		self->PPUSTATUS &= ~0x80;
		self->vram.w = 0;
		return EXIT_SUCCESS;
	}

	/* OAMDATA behavior:
	 * Write	->	Update OAM[OAMADDR] with given value and inc OAMADDR */
	if (Mapper_Ack(self->mapper, 0x2004) & AC_WR) {
		self->OAM[self->OAMADDR++] = self->OAMDATA;
		return EXIT_SUCCESS;
	}

	/* PPUSCROLL behavior:
	 * 2 Write	->	VRAM.w++ and update VRAM.t */
	if (Mapper_Ack(self->mapper, 0x2005) & AC_WR) {
		if (self->vram.w == 0) {
			/* t: ....... ...HGFED = d: HGFED... */
			self->vram.t &= ~0x001F;
			self->vram.t |= self->PPUSCROLL >> 3;
			/* x:              CBA = d: .....CBA */
			self->vram.x = self->PPUSCROLL & 0x07;
			self->vram.w++;
		} else {
			/* t: CBA..HG FED..... = d: HGFEDCBA */
			self->vram.t &= ~0xE3E0;
			self->vram.t |= (self->PPUSCROLL << 12) & 0x7FFF;
			self->vram.t |= (self->PPUSCROLL & 0xF8) << 2;
			self->vram.w = 0;
		}
		return EXIT_SUCCESS;
	}

	/* PPUADDR behavior:
	 * 2 Write	->	VRAM.w++ and update VRAM.t */
	if (Mapper_Ack(self->mapper, 0x2006) & AC_WR) {
		if (self->vram.w == 0) {
			/* t: 0FEDCBA ........ = d: ..FEDCBA */
			self->vram.t &= ~0x7F00;
			self->vram.t |= (self->PPUADDR << 8) & 0x7F00;
			self->vram.w++;
		} else {
			/* t: ....... HGFEDCBA = d: HGFEDCBA */
			self->vram.t &= ~0x00FF;
			self->vram.t |= self->PPUADDR;
			/* v = t */
			self->vram.v = self->vram.t;
			self->vram.w = 0;
		}
		return EXIT_SUCCESS;
	}

	/* PPUDATA behavior:
	 * R/W		->	Update Mapper[VRAM] with given value and update VRAM.v */
	if ((ack = Mapper_Ack(self->mapper, 0x2007))) {

		uint8_t *vram = Mapper_Get(self->mapper, AS_PPU, self->vram.v);
		/* Set value in VRAM correspondly to value of PPUDATA */
		if (ack & AC_WR) {
			/* If write occurs in palette color universal value */
			if ((self->vram.v == 0x3F10) && ((self->vram.v & 0x3) == 0)) {
				int i = 0;
				uint8_t* palette = Mapper_Get(self->mapper, AS_PPU, 0x3F00);
				for (i = 0; i < 8; i++) { 
					*(palette + (i << 2)) = self->PPUDATA;
				}
			} else {
				*vram = self->PPUDATA;
			}
		}
		/* Place in PPUDATA the desired data from VRAM */
		else if (ack & AC_RD)
			self->PPUDATA = *vram;

		/* Depending of the self->spriteState of rendering,
		 * increment is acting differently */
		if (VALUE_IN(self->scanline, -1, 239) && IS_RENDERING_ON()) {
			PPU_IncrementCorseX(self);
			PPU_IncrementY(self);
			/* If not rendering, increment linearly */
		} else {
			self->vram.v += (self->PPUCTRL & 0x04) ? 32 : 1;
		}
		return EXIT_SUCCESS;
	}

	return EXIT_SUCCESS;
}

uint8_t PPU_ManageTiming(PPU *self, Stack *taskList) {

	/* Pre-render and visible scanlines */
	if (VALUE_IN(self->scanline, -1, 239)) {
		/* Skip cycle if odd frame and rendering enable */
		if ((self->scanline == 0) && (self->cycle == 0)) {
			if ((self->nbFrame % 2) && IS_RENDERING_ON()) {
				self->cycle++;
			}
			self->nbFrame++;
		}
		/* Visible dot part */
		if (VALUE_IN(self->cycle, 1, 256)) {
			/* Increment hori(v) every 8's clock */
			if (((self->cycle % 8) == 0) && IS_RENDERING_ON()) {
				Stack_Push(taskList, (void*) PPU_ManageV);
			}
			/* Clear Vertical Blank flag and Sprite 0 hit
			 * when we are in the second cycle of pre-render line */
			if ((self->scanline == -1) && (self->cycle == 1)) {
				Stack_Push(taskList, (void*) PPU_ClearFlag);
			}
			/* Clean Secondary OAM */
			if (VALUE_IN(self->cycle, 1, 64) && IS_RENDERING_ON()) {
				Stack_Push(taskList, (void*) PPU_ClearSecondaryOAM);
				/* Sprite Evaluation */
			} else if (VALUE_IN(self->cycle, 65, 256) && IS_RENDERING_ON()) {
				Stack_Push(taskList, (void*) PPU_SpriteEvaluation);
			}
			/* Fetch Tile at every cycle */
			if (IS_RENDERING_ON())
				Stack_Push(taskList, (void*) PPU_FetchTile);
			/* Draw pixel */
			if (VALUE_IN(self->scanline, 0, 239) && IS_RENDERING_ON())
				Stack_Push(taskList, (void*) PPU_Draw);
		} else if (VALUE_IN(self->cycle, 257, 320) && IS_RENDERING_ON()) {
			/* Affect hori(t) to hori(v)
			 * or vert(t) to vert(v) */
			if ((self->cycle == 257) ||
					(VALUE_IN(self->cycle, 280, 304) && (self->scanline == -1))) {
				Stack_Push(taskList, (void*) PPU_ManageV);
			}
			/* Fetch Sprite every 8's clock cycle */
			if (((self->cycle - 256) % 8) == 0)
				Stack_Push(taskList, (void*) PPU_FetchSprite);
			/* Fetch Tile for next scanline */
		} else if (VALUE_IN(self->cycle, 321, 336) && IS_RENDERING_ON()) {
			/* Increment hori(v) every 8's clock cycle */
			if ((self->cycle % 8) == 0) {
				Stack_Push(taskList, (void*) PPU_ManageV);
			}
			/* Fetch tile at every cycle */
			Stack_Push(taskList, (void*) PPU_FetchTile);
		}
		/* Set Vertical Blank flag */
	} else if ((self->scanline == 241) && (self->cycle == 1)) {
		Stack_Push(taskList, (void*) PPU_SetFlag);
	}

	return EXIT_SUCCESS;
}

uint8_t PPU_SetFlag(PPU *self) {
	/* Set Vertical Blank bit */
	self->PPUSTATUS |= 0x80;
	self->pictureDrawn = 1;
	return EXIT_SUCCESS;
}

uint8_t PPU_ClearFlag(PPU *self) {
	/* Clear Vertical Blank, Sprite 0 and Sprite Overflow bits */
	self->PPUSTATUS &= ~0xE0;
	self->nmiSent = 0;
	return EXIT_SUCCESS;
}

uint8_t PPU_IncrementCorseX(PPU *self) {
	/* If VRAM.v Corse X is going to overflow, switch nametable */
	if ((self->vram.v & 0x001F) == 31) {
		self->vram.v &= ~0x001F;
		self->vram.v ^= 0x0400;
		/* Else increment Corse X */
	} else
		self->vram.v++;
	return EXIT_SUCCESS;
}

uint8_t PPU_IncrementY(PPU *self) {
	/* If Fine Y is not going to overflow, increment it */
	if ((self->vram.v & 0x7000) != 0x7000) {
		self->vram.v += 0x1000;
		/* Else manage Corse Y component */
	} else {
		/* Reset Fine Y component because it has overflowed */
		self->vram.v &= ~0x7000;
		/* var y correspond to Corse Y component */
		int y = (self->vram.v & 0x03E0) >> 5;
		/* If Corse Y achieved the last tile, reset it and switch nametable */
		if (y == 29) {
			y = 0;
			self->vram.v ^= 0x0800;
			/* If Corse Y is going to overflow, reset it and do not switch NT */
		} else if (y == 31)
			y = 0;
		/* Else increment Corse Y component */
		else
			y++;
		/* Insert y into VRAM.v */
		self->vram.v = (self->vram.v & ~0x03E0) | (y << 5);
	}
	return EXIT_SUCCESS;
}

uint8_t PPU_ManageV(PPU *self) {
	/* Dot 8 to 248 (every 8) : Increment Corse X */
	if (VALUE_IN(self->cycle, 8, 248)) {
		PPU_IncrementCorseX(self);
		/* Dot 256 : Increment Y */
	} else if (self->cycle == 256) {
		PPU_IncrementY(self);
		/* Dot 257 : Horizontal component of t set to v */
	} else if (self->cycle == 257) {
		/* hori(v) = hori(t) */
		self->vram.v &= ~0x041F;
		self->vram.v |= self->vram.t & 0x041F;
		/* Dot 280 to 304 : Vertical component of t set to v */
	} else if (VALUE_IN(self->cycle, 280, 304)) {
		/* vert(v) = vert(t) */
		self->vram.v &= ~0x7BE0;
		self->vram.v |= self->vram.t & 0x7BE0;
	} else if (VALUE_IN(self->cycle, 328, 336)) {
		PPU_IncrementCorseX(self);
	}
	return EXIT_SUCCESS;
}

uint8_t PPU_RefreshRegister(PPU *self, uint8_t *context) {

	/* OAMDATA read behavior */
	self->OAMDATA = self->OAM[self->OAMADDR];

	/* PPUDATA read behavior when address looking at Color Palette */
	if (VALUE_IN(self->vram.v, 0x3F00, 0x3FFF)) {
		self->PPUDATA = *Mapper_Get(self->mapper, AS_PPU, self->vram.v);
	}

	/* NMI Interrupt Generator */
	if (!self->nmiSent && ((self->PPUSTATUS & 0x80) != 0) &&
			((self->PPUCTRL & 0x80) != 0)) {
		*context |= 0x02;
		self->nmiSent = 1;
	}

	return EXIT_SUCCESS;
}

uint8_t PPU_ClearSecondaryOAM(PPU *self) {
	/* Clear Secondary OAM progressively */
	if (!(self->cycle % 2)) {
		self->SOAM[(self->cycle - 1) >> 1] = 0xFF;
	}
	return EXIT_SUCCESS;
}

uint8_t PPU_SpriteEvaluation(PPU *self) {
	/* Is it cycle 65? Init variable then */
	if (self->cycle == 65) {
		self->OAMADDR = self->SOAMADDR = 0;
		self->spriteState = STATE_COPY_Y;
	}

	/* Read or write cycle? */
	if ((self->cycle & 1) == 1) {
		self->spriteData = self->OAM[self->OAMADDR];
	} else {
		switch (self->spriteState) {
			/*  Copy Y coordonate in Secondary OAM */
			case STATE_COPY_Y:
				/* Copy Y value to secondary OAM */
				self->SOAM[self->SOAMADDR] = self->spriteData;
				/* Test if scanline correspond to Y range */
				if (((self->scanline + 1) >= self->spriteData) &&
						((self->scanline + 1) < (self->spriteData + 8))) {
					self->spriteState = STATE_COPY_REMAINING;
					/* Increment primary and secondary index */
					self->SOAMADDR = 0x1F & (self->SOAMADDR + 1);
					self->OAMADDR++;
				} else {
					/* Go to the next sprite in OAM */
					self->OAMADDR += 0x04;
					/* All 64 sprites has been evaluated? */
					if (self->OAMADDR == 0) {
						self->spriteState = STATE_WAIT;
					}
				}
				break;
				/* Copy the three remaining byte from POAM to SOAM */
			case STATE_COPY_REMAINING:
				/* Copy value of primary OAM to secondary OAM */
				self->SOAM[self->SOAMADDR] = self->spriteData;
				/* Increment primary and secondary index */
				self->SOAMADDR = 0x1F & (self->SOAMADDR + 1);
				self->OAMADDR++;
				/* If we have copied all 3 bytes */
				if ((self->SOAMADDR & 0x3) == 0) {
					/* If all sprite in OAM has been evaluated */
					if (self->OAMADDR == 0) {
						self->spriteState = STATE_WAIT;
						/* If there is no more space in secondary OAM */
					} else if (self->SOAMADDR == 0) {
						self->spriteState = STATE_OVERFLOW;
						/* If there is enough space for more sprites in SOAM */
					} else {
						self->spriteState = STATE_COPY_Y;
					}
				}
				break;
				/* No more space in Secondary OAM */
			case STATE_OVERFLOW:
				/* If sprite evaluated is in range, signal overflow */
				if (((self->scanline + 1) >= self->spriteData) &&
						((self->scanline + 1) < (self->spriteData + 8))) {
					/* Set Sprite Overflow bit to one */
					self->PPUSTATUS |= 0x20;
					/* Increment primary and secondary index */
					self->SOAMADDR = 0x1F & (self->SOAMADDR + 1);
					self->OAMADDR++;
					self->spriteState = STATE_OVERFLOW_REMAINING;
				} else {
					self->OAMADDR += ((self->OAMADDR & 0x3) == 3) ? 1 : 5;
					if (self->OAMADDR & 0xFC) {
						self->spriteState = STATE_OVERFLOW;
					} else {
						self->spriteState = STATE_WAIT;
					}
				}
				break;
				/* Increment three times */
			case STATE_OVERFLOW_REMAINING:
				/* Increment primary and secondary index */
				self->OAMADDR++;
				if ((self->OAMADDR & 0x3) == 0) {
					self->spriteState = STATE_WAIT;
				}
				break;
			/* Wait for the end of sprite evaluation */
			case STATE_WAIT:
				self->OAMADDR += 0x04;
				break;
			default:
				self->spriteState = STATE_COPY_Y;
		}
	}

	return EXIT_SUCCESS;
}

uint8_t PPU_FetchTile(PPU *self) {

	uint8_t* pattern_address = Mapper_Get(self->mapper, AS_PPU, 0x2000
			| (self->vram.v & 0x0FFF));
	uint8_t* attribute = Mapper_Get(self->mapper, AS_PPU,0x23C0
			| (self->vram.v & 0x0C00)
			| ((self->vram.v >> 4) & 0x38)
			| ((self->vram.v >> 2) & 0x07));
	uint8_t* pattern = Mapper_Get(self->mapper, AS_LDR, LDR_CHR) +
		((self->PPUCTRL & 0x10) ? 0x1000 : 0);

	uint8_t shift, attribute_value = *attribute;
	/* coarse X */
	uint8_t x = self->vram.v & 0x01F;
	/* coarse Y */
	uint8_t y = (self->vram.v >> 5) & 0x1F;
	/* fine y */
	uint8_t fine_y = self->vram.v>>12;

	uint8_t* tile_pattern = pattern + ( *pattern_address << 4);

	/* shift the attribute and bitmap registers */
	self->bitmapL <<= 0x01;
	self->bitmapH <<= 0x01;
	self->attributeL <<= 0x01;
	self->attributeH <<= 0x01;


	if((self->cycle % 8) == 0) {
		/* insert data from pattern table into shift registers */
		self->bitmapL &=~ 0x00FF;
		self->bitmapL |= tile_pattern[fine_y];

		self->bitmapH &=~ 0x00FF;
		self->bitmapH |= tile_pattern[fine_y | 0x08 ];

		/* get and decode attribute */
		shift = (x & 0x02) | ((y & 0x02) << 1);
		attribute_value = (attribute_value >> shift) & 0x03;

		/* set all the byte's bits to the value of the attribute bit */
		self->attributeL &=~ 0x00FF;
		self->attributeL |= ((attribute_value & 0x01)? 0x00FF : 0);

		self->attributeH &=~ 0x00FF;
		self->attributeH |= (((attribute_value >> 0x01) & 0x01)? 0x00FF : 0);
	}
	return EXIT_SUCCESS;
}

uint8_t PPU_FetchSprite(PPU *self) {
	uint8_t *pattern = Mapper_Get(self->mapper, AS_LDR, LDR_CHR) +
		((self->PPUCTRL & 0x08) ? 0x1000 : 0);
	uint8_t *tile = NULL;
	uint8_t y, index, soamIndex;

	/* Which sprite number in SOAM are we going to process? */
	index = ((self->cycle - 257) & 0xF8) >> 3;
	soamIndex = ((self->cycle - 257) & 0xF8) >> 1;
	/* Empty slot? */
	if ((self->SOAM[soamIndex + 1] == 0xFF) &&
		(self->SOAM[soamIndex + 2] == 0xFF) &&
		(self->SOAM[soamIndex + 3] == 0xFF)) {
		/* Set to transparency */
		self->sprite[index].patternL = 0x00;
		self->sprite[index].patternH = 0x00;
		self->sprite[index].attribute = 0x00;
		self->sprite[index].x = 0xFF;
		/* Used slot? */
	} else {
		/* Compute Fine Y coordonate */
		y = self->scanline - self->SOAM[soamIndex] + 1;
		/* Retrieve corresponding pattern address */
		tile = pattern + (self->SOAM[soamIndex + 1] << 4);
		/* Copy attributes and X coordonate */
		self->sprite[index].attribute = self->SOAM[soamIndex + 2];
		self->sprite[index].x = self->SOAM[soamIndex + 3];
		/* Flip both orientation */
		if ((self->sprite[index].attribute & 0xC0) == 0xC0) {
			self->sprite[index].patternL = reverse_byte(tile[7-y]);
			self->sprite[index].patternH = reverse_byte(tile[(7-y) | 0x08]);
			/* Flip vertical */
		} else if ((self->sprite[index].attribute & 0xC0) == 0x80) {
			self->sprite[index].patternL = tile[7-y];
			self->sprite[index].patternH = tile[(7-y) | 0x08];
			/* Flip horizontal */
		} else if ((self->sprite[index].attribute & 0xC0) == 0x40) {
			self->sprite[index].patternL = reverse_byte(tile[y]);
			self->sprite[index].patternH = reverse_byte(tile[y | 0x08]);
			/* Don't flip */
		} else {
			self->sprite[index].patternL = tile[y];
			self->sprite[index].patternH = tile[y | 0x08];
		}
	}

	return EXIT_SUCCESS;
}


uint8_t PPU_Draw(PPU *self) {
	/* variables used for background */
	uint8_t *palette = Mapper_Get(self->mapper, AS_PPU, 0x3F00);

	uint8_t attribute = (self->attributeL & (0x8000 >> self->vram.x)) >> (15 - self->vram.x)
						| (self->attributeH & (0x8000 >> self->vram.x)) >> (14 - self->vram.x);

	uint8_t bitmap = (self->bitmapL & (0x8000 >> self->vram.x)) >> (15 - self->vram.x)
					| (self->bitmapH & (0x8000 >> self->vram.x)) >> (14 - self->vram.x);

	uint8_t color;
	uint8_t* color_palette = palette + (attribute << 2); /* color palette address */

	/* variables used for sprites*/

	uint8_t sprite_mux_is_empty = 1;
	uint8_t sprite_pixel_color;
	Sprite sprite_mux;

	uint8_t i;

	for (i = 0; i < 8; i++) {
		if(self->sprite[i].x) {
			/* decrement x counter for each sprite until it reaches 0 */
			self->sprite[i].x--;
		} else {
			/* at x = 0, the sprite is active and needs to be printed */

			sprite_pixel_color = ((self->sprite[i].patternH >> 6) & 0x02)
								| ((self->sprite[i].patternL >> 7) & 0x01);

			if ((i == 0) && (sprite_pixel_color != 0) && (bitmap != 0)) {
				if((((self->PPUMASK >> 3) & 0x03) == 0x03)
					&& !(((self->sprite[i].x >= 0) && (self->sprite[i].x <= 7)) && (((self->PPUMASK >> 1) & 0x03) != 0x03))
					&& (self->sprite[i].x != 255)
					&& !((self->PPUSTATUS >> 6) & 0x01)) {

					self->PPUSTATUS |= 0x40;
				}
			}

			/* the first non transparent pixel has to be multiplexed */
			if((sprite_pixel_color != 0) && sprite_mux_is_empty) {

				/*set this bit only in multiplexer */
				sprite_mux = self->sprite[i];
				sprite_mux_is_empty = 0;

			}
			self->sprite[i].patternH <<= 1;
			self->sprite[i].patternL <<= 1;
		}
	}

	/* if the sprite pixel has priority over background (0) or BG pixel is zero */
	if ((!((sprite_mux.attribute >> 5) & 0x01) || ((bitmap & 0x3) == 0))
			&& !sprite_mux_is_empty) {
		/* display the sprite */
		color_palette = palette + ((sprite_mux.attribute & 0x03) << 2) + 0x10;

		color = ((sprite_mux.patternH >> 6) & 0x02)
				| ((sprite_mux.patternL >> 7) & 0x01);
	} else {
		/* color_palette = palette + (attribute << 2); */
		color = bitmap;
	}

	self->image[(self->scanline << 8) + self->cycle-1] = colorPalette[color_palette[color]];

	return EXIT_SUCCESS;
}


uint8_t PPU_UpdateCycle(PPU *self) {
	/* Increment cycle and scanline if it has overflow */
	if (self->cycle < 340)
		self->cycle++;
	else {
		self->cycle = 0;
		if (self->scanline < 261)
			self->scanline++;
		else
			self->scanline = -1;
	}
	return EXIT_SUCCESS;
}

uint8_t PPU_Execute(PPU* self, uint8_t *context, uint8_t clock) {
	Stack taskList;
	uint8_t (*task)(PPU*) = NULL;

	Stack_Init(&taskList);
	PPU_CheckRegister(self);
	while (clock) {
		PPU_ManageTiming(self, &taskList);
		while (!Stack_IsEmpty(&taskList)) {
			/* Execute task */
			task = Stack_Pop(&taskList);
			task(self);
		}
		clock--;
		PPU_UpdateCycle(self);
	}
	PPU_RefreshRegister(self, context);

	return EXIT_SUCCESS;
}

uint8_t PPU_PictureDrawn(PPU *self) {
	/* Retrieve information and acknowledge it */
	uint8_t result = self->pictureDrawn;
	self->pictureDrawn = 0;
	return result;
}

void PPU_Destroy(PPU *self) {
	if (self == NULL)
		return;
	if (self->image != NULL)
		free(self->image);
	free(self);
}
