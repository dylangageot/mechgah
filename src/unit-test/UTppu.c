#include "UTest.h"
#include "../nes/ppu/ppu.h"
#include "../nes/mapper/nrom.h"
#include "../common/macro.h"
#include "../nes/const.h"
#include <stdlib.h>

static int setup_PPU(void** state) {
	/* create a NROM Mapper*/
	Header config;
	config.mirroring = NROM_HORIZONTAL;
	config.romSize = NROM_16KIB;
	Mapper *mapper = MapNROM_Create(&config);

	/* create a PPU instance */
	PPU *self = PPU_Create(mapper);
	if (self == NULL)
		return -1;
	*state = (void*) self;
	IOReg *ioreg = IOReg_Extract(self->mapper);
	ioreg->bank1[PPUCTRL]	= &(self->PPUCTRL);
	ioreg->bank1[PPUMASK]	= &(self->PPUMASK);
	ioreg->bank1[PPUSTATUS]	= &(self->PPUSTATUS);
	ioreg->bank1[OAMADDR]	= &(self->OAMADDR);
	ioreg->bank1[OAMDATA]	= &(self->OAMDATA);
	ioreg->bank1[PPUSCROLL]	= &(self->PPUSCROLL);
	ioreg->bank1[PPUADDR]	= &(self->PPUADDR);
	ioreg->bank1[PPUDATA]	= &(self->PPUDATA);
	PPU_Init(self);
	return 0;
}

static void test_PPU_CheckRegister_PPUCTRL(void **state) {
	PPU *self = (PPU*) *state;

	self->vram.t = 0x0000;
	/* Test if data is written to VRAM.t */
	*Mapper_Get(self->mapper, AC_WR | AS_CPU, 0x2000) = 0xFF;
	assert_int_equal(PPU_CheckRegister(self), EXIT_SUCCESS);
	assert_int_equal(self->vram.t, 0x0C00);
	*Mapper_Get(self->mapper, AC_WR | AS_CPU, 0x2000) = 0xF0;
	assert_int_equal(PPU_CheckRegister(self), EXIT_SUCCESS);
	assert_int_equal(self->vram.t, 0x0000);
}

static void test_PPU_CheckRegister_PPUSTATUS(void **state) {
	PPU *self = (PPU*) *state;

	self->vram.w = 1;
	self->PPUSTATUS = 0xFF;
	/* Test if VBL flag is cleared and that VRAM.w is set to zero */
	*Mapper_Get(self->mapper, AC_RD | AS_CPU, 0x2002) = 0xFF;
	assert_int_equal(PPU_CheckRegister(self), EXIT_SUCCESS);
	assert_int_equal(self->vram.w, 0x00);
	assert_int_equal(self->PPUSTATUS, 0x7F);
}

static void test_PPU_CheckRegister_OAMDATA(void **state) {
	PPU *self = (PPU*) *state;

	self->OAMADDR = 0;
	self->OAM[0] = 0;
	/* Test if OAMADDR is incremented if OAMDATA has been written
	 * If OAMDATA read only test if not increment */
	*Mapper_Get(self->mapper, AC_WR | AS_CPU, 0x2004) = 0xAA;
	assert_int_equal(PPU_CheckRegister(self), EXIT_SUCCESS);
	assert_int_equal(self->OAMADDR, 0x01);
	assert_int_equal(self->OAM[0], 0xAA);
	Mapper_Get(self->mapper, AC_RD | AS_CPU, 0x2004);
	assert_int_equal(PPU_CheckRegister(self), EXIT_SUCCESS);
	assert_int_equal(self->OAMADDR, 0x01);
}

static void test_PPU_CheckRegister_PPUSCROLL(void **state) {
	PPU *self = (PPU*) *state;

	self->vram.w = 0;
	self->vram.t = 0;
	/* Test if the pattern is correctly written */
	*Mapper_Get(self->mapper, AC_WR | AS_CPU, 0x2005) = 0x7D;
	assert_int_equal(PPU_CheckRegister(self), EXIT_SUCCESS);
	assert_int_equal(self->vram.t, 0x000F);
	assert_int_equal(self->vram.x, 0x05);
	assert_int_equal(self->vram.w, 1);
	*Mapper_Get(self->mapper, AC_WR | AS_CPU, 0x2005) = 0x5E;
	assert_int_equal(PPU_CheckRegister(self), EXIT_SUCCESS);
	assert_int_equal(self->vram.t, 0x616F);
	assert_int_equal(self->vram.x, 0x05);
	assert_int_equal(self->vram.w, 0);
}

static void test_PPU_CheckRegister_PPUADDR(void **state) {
	PPU *self = (PPU*) *state;

	self->vram.w = 0;
	self->vram.t = 0x616F;
	self->vram.x = 0x05;
	self->vram.v = 0;
	/* Test if the pattern is correctly written */
	*Mapper_Get(self->mapper, AC_WR | AS_CPU, 0x2006) = 0x3D;
	assert_int_equal(PPU_CheckRegister(self), EXIT_SUCCESS);
	assert_int_equal(self->vram.t, 0x3D6F);
	assert_int_equal(self->vram.x, 0x05);
	assert_int_equal(self->vram.w, 1);
	assert_int_equal(self->vram.v, 0);
	*Mapper_Get(self->mapper, AC_WR | AS_CPU, 0x2006) = 0xF0;
	assert_int_equal(PPU_CheckRegister(self), EXIT_SUCCESS);
	assert_int_equal(self->vram.t, 0x3DF0);
	assert_int_equal(self->vram.x, 0x05);
	assert_int_equal(self->vram.w, 0);
	assert_int_equal(self->vram.v, 0x3DF0);
}

