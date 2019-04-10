#include "UTest.h"
#include "../nes/ppu/ppu.h"
#include "../nes/mapper/nrom.h"
#include "../common/macro.h"
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

	self->PPUCTRL = 0;
	self->vram.v = 0;
	uint8_t *data = Mapper_Get(self->mapper, AS_PPU, 0x0000);
	*data = 0x00;
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
}

static void test_PPU_ManageTiming_Prerender(void **state) {
	PPU *self = (PPU*) *state;
	Stack s;	
	int i;

	Stack_Init(&s);
	PPU_Init(self);
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

static void test_PPU_ManageTiming_VisibleScanline(void **state) {
	PPU *self = (PPU*) *state;
	Stack s;	
	int i, j;

	Stack_Init(&s);
	PPU_Init(self);
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
			assert_ptr_equal(Stack_Pop(&s), (void*) PPU_FetchTile);
			assert_ptr_equal(Stack_Pop(&s), (void*) PPU_Draw);
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
	const struct CMUnitTest test_PPU_ManageTiming[] = {
		cmocka_unit_test(test_PPU_ManageTiming_Prerender),
		cmocka_unit_test(test_PPU_ManageTiming_VisibleScanline),
		cmocka_unit_test(test_PPU_ManageTiming_IdleScanline),
	};
	int out = 0;
	out += cmocka_run_group_tests(test_PPU_CheckRegister, setup_PPU, teardown_PPU);
	out += cmocka_run_group_tests(test_PPU_ManageTiming, setup_PPU, teardown_PPU);
	return out;
}
