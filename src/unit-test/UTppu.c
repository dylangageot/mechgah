#include "UTest.h"
#include "../nes/ppu/ppu.h"
#include "../nes/mapper/nrom.h"
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
	const struct CMUnitTest test_ppu[] = {
		cmocka_unit_test(test_PPU_CheckRegister_PPUCTRL),
		cmocka_unit_test(test_PPU_CheckRegister_PPUSTATUS),
		cmocka_unit_test(test_PPU_CheckRegister_OAMDATA),
		cmocka_unit_test(test_PPU_CheckRegister_PPUSCROLL),
		cmocka_unit_test(test_PPU_CheckRegister_PPUADDR),
		cmocka_unit_test(test_PPU_CheckRegister_PPUDATA),
	};
    int out = 0;
    out += cmocka_run_group_tests(test_ppu, setup_PPU, teardown_PPU);
    return out;
}