static void test_PPU_CheckRegister_PPUDATA(void **state) {
	PPU *self = (PPU*) *state;

	self->scanline = 241;
	self->PPUMASK = 0x18;
	self->PPUCTRL = 0;
	self->vram.v = 0;
	uint8_t *data = Mapper_Get(self->mapper, AS_PPU, 0x0000);
	*data = 0x00;

	/* Write test --------------------------------------- */
	/* Test if address is update by 1 and data is written */
	*Mapper_Get(self->mapper, AC_WR | AS_CPU, 0x2007) = 0xAA;
	assert_int_equal(PPU_CheckRegister(self), EXIT_SUCCESS);
	assert_int_equal(self->vram.v, 1);
	assert_int_equal(*data, 0xAA);
	self->PPUCTRL = 0x04;
	/* Test if address is update by 32 and data is written */
	*Mapper_Get(self->mapper, AC_WR | AS_CPU, 0x2007) = 0xBB;
	assert_int_equal(PPU_CheckRegister(self), EXIT_SUCCESS);
	assert_int_equal(self->vram.v, 33);
	assert_int_equal(*(data + 1), 0xBB);
	/* Test if PPU is rendering */
	*Mapper_Get(self->mapper, AC_WR | AS_CPU, 0x2007) = 0xCC;
	self->scanline = 0;
	assert_int_equal(PPU_CheckRegister(self), EXIT_SUCCESS);
	assert_int_equal(self->vram.v, 0x1022);
	assert_int_equal(*(data + 33), 0xCC);

	/* Read test ---------------------------------------- */
	/* Test if address is update by 1 and read value is in PPUDATA */
	self->scanline = 241;
	self->PPUCTRL = 0;
	self->vram.v = 0;
	Mapper_Get(self->mapper, AC_RD | AS_CPU, 0x2007);
	assert_int_equal(PPU_CheckRegister(self), EXIT_SUCCESS);
	assert_int_equal(self->vram.v, 1);
	assert_int_equal(self->PPUDATA, 0xAA);
	self->PPUCTRL = 0x04;
	/* Test if address is update by 32 and data is written */
	Mapper_Get(self->mapper, AC_RD | AS_CPU, 0x2007);
	assert_int_equal(PPU_CheckRegister(self), EXIT_SUCCESS);
	assert_int_equal(self->vram.v, 33);
	assert_int_equal(self->PPUDATA, 0xBB);
	/* Test if PPU is rendering */
	self->scanline = 0;
	Mapper_Get(self->mapper, AC_RD | AS_CPU, 0x2007);
	assert_int_equal(PPU_CheckRegister(self), EXIT_SUCCESS);
	assert_int_equal(self->vram.v, 0x1022);
	assert_int_equal(self->PPUDATA, 0xCC);

}


static void test_PPU_RefreshRegister_OAMDATA(void **state) {
	PPU *self = (PPU*) *state;
	PPU_Init(self);
	self->OAMADDR = 0xAA;
	self->OAM[0xAA] = 0xBB;
	self->OAMDATA = 0xCC;
	assert_int_equal(PPU_RefreshRegister(self, NULL), EXIT_SUCCESS);
	assert_int_equal(self->OAMDATA, 0xBB);
}

static void test_PPU_RefreshRegister_PPUDATA(void **state) {
	PPU *self = (PPU*) *state;
	PPU_Init(self);
	int i;
	self->vram.v = 0x3F00;
	for (i = 0; i < 256; i++) {
		*Mapper_Get(self->mapper, AS_PPU, self->vram.v) = i;
		assert_int_equal(PPU_RefreshRegister(self, NULL), EXIT_SUCCESS);
		assert_int_equal(self->PPUDATA,
				*Mapper_Get(self->mapper, AS_PPU, self->vram.v));
		self->vram.v++;
	}
}

static void test_PPU_RefreshRegister_NMI(void **state) {
	PPU *self = (PPU*) *state;
	PPU_Init(self);
	uint8_t context = 0;
	self->nmiSent = 0;
	self->PPUSTATUS = 0x80;
	self->PPUCTRL = 0x80;
	assert_int_equal(PPU_RefreshRegister(self, &context), EXIT_SUCCESS);
	assert_int_equal(self->nmiSent, 1);
	assert_int_equal(context, 0x02);
	context = 0;
	assert_int_equal(PPU_RefreshRegister(self, &context), EXIT_SUCCESS);
	assert_int_equal(self->nmiSent, 1);
	assert_int_equal(context, 0x00);
	self->nmiSent = 0;
	self->PPUSTATUS = 0x80;
	self->PPUCTRL = 0;
	assert_int_equal(PPU_RefreshRegister(self, &context), EXIT_SUCCESS);
	assert_int_equal(self->nmiSent, 0);
	assert_int_equal(context, 0x00);
}

static void test_PPU_ManageTiming_Prerender(void **state) {
	PPU *self = (PPU*) *state;
	Stack s;
	int i;

	Stack_Init(&s);
	PPU_Init(self);
	self->PPUMASK = 0x18;
	/* First cycle, IDLE */
	PPU_ManageTiming(self, &s);
	assert_int_equal(Stack_IsEmpty(&s), 1);
	self->cycle++;
	/* Cycle 1 - 256 */
	for (i = 1; i <= 256; i++) {
		Stack_Init(&s);
		PPU_ManageTiming(self, &s);
		assert_ptr_equal(Stack_Pop(&s), (void*) PPU_FetchTile);
		if (i < 65)
			assert_ptr_equal(Stack_Pop(&s), (void*) PPU_ClearSecondaryOAM);
		else
			assert_ptr_equal(Stack_Pop(&s), (void*) PPU_SpriteEvaluation);
		if (i == 1)
			assert_ptr_equal(Stack_Pop(&s), (void*) PPU_ClearFlag);
		if ((i % 8) == 0)
			assert_ptr_equal(Stack_Pop(&s), (void*) PPU_ManageV);
		assert_int_equal(Stack_IsEmpty(&s), 1);
		self->cycle++;
	}
	/* Cycle 257-320 */
	for (i = 257; i <= 320; i++) {
		Stack_Init(&s);
		PPU_ManageTiming(self, &s);
		if (((i - 256) % 8) == 0)
			assert_ptr_equal(Stack_Pop(&s), (void*) PPU_FetchSprite);
		if ((i == 257) || VALUE_IN(i, 280, 304))
			assert_ptr_equal(Stack_Pop(&s), (void*) PPU_ManageV);
		assert_int_equal(Stack_IsEmpty(&s), 1);
		self->cycle++;
	}
	/* Cycle 321-336 */
	for (i = 321; i <= 336; i++) {
		Stack_Init(&s);
		PPU_ManageTiming(self, &s);
		assert_ptr_equal(Stack_Pop(&s), (void*) PPU_FetchTile);
		if ((i % 8) == 0)
			assert_ptr_equal(Stack_Pop(&s), (void*) PPU_ManageV);
		assert_int_equal(Stack_IsEmpty(&s), 1);
		self->cycle++;
	}
	/* Cycle 337-340 */
	for (i = 337; i <= 340; i++) {
		Stack_Init(&s);
		PPU_ManageTiming(self, &s);
		assert_int_equal(Stack_IsEmpty(&s), 1);
		self->cycle++;
	}

}

