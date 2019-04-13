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

PPU* PPU_Create(Mapper *mapper) {
	/* Allocate PPU structure */
	PPU *self = (PPU*) malloc(sizeof(PPU));
	if (self == NULL) {
		ERROR_MSG("can't allocate memory for PPU structure");
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
			self->vram.t &= 0x001F;
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
		if (ack & AC_WR)
			*vram = self->PPUDATA;
		/* Place in PPUDATA the desired data from VRAM */
		else if (ack & AC_RD)
			self->PPUDATA = *vram;

		/* Depending of the state of rendering, 
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
			/* Draw pixel */
			if (VALUE_IN(self->scanline, 0, 239) && IS_RENDERING_ON())
				Stack_Push(taskList, (void*) PPU_Draw);
			/* Fetch Tile at every cycle */
			if (IS_RENDERING_ON())
				Stack_Push(taskList, (void*) PPU_FetchTile);
		/* Fetch Sprite part */
		} else if (VALUE_IN(self->cycle, 257, 320) && IS_RENDERING_ON()) { 
			/* Affect hori(t) to hori(v) 
			 * or vert(t) to vert(v) */
			if ((self->cycle == 257) || 
				(VALUE_IN(self->cycle, 280, 304) && (self->scanline == -1))) {
				Stack_Push(taskList, (void*) PPU_ManageV);
			}
			/* Fetch Sprite at every cycle */
			Stack_Push(taskList, (void*) PPU_FetchSprite);
		/* Fetch Tile for next scanline */
		} else if (VALUE_IN(self->cycle, 321, 336) && IS_RENDERING_ON()) {
			/* Increment hori(v) every 8's clock */
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
	if (!self->nmiSent && ((self->PPUSTATUS & 0x80) != 0)) {
		*context |= 0x02;
		self->nmiSent = 1;
	}
	
	return EXIT_SUCCESS;
}

uint8_t PPU_ClearSecondaryOAM(PPU *self) { return EXIT_SUCCESS; }
uint8_t PPU_FetchTile(PPU *self) { return EXIT_SUCCESS; }
uint8_t PPU_FetchSprite(PPU *self) { return EXIT_SUCCESS; }
uint8_t PPU_SpriteEvaluation(PPU *self) { return EXIT_SUCCESS; }
uint8_t PPU_Draw(PPU *self) { return EXIT_SUCCESS; }


uint8_t PPU_Execute(PPU* self, uint8_t *context, uint8_t clock) {
	Stack taskList;
	uint8_t (*task)(PPU *self) = NULL;

	PPU_CheckRegister(self);
	while (clock) {
		Stack_Init(&taskList);
		PPU_ManageTiming(self, &taskList); 
		while (!Stack_IsEmpty(&taskList)) {
			/* Execute task */
			task = Stack_Pop(&taskList);
			task(self);
		}
		clock--;
	}
	PPU_RefreshRegister(self, context);

	return EXIT_SUCCESS;
}

void PPU_Destroy(PPU *self) {
	if (self == NULL)
		return;
	free(self);
}