static void test_PPU_ManageTiming_Prerender_RenderOFF(void **state) {
	PPU *self = (PPU*) *state;
	Stack s;
	int i;

	Stack_Init(&s);
	PPU_Init(self);
	self->PPUMASK = 0;
	/* First cycle, IDLE */
	PPU_ManageTiming(self, &s);
	assert_int_equal(Stack_IsEmpty(&s), 1);
	self->cycle++;
	/* Cycle 1 - 256 */
	for (i = 1; i <= 256; i++) {
		Stack_Init(&s);
		PPU_ManageTiming(self, &s);
		if (i == 1)
			assert_ptr_equal(Stack_Pop(&s), (void*) PPU_ClearFlag);
		assert_int_equal(Stack_IsEmpty(&s), 1);
		self->cycle++;
	}
	/* Cycle 257-320 */
	for (i = 257; i <= 320; i++) {
		Stack_Init(&s);
		PPU_ManageTiming(self, &s);
		assert_int_equal(Stack_IsEmpty(&s), 1);
		self->cycle++;
	}
	/* Cycle 321-340 */
	for (i = 321; i <= 340; i++) {
		Stack_Init(&s);
		PPU_ManageTiming(self, &s);
		assert_int_equal(Stack_IsEmpty(&s), 1);
		self->cycle++;
	}

}

static void test_PPU_ManageTiming_VisibleScanline(void **state) {
	PPU *self = (PPU*) *state;
	Stack s;
	int i, j;

	Stack_Init(&s);
	PPU_Init(self);
	self->PPUMASK = 0x18;
	self->scanline++;
	for (j = 0; j < 240; j++) {
		self->cycle = 0;
		/* First cycle, IDLE */
		PPU_ManageTiming(self, &s);
		assert_int_equal(Stack_IsEmpty(&s), 1);
		self->cycle++;
		/* Cycle 1 - 256 */
		for (i = 1; i <= 256; i++) {
			Stack_Init(&s);
			PPU_ManageTiming(self, &s);
			assert_ptr_equal(Stack_Pop(&s), (void*) PPU_Draw);
			assert_ptr_equal(Stack_Pop(&s), (void*) PPU_FetchTile);
			if (i < 65)
				assert_ptr_equal(Stack_Pop(&s), (void*) PPU_ClearSecondaryOAM);
			else
				assert_ptr_equal(Stack_Pop(&s), (void*) PPU_SpriteEvaluation);
			if ((i % 8) == 0)
				assert_ptr_equal(Stack_Pop(&s), (void*) PPU_ManageV);
			assert_int_equal(Stack_IsEmpty(&s), 1);
			self->cycle++;
		}
		/* Cycle 257-320 */
		for (i = 257; i <= 320; i++) {
			Stack_Init(&s);
			PPU_ManageTiming(self, &s);
			if (((i - 256) % 8) == 0)
				assert_ptr_equal(Stack_Pop(&s), (void*) PPU_FetchSprite);
			if (i == 257)
				assert_ptr_equal(Stack_Pop(&s), (void*) PPU_ManageV);
			assert_int_equal(Stack_IsEmpty(&s), 1);
			self->cycle++;
		}
		/* Cycle 321-336 */
		for (i = 321; i <= 336; i++) {
			Stack_Init(&s);
			PPU_ManageTiming(self, &s);
			assert_ptr_equal(Stack_Pop(&s), (void*) PPU_FetchTile);
			if ((i % 8) == 0)
				assert_ptr_equal(Stack_Pop(&s), (void*) PPU_ManageV);
			assert_int_equal(Stack_IsEmpty(&s), 1);
			self->cycle++;
		}
		/* Cycle 337-340 */
		for (i = 337; i <= 340; i++) {
			Stack_Init(&s);
			PPU_ManageTiming(self, &s);
			assert_int_equal(Stack_IsEmpty(&s), 1);
			self->cycle++;
		}
		self->scanline++;
	}

	/* Test skipped on odd frame */
	self->PPUMASK = 0xFF;
	self->nbFrame = 1;
	self->scanline = 0;
	self->cycle = 0;
	Stack_Init(&s);
	PPU_ManageTiming(self, &s);
	assert_int_equal(self->cycle, 1);
	/* Test if not skiped on even frame */
	self->nbFrame = 2;
	self->cycle = 0;
	Stack_Init(&s);
	PPU_ManageTiming(self, &s);
	assert_int_equal(self->cycle, 0);
	/* Test if not skipped on odd frame */
	self->PPUMASK = 0x00;
	self->nbFrame = 3;
	self->cycle = 0;
	Stack_Init(&s);
	PPU_ManageTiming(self, &s);
	assert_int_equal(self->cycle, 0);
}

static void test_PPU_ManageTiming_VisibleScanline_RenderOFF(void **state) {
	PPU *self = (PPU*) *state;
	Stack s;
	int i, j;

	Stack_Init(&s);
	PPU_Init(self);
	self->PPUMASK = 0x00;
	self->scanline++;
	for (j = 0; j < 240; j++) {
		self->cycle = 0;
		/* Cycle 0 - 340 */
		for (i = 0; i <= 340; i++) {
			Stack_Init(&s);
			PPU_ManageTiming(self, &s);
			assert_int_equal(Stack_IsEmpty(&s), 1);
			self->cycle++;
		}
		self->scanline++;
	}

}

static void test_PPU_ManageTiming_IdleScanline(void **state) {
	PPU *self = (PPU*) *state;
	Stack s;
	int i, j;

	Stack_Init(&s);
	PPU_Init(self);
	self->scanline = 240;
	for (j = 240; j < 260; j++) {
		self->cycle = 0;
		for (i = 0; i <= 340; i++) {
			Stack_Init(&s);
			PPU_ManageTiming(self, &s);
			if ((j == 241) && (i == 1))
				assert_ptr_equal(Stack_Pop(&s), (void*) PPU_SetFlag);
			assert_int_equal(Stack_IsEmpty(&s), 1);
			self->cycle++;
		}
		self->scanline++;
	}
}

static void test_PPU_IncrementCorseX(void **state) {
	PPU *self = (PPU*) *state;
	/* Increment Corse X without overflow */
	self->vram.v = 1;
	assert_int_equal(PPU_IncrementCorseX(self), EXIT_SUCCESS);
	assert_int_equal(self->vram.v, 2);
	/* Increment Corse X with overflow */
	self->vram.v = 31;
	assert_int_equal(PPU_IncrementCorseX(self), EXIT_SUCCESS);
	assert_int_equal(self->vram.v, 0x0400);
	/* Increment Corse X with overflow and nametable already switched */
	self->vram.v = 31 | 0x0400;
	assert_int_equal(PPU_IncrementCorseX(self), EXIT_SUCCESS);
	assert_int_equal(self->vram.v, 0x0000);
}

static void test_PPU_IncrementY(void **state) {
	PPU *self = (PPU*) *state;
	/* Increment Fine Y without overflow */
	self->vram.v = 1;
	assert_int_equal(PPU_IncrementY(self), EXIT_SUCCESS);
	assert_int_equal(self->vram.v, 0x1001);
	/* Increment Fine Y with overflow */
	self->vram.v = 0x7000;
	assert_int_equal(PPU_IncrementY(self), EXIT_SUCCESS);
	assert_int_equal(self->vram.v, 0x0020);
	/* Increment Fine Y with overflow of both fine and corse */
	self->vram.v = 0x7BE0;
	assert_int_equal(PPU_IncrementY(self), EXIT_SUCCESS);
	assert_int_equal(self->vram.v, 0x0800);
	/* Increment Fine Y with overflow of "tile" of corse */
	self->vram.v = 0x7BA0;
	assert_int_equal(PPU_IncrementY(self), EXIT_SUCCESS);
	assert_int_equal(self->vram.v, 0x0000);
	/* Increment Fine Y with overflow of fine only */
	self->vram.v = 0x7B80;
	assert_int_equal(PPU_IncrementY(self), EXIT_SUCCESS);
	assert_int_equal(self->vram.v, 0x0BA0);
}

static void test_PPU_ManageV(void **state) {
	PPU *self = (PPU*) *state;
	int i;
	/* Increment Corse X when in 8->248 interval */
	self->vram.t = 0;
	self->vram.v = 0;
	for (i = 8; i <= 248; i += 8) {
		self->cycle = i;
		assert_int_equal(PPU_ManageV(self), EXIT_SUCCESS);
		assert_int_equal(self->vram.v, i >> 3);
	}
	/* Increment Y when at cycle at dot 256 */
	self->cycle = 256;
	assert_int_equal(PPU_ManageV(self), EXIT_SUCCESS);
	assert_int_equal(self->vram.v, 0x101F);
	/* Affect hori(t) to hori(v) at dot 257 */
	self->cycle = 257;
	assert_int_equal(PPU_ManageV(self), EXIT_SUCCESS);
	assert_int_equal(self->vram.v, 0x1000);
	/* Affect vert(t) to vert(v) at dot 280 to 304 */
	for (i = 280; i <= 304; i++) {
		self->cycle = i;
		assert_int_equal(PPU_ManageV(self), EXIT_SUCCESS);
		assert_int_equal(self->vram.v, 0x0000);
	}

}


static void test_PPU_FetchTile_Shift(void **state) {
	PPU* self = (PPU*) *state;

	uint16_t bl, bh, al, ah;
	bl = 0x0FFF;
	bh = 0x07FF;
	al = 0x03FF;
	ah = 0x01FF;

	/* shift register shifting test */

	self->cycle = 1;

	self->bitmapL = bl;
	self->bitmapH = bh;
	self->attributeL = al;
	self->attributeH = ah;

	PPU_FetchTile(self);

	assert_int_equal(self->bitmapL, bl << 0x01);
	assert_int_equal(self->bitmapH, bh << 0x01);
	assert_int_equal(self->attributeL, al << 0x01);
	assert_int_equal(self->attributeH, ah << 0x01);
}

static void test_PPU_FetchTile_Filling(void **state) {
	PPU* self = (PPU*) *state;

	/* bitmapL, bitmapH, attributeL, attributeH */
	uint16_t bl, bh, al, ah;

	self->vram.v = 0;
	 /* set coarse x to 28, coarse y to 22 and fine y to 3 */
	self->vram.v |= ( 0x001C
					| (0x0016 << 5)
					| (0x0000 << 10)
					| (0x0003 << 12));

	uint8_t* pattern_address = Mapper_Get(self->mapper, AS_PPU, 0x2000
											| (self->vram.v & 0x0FFF));
	 /* initialize with a random pattern address */
	*pattern_address = 0x85;

	uint8_t* attribute = Mapper_Get(self->mapper, AS_PPU,0x23C0
									| (self->vram.v & 0x0C00)
									| ((self->vram.v >> 4) & 0x38)
									| ((self->vram.v >> 2) & 0x07));

	uint8_t* pattern = Mapper_Get(self->mapper, AS_LDR, LDR_CHR)
								+ ((self->PPUCTRL & 0x10) ? 0x1000 : 0);

	uint8_t* tile_pattern = pattern + ( *pattern_address << 4);

	/* set pattern low bits */
	tile_pattern[self->vram.v>>12] = 0xE1;
	/* set pattern high bits */
	tile_pattern[(self->vram.v>>12) | 0x08 ] = 0x2C;

	/* set in accord with coarse x and coarse y */
	/* attribute is set to 2 */
	*attribute = 0x20;

	/* data insertion test */
	self->cycle = 8;

	bl = 0xFEDC;
	bh = 0x96D3;
	al = 0x778A;
	ah = 0xAE9F;

	self->bitmapL = bl;
	self->bitmapH = bh;
	self->attributeL = al;
	self->attributeH = ah;


	PPU_FetchTile(self);

	assert_int_equal(self->bitmapL & 0xFF00, (bl << 0x01) & 0xFF00);
	assert_int_equal(self->bitmapL & 0x00FF, 0xE1);

	assert_int_equal(self->bitmapH & 0xFF00, (bh << 0x01) & 0xFF00);
	assert_int_equal(self->bitmapH & 0x00FF, 0x2C);

	assert_int_equal(self->attributeL & 0xFF00, (al << 0x01) & 0xFF00);
	assert_int_equal(self->attributeL & 0x00FF, 0);

	assert_int_equal(self->attributeH & 0xFF00, (ah << 0x01) & 0xFF00);
	assert_int_equal(self->attributeH & 0x00FF, 0x00FF);
}

static void test_PPU_ClearSecondaryOAM(void **state) {
	PPU *self = (PPU*) *state;
	int i;
	/* Fill Secondary OAM will 0xAA */
	for (i = 0; i < 32; i++) {
		self->SOAM[i] = 0xAA;
	}
	for (i = 0; i < 64; i++) {
		self->cycle = i + 1;
		PPU_ClearSecondaryOAM(self);
		if (i % 2)
			assert_int_equal(0xFF, self->SOAM[i >> 1]);
		else
			assert_int_equal(0xAA, self->SOAM[i >> 1]);
	}

}

static void test_PPU_SpriteEvaluation_NoOverflow(void **state) {
	PPU *self = (PPU*) *state;

	int i;
	self->scanline = 10;
	self->PPUSTATUS = 0;

	/* Set value into OAM */
	for (i = 0; i < 256; i++) {
		self->OAM[i] = i;
	}
	/* Set to 0xFF every Y-coordonate */
	for (i = 0; i < 64; i++) {
		self->OAM[i << 2] = 0xAA;
	}
	/* Clear SOAM */
	for (i = 0; i < 32; i++) {
		self->SOAM[i] = 0xFF;
	}
	/* Set 4 sprites Y-coordonate in range
	 * Make every sprite spaced in memory to hard test the algorithm */
	self->OAM[1 << 2] = self->scanline;
	self->OAM[3 << 2] = self->scanline;
	self->OAM[5 << 2] = self->scanline;
	self->OAM[7 << 2] = self->scanline;
	/* Evaluate */
	for (self->cycle = 65; self->cycle < 257; self->cycle++) {
		PPU_SpriteEvaluation(self);
	}
	/* Verify if 4 sprite has been copied */
	for (i = 0; i < 4; i++) {
		assert_int_equal(self->OAM[1 * 4 + i], self->SOAM[i]);
		assert_int_equal(self->OAM[3 * 4 + i], self->SOAM[1 * 4 + i]);
		assert_int_equal(self->OAM[5 * 4 + i], self->SOAM[2 * 4 + i]);
		assert_int_equal(self->OAM[7 * 4 + i], self->SOAM[3 * 4 + i]);
	}
	/* Test the copied data for Y in range test */
	assert_int_equal(0xAA, self->SOAM[16]);
	for (i = 17; i < 32; i++) {
		assert_int_equal(0xFF, self->SOAM[i]);
	}

	/* Verify that no overflow has been detected */
	assert_int_equal(self->PPUSTATUS, 0);
}

static void test_PPU_SpriteEvaluation_Eight(void **state) {
	PPU *self = (PPU*) *state;

	int i;
	self->scanline = 10;
	self->PPUSTATUS = 0;

	/* Set value into OAM */
	for (i = 0; i < 256; i++) {
		self->OAM[i] = i;
	}
	/* Set to 0xFF every Y-coordonate */
	for (i = 0; i < 64; i++) {
		self->OAM[i << 2] = 0xAA;
	}
	/* Clear SOAM */
	for (i = 0; i < 32; i++) {
		self->SOAM[i] = 0xFF;
	}
	/* Set 8 sprites Y-coordonate in range
	 * Make every sprite spaced in memory to hard test the algorithm */
	for (i = 0; i < 8; i++) {
		self->OAM[i << 2] = self->scanline;
	}
	/* Evaluate */
	for (self->cycle = 65; self->cycle < 257; self->cycle++) {
		PPU_SpriteEvaluation(self);
	}
	/* Verify if 8 sprite has been copied */
	for (i = 0; i < 32; i++) {
		assert_int_equal(self->OAM[i], self->SOAM[i]);
	}

	/* Verify that no overflow has been detected */
	assert_int_equal(self->PPUSTATUS, 0);
}

static void test_PPU_SpriteEvaluation_Overflow(void **state) {
	PPU *self = (PPU*) *state;

	int i;
	self->scanline = 10;
	self->PPUSTATUS = 0;

	/* Set value into OAM */
	for (i = 0; i < 256; i++) {
		self->OAM[i] = i;
	}
	/* Set to 0xFF every Y-coordonate */
	for (i = 0; i < 64; i++) {
		self->OAM[i << 2] = 0xAA;
	}
	/* Clear SOAM */
	for (i = 0; i < 32; i++) {
		self->SOAM[i] = 0xFF;
	}
	/* Set 9 sprites Y-coordonate in range
	 * Make every sprite spaced in memory to hard test the algorithm */
	for (i = 0; i < 9; i++) {
		self->OAM[i << 2] = self->scanline;
	}
	/* Evaluate */
	for (self->cycle = 65; self->cycle < 257; self->cycle++) {
		PPU_SpriteEvaluation(self);
	}
	/* Verify if 8 sprite has been copied */
	for (i = 0; i < 32; i++) {
		assert_int_equal(self->OAM[i], self->SOAM[i]);
	}

	/* Verify that overflow has been detected */
	assert_int_equal(self->PPUSTATUS, 0x20);
}

static void test_PPU_SpriteEvaluation_Zero(void **state) {
	PPU *self = (PPU*) *state;

	int i;
	self->scanline = 10;
	self->PPUSTATUS = 0;

	/* Set value into OAM */
	for (i = 0; i < 256; i++) {
		self->OAM[i] = i;
	}
	/* Set to 0xFF every Y-coordonate */
	for (i = 0; i < 64; i++) {
		self->OAM[i << 2] = 0xAA;
	}
	/* Clear SOAM */
	for (i = 0; i < 32; i++) {
		self->SOAM[i] = 0xFF;
	}
	/* Evaluate */
	for (self->cycle = 65; self->cycle < 257; self->cycle++) {
		PPU_SpriteEvaluation(self);
	}
	/* Verify that SOAM is clear */
	for (i = 1; i < 32; i++) {
		assert_int_equal(0xFF, self->SOAM[i]);
	}

	/* Verify that no overflow has been detected */
	assert_int_equal(self->PPUSTATUS, 0x00);
}

static void test_PPU_FetchSprite_NoFlip(void **state) {
	PPU *self = (PPU*) *state;

	uint8_t* pattern = Mapper_Get(self->mapper, AS_LDR, LDR_CHR);
	uint8_t* tile_pattern = NULL;

	int i;
	self->scanline = 10;
	self->PPUSTATUS = 0;

	/* Clear SOAM */
	for (i = 0; i < 32; i++) {
		self->SOAM[i] = 0xFF;
	}

	/* Test only for two sprites */
	/* Set first sprite found in OAM */
	self->SOAM[0] = self->scanline - 1;
	self->SOAM[1] = 5; /* Set pattern n°6 as sprite */
	self->SOAM[2] = 0x03;
	self->SOAM[3] = 0xCC;
	self->SOAM[4] = self->scanline - 7;
	self->SOAM[5] = 25; /* Set pattern n°26 as sprite */
	self->SOAM[6] = 0x01;
	self->SOAM[7] = 0xDD;
	/* Initialize pattern table */
	/* Pattern n°6 */
	tile_pattern = pattern + (5 << 4);
	for (i = 0; i < 16; i++) {
		tile_pattern[i] = 0xFF;
	}
	tile_pattern[1] = 0xAA;
	tile_pattern[1 | 0x8] = 0x55;
	/* Pattern n°26 */
	tile_pattern = pattern + (25 << 4);
	for (i = 0; i < 16; i++) {
		tile_pattern[i] = 0xFF;
	}
	tile_pattern[7] = 0x55;
	tile_pattern[7 | 0x8] = 0xAA;

	/* Execute Fetch sprite */
	for (self->cycle = 257; self->cycle <= 320; self->cycle += 8) {
		PPU_FetchSprite(self);
	}

	/* Test first sprite */
	assert_int_equal(self->sprite[0].patternL, 0xAA);
	assert_int_equal(self->sprite[0].patternH, 0x55);
	assert_int_equal(self->sprite[0].attribute, self->SOAM[2]);
	assert_int_equal(self->sprite[0].x, self->SOAM[3]);
	/* Test seconde sprite */
	assert_int_equal(self->sprite[1].patternL, 0x55);
	assert_int_equal(self->sprite[1].patternH, 0xAA);
	assert_int_equal(self->sprite[1].attribute, self->SOAM[6]);
	assert_int_equal(self->sprite[1].x, self->SOAM[7]);
	/* Test emptyness of following sprite */
	for (i = 2; i < 8; i++) {
		assert_int_equal(self->sprite[i].patternL, 0x00);
		assert_int_equal(self->sprite[i].patternH, 0x00);
		assert_int_equal(self->sprite[i].attribute, 0);
		assert_int_equal(self->sprite[i].x, 0xFF);
	}

}

static void test_PPU_FetchSprite_FlipHorizontal(void **state) {
	PPU *self = (PPU*) *state;

	uint8_t* pattern = Mapper_Get(self->mapper, AS_LDR, LDR_CHR);
	uint8_t* tile_pattern = NULL;

	int i;
	self->scanline = 10;
	self->PPUSTATUS = 0;

	/* Clear SOAM */
	for (i = 0; i < 32; i++) {
		self->SOAM[i] = 0xFF;
	}

	/* Test only for two sprites */
	/* Set first sprite found in OAM */
	self->SOAM[0] = self->scanline - 1;
	self->SOAM[1] = 5; /* Set pattern n°6 as sprite */
	self->SOAM[2] = 0x43;
	self->SOAM[3] = 0xCC;
	self->SOAM[4] = self->scanline - 7;
	self->SOAM[5] = 25; /* Set pattern n°26 as sprite */
	self->SOAM[6] = 0x41;
	self->SOAM[7] = 0xDD;
	/* Initialize pattern table */
	/* Pattern n°6 */
	tile_pattern = pattern + (5 << 4);
	for (i = 0; i < 16; i++) {
		tile_pattern[i] = 0xFF;
	}
	tile_pattern[1] = 0xAA;
	tile_pattern[1 | 0x8] = 0x55;
	/* Pattern n°26 */
	tile_pattern = pattern + (25 << 4);
	for (i = 0; i < 16; i++) {
		tile_pattern[i] = 0xFF;
	}
	tile_pattern[7] = 0x55;
	tile_pattern[7 | 0x8] = 0xAA;

	/* Execute Fetch sprite */
	for (self->cycle = 257; self->cycle <= 320; self->cycle += 8) {
		PPU_FetchSprite(self);
	}

	/* Test first sprite */
	assert_int_equal(self->sprite[0].patternL, 0x55);
	assert_int_equal(self->sprite[0].patternH, 0xAA);
	assert_int_equal(self->sprite[0].attribute, self->SOAM[2]);
	assert_int_equal(self->sprite[0].x, self->SOAM[3]);
	/* Test seconde sprite */
	assert_int_equal(self->sprite[1].patternL, 0xAA);
	assert_int_equal(self->sprite[1].patternH, 0x55);
	assert_int_equal(self->sprite[1].attribute, self->SOAM[6]);
	assert_int_equal(self->sprite[1].x, self->SOAM[7]);
	/* Test emptyness of following sprite */
	for (i = 2; i < 8; i++) {
		assert_int_equal(self->sprite[i].patternL, 0x00);
		assert_int_equal(self->sprite[i].patternH, 0x00);
		assert_int_equal(self->sprite[i].attribute, 0);
		assert_int_equal(self->sprite[i].x, 0xFF);
	}

}

static void test_PPU_FetchSprite_FlipVertical(void **state) {
	PPU *self = (PPU*) *state;

	uint8_t* pattern = Mapper_Get(self->mapper, AS_LDR, LDR_CHR);
	uint8_t* tile_pattern = NULL;

	int i;
	self->scanline = 10;
	self->PPUSTATUS = 0;

	/* Clear SOAM */
	for (i = 0; i < 32; i++) {
		self->SOAM[i] = 0xFF;
	}

	/* Test only for two sprites */
	/* Set first sprite found in OAM */
	self->SOAM[0] = self->scanline - 1;
	self->SOAM[1] = 5; /* Set pattern n°6 as sprite */
	self->SOAM[2] = 0x83;
	self->SOAM[3] = 0xCC;
	self->SOAM[4] = self->scanline - 7;
	self->SOAM[5] = 25; /* Set pattern n°26 as sprite */
	self->SOAM[6] = 0x81;
	self->SOAM[7] = 0xDD;
	/* Initialize pattern table */
	/* Pattern n°6 */
	tile_pattern = pattern + (5 << 4);
	for (i = 0; i < 16; i++) {
		tile_pattern[i] = 0xFF;
	}
	tile_pattern[7-1] = 0xAA;
	tile_pattern[(7-1) | 0x8] = 0x55;
	/* Pattern n°26 */
	tile_pattern = pattern + (25 << 4);
	for (i = 0; i < 16; i++) {
		tile_pattern[i] = 0xFF;
	}
	tile_pattern[7-7] = 0x55;
	tile_pattern[(7-7) | 0x8] = 0xAA;

	/* Execute Fetch sprite */
	for (self->cycle = 257; self->cycle <= 320; self->cycle += 8) {
		PPU_FetchSprite(self);
	}

	/* Test first sprite */
	assert_int_equal(self->sprite[0].patternL, 0xAA);
	assert_int_equal(self->sprite[0].patternH, 0x55);
	assert_int_equal(self->sprite[0].attribute, self->SOAM[2]);
	assert_int_equal(self->sprite[0].x, self->SOAM[3]);
	/* Test seconde sprite */
	assert_int_equal(self->sprite[1].patternL, 0x55);
	assert_int_equal(self->sprite[1].patternH, 0xAA);
	assert_int_equal(self->sprite[1].attribute, self->SOAM[6]);
	assert_int_equal(self->sprite[1].x, self->SOAM[7]);
	/* Test emptyness of following sprite */
	for (i = 2; i < 8; i++) {
		assert_int_equal(self->sprite[i].patternL, 0x00);
		assert_int_equal(self->sprite[i].patternH, 0x00);
		assert_int_equal(self->sprite[i].attribute, 0);
		assert_int_equal(self->sprite[i].x, 0xFF);
	}

}

static void test_PPU_FetchSprite_FlipBoth(void **state) {
	PPU *self = (PPU*) *state;

	uint8_t* pattern = Mapper_Get(self->mapper, AS_LDR, LDR_CHR);
	uint8_t* tile_pattern = NULL;

	int i;
	self->scanline = 10;
	self->PPUSTATUS = 0;

	/* Clear SOAM */
	for (i = 0; i < 32; i++) {
		self->SOAM[i] = 0xFF;
	}

	/* Test only for two sprites */
	/* Set first sprite found in OAM */
	self->SOAM[0] = self->scanline - 1;
	self->SOAM[1] = 5; /* Set pattern n°6 as sprite */
	self->SOAM[2] = 0xC3;
	self->SOAM[3] = 0xCC;
	self->SOAM[4] = self->scanline - 7;
	self->SOAM[5] = 25; /* Set pattern n°26 as sprite */
	self->SOAM[6] = 0xC1;
	self->SOAM[7] = 0xDD;
	/* Initialize pattern table */
	/* Pattern n°6 */
	tile_pattern = pattern + (5 << 4);
	for (i = 0; i < 16; i++) {
		tile_pattern[i] = 0xFF;
	}
	tile_pattern[7-1] = 0xAA;
	tile_pattern[(7-1) | 0x8] = 0x55;
	/* Pattern n°26 */
	tile_pattern = pattern + (25 << 4);
	for (i = 0; i < 16; i++) {
		tile_pattern[i] = 0xFF;
	}
	tile_pattern[7-7] = 0x55;
	tile_pattern[(7-7) | 0x8] = 0xAA;

	/* Execute Fetch sprite */
	for (self->cycle = 257; self->cycle <= 320; self->cycle += 8) {
		PPU_FetchSprite(self);
	}

	/* Test first sprite */
	assert_int_equal(self->sprite[0].patternL, 0x55);
	assert_int_equal(self->sprite[0].patternH, 0xAA);
	assert_int_equal(self->sprite[0].attribute, self->SOAM[2]);
	assert_int_equal(self->sprite[0].x, self->SOAM[3]);
	/* Test seconde sprite */
	assert_int_equal(self->sprite[1].patternL, 0xAA);
	assert_int_equal(self->sprite[1].patternH, 0x55);
	assert_int_equal(self->sprite[1].attribute, self->SOAM[6]);
	assert_int_equal(self->sprite[1].x, self->SOAM[7]);
	/* Test emptyness of following sprite */
	for (i = 2; i < 8; i++) {
		assert_int_equal(self->sprite[i].patternL, 0x00);
		assert_int_equal(self->sprite[i].patternH, 0x00);
		assert_int_equal(self->sprite[i].attribute, 0);
		assert_int_equal(self->sprite[i].x, 0xFF);
	}

}

static void test_Draw_SpriteZero(void **state) {

	/* tests spriteZero flag */

	PPU* self = (PPU*) *state;
	int i;

	/* setup values for test */

	uint8_t *palette = Mapper_Get(self->mapper, AS_PPU, ADDR_PALETTE_BG);

	uint8_t value1 = 0x8F;
	uint8_t value2 = 0xFC;


	self->PPUMASK = 0x18;
	self->PPUSTATUS = 0x04;

	self->cycle = 10;
	self->scanline = 20;

	self->attributeL = 0;
	self->attributeH = 0;

	self->bitmapL = 0x89C4;
	self->bitmapH = 0xF25C;

	self->vram.x = 0;

	for (i = 0; i < SPR_SOAM_CNT; i++) {
		self->sprite[i].x = (i==0)? 0 : 200;
		self->sprite[i].patternH = value1;
		self->sprite[i].patternL = value2;
		self->sprite[i].isSpriteZero = 1;
		self->sprite[i].attribute = 0x03;
	}

	/* initialize one value for the test to run properly */
	uint8_t* color_palette = palette + 0x1C;
	uint32_t color = ((value1 >> 6) & 0x02) | ((value2 >> 7) & 0x01);
	color_palette[color] = 2;

	PPU_Draw(self);

	assert_int_equal(self->PPUSTATUS & PPUSTATUS_SPR_ZERO , PPUSTATUS_SPR_ZERO);
}

static void test_Draw_Shift(void** state) {
	/* tests the paattern shifting */

	PPU* self = (PPU*)*state;
	int i;

	/* setup values for the test */

	uint8_t *palette = Mapper_Get(self->mapper, AS_PPU, ADDR_PALETTE_BG);

	uint8_t value1 = 0x8F;
	uint8_t value2 = 0xFC;

	self->PPUMASK = 0x18;
	self->PPUSTATUS = 0x04;

	self->cycle = 10;
	self->scanline = 20;

	self->attributeL = 0;
	self->attributeH = 0;

	self->bitmapL = 0x89C4;
	self->bitmapH = 0xF25C;

	self->vram.x = 0;

	for (i = 0; i < SPR_SOAM_CNT; i++) {
		self->sprite[i].x = (i==0)? 0 : 200;
		self->sprite[i].patternH = value1;
		self->sprite[i].patternL = value2;
		self->sprite[i].isSpriteZero = 1;
		self->sprite[i].attribute = 0x03;
	}

	/* initialize one value for the test to run properly */

	uint8_t* color_palette = palette + 0x1C;
	uint32_t color = ((value1 >> 6) & 0x02) | ((value2 >> 7) & 0x01);
	color_palette[color] = 2;

	PPU_Draw(self);

	assert_int_equal(self->sprite[0].patternH, value1<<1 & 0xFF);
	assert_int_equal(self->sprite[0].patternL, value2<<1 & 0xFF);

}

static void test_Draw_Color(void** state) {
	/* tests the color stored in image */

	PPU* self = (PPU*)*state;
	int i;

	/* setup values for the test */

	uint32_t colorPalette[64] = {
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

	uint8_t *palette = Mapper_Get(self->mapper, AS_PPU, ADDR_PALETTE_BG);

	uint8_t value1 = 0x8F;
	uint8_t value2 = 0xFC;
	uint8_t value3 = 0x9E;
	uint8_t value4 = 0x68;
	uint8_t value5 = 0x14;
	uint8_t value6 = 0xA;

	self->PPUMASK = 0x18;
	self->PPUSTATUS = 0x04;

	self->cycle = value6;
	self->scanline = value5;

	self->attributeL = value3;
	self->attributeH = value4;

	self->bitmapL = 0x89C4;
	self->bitmapH = 0xF25C;

	self->vram.x = 0;

	for (i = 0; i < SPR_SOAM_CNT; i++) {
		self->sprite[i].x = (i==0)? 0 : 200;
		self->sprite[i].patternH = value1;
		self->sprite[i].patternL = value2;
		self->sprite[i].isSpriteZero = 1;
		self->sprite[i].attribute = 0x03;
	}

	/* initialize one value for the test to run properly */

	uint8_t* color_palette = palette + 0x1C;
	uint32_t color = ((value1 >> 6) & 0x02) | ((value2 >> 7) & 0x01);
	color_palette[color] = 2;

	PPU_Draw(self);

	assert_int_equal(self->image[(value5<<8)+(value6-1)], colorPalette[2]);
}

static int teardown_PPU(void **state) {
	if (*state != NULL) {
		PPU *self = (PPU*) *state;
		Mapper_Destroy(self->mapper);
		PPU_Destroy(self);
		return 0;
	} else
		return -1;
}

int run_UTppu(void) {
	const struct CMUnitTest test_PPU_CheckRegister[] = {
		cmocka_unit_test(test_PPU_CheckRegister_PPUCTRL),
		cmocka_unit_test(test_PPU_CheckRegister_PPUSTATUS),
		cmocka_unit_test(test_PPU_CheckRegister_OAMDATA),
		cmocka_unit_test(test_PPU_CheckRegister_PPUSCROLL),
		cmocka_unit_test(test_PPU_CheckRegister_PPUADDR),
		cmocka_unit_test(test_PPU_CheckRegister_PPUDATA),
	};
	const struct CMUnitTest test_PPU_RefreshRegister[] = {
		cmocka_unit_test(test_PPU_RefreshRegister_OAMDATA),
		cmocka_unit_test(test_PPU_RefreshRegister_PPUDATA),
		cmocka_unit_test(test_PPU_RefreshRegister_NMI),
	};
	const struct CMUnitTest test_PPU_ManageTiming[] = {
		cmocka_unit_test(test_PPU_ManageTiming_Prerender),
		cmocka_unit_test(test_PPU_ManageTiming_Prerender_RenderOFF),
		cmocka_unit_test(test_PPU_ManageTiming_VisibleScanline),
		cmocka_unit_test(test_PPU_ManageTiming_VisibleScanline_RenderOFF),
		cmocka_unit_test(test_PPU_ManageTiming_IdleScanline),
	};
	const struct CMUnitTest test_PPU_ManageVRAMAddr[] = {
		cmocka_unit_test(test_PPU_IncrementCorseX),
		cmocka_unit_test(test_PPU_IncrementY),
		cmocka_unit_test(test_PPU_ManageV),
	};

	const struct CMUnitTest test_PPU_FetchTile[] = {
		cmocka_unit_test(test_PPU_FetchTile_Shift),
		cmocka_unit_test(test_PPU_FetchTile_Filling),
	};
	const struct CMUnitTest test_PPU_Sprite[] = {
		cmocka_unit_test(test_PPU_ClearSecondaryOAM),
		cmocka_unit_test(test_PPU_SpriteEvaluation_NoOverflow),
		cmocka_unit_test(test_PPU_SpriteEvaluation_Overflow),
		cmocka_unit_test(test_PPU_SpriteEvaluation_Eight),
		cmocka_unit_test(test_PPU_SpriteEvaluation_Zero),
		cmocka_unit_test(test_PPU_FetchSprite_NoFlip),
		cmocka_unit_test(test_PPU_FetchSprite_FlipHorizontal),
		cmocka_unit_test(test_PPU_FetchSprite_FlipVertical),
		cmocka_unit_test(test_PPU_FetchSprite_FlipBoth),
	};
	const struct CMUnitTest test_PPU_Draw[] = {
		cmocka_unit_test(test_Draw_SpriteZero),
		cmocka_unit_test(test_Draw_Shift),
		cmocka_unit_test(test_Draw_Color)
	};
	int out = 0;
	out += cmocka_run_group_tests(test_PPU_CheckRegister, setup_PPU, teardown_PPU);
	out += cmocka_run_group_tests(test_PPU_RefreshRegister, setup_PPU, teardown_PPU);
	out += cmocka_run_group_tests(test_PPU_ManageTiming, setup_PPU, teardown_PPU);
	out += cmocka_run_group_tests(test_PPU_ManageVRAMAddr, setup_PPU, teardown_PPU);
	out += cmocka_run_group_tests(test_PPU_FetchTile, setup_PPU, teardown_PPU);
	out += cmocka_run_group_tests(test_PPU_Sprite, setup_PPU, teardown_PPU);
	out += cmocka_run_group_tests(test_PPU_Draw, setup_PPU, teardown_PPU);
	return out;
}
