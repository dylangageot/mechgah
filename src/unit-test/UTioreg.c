#include "UTest.h"
#include "../nes/mapper/ioreg.h"
#include "../nes/cpu/cpu.h"
#include "../nes/ppu/ppu.h"
#include "../nes/controller/controller.h"
#include <stdlib.h>

static int setup_IOReg(void **state) {
	*state = (void *) IOReg_Create();
	if (*state == NULL)
		return -1;
	return 0;
}

static void test_IOReg_Get(void **state) {
	IOReg *self = (IOReg*) *state;
	uint8_t *ptr = NULL, *ptrNull = NULL;
	uint16_t i = 0;

	assert_int_equal(IOReg_Get(NULL, 0, 0), NULL);
	assert_int_equal(IOReg_Get(self, 0, 0), NULL);
	for (i = 0; i < 8; i++) {
		self->bank1[i] = ptrNull + i;
	}
	for (i = 0; i < 32; i++) {
		self->bank2[i] = ptrNull + i;
	}

	/* Test CPU IO bank 1 memory space */
	for (i = 0x2000; i < 0x4000; i++) {
		ptr = IOReg_Get(self, AC_WR | AC_RD, i);
		assert_ptr_equal((void*) ptrNull + i % 8, ptr);
	}

	/* Test CPU IO bank 2 memory space */
	for (i = 0x4000; i < 0x4020; i++) {
		ptr = IOReg_Get(self, AC_WR | AC_RD, i);
		assert_ptr_equal((void*) ptrNull + (i & 0x0FF), ptr);
	}
}

static void test_IOReg_Ack_NoRead(void **state) {
	uint16_t i;
	assert_int_equal(IOReg_Ack(NULL, 0), 0);
	for (i = 0x2000; i < 0x4020; i++)
		assert_int_equal(0, IOReg_Ack((IOReg*) *state, i)); 
}

static void test_IOReg_Ack_IsRead(void **state) {
	uint16_t i;
	assert_int_equal(IOReg_Ack(NULL, 0), 0);
	for (i = 0x3FF8; i < 0x4020; i++)
		assert_int_equal(AC_RD | AC_WR, IOReg_Ack((IOReg*) *state, i)); 
}

static void test_IOReg_Connect(void **state) {
	IOReg *self = (IOReg*) *state;
	CPU *cpu = CPU_Create(NULL);
	PPU *ppu = PPU_Create(NULL);
	Controller *ctrl = Controller_Create(NULL);
	
	int i = 0;

	/* Verify that nothing is connected */
	for (i = 0; i < 8; i++) {
		assert_ptr_equal((void*) self->bank1[i], (void*) &self->dummy);
	}
	for (i = 0; i < 32; i++) {
		assert_ptr_equal((void*) self->bank2[i], (void*) &self->dummy);
	}

	assert_int_equal(IOReg_Connect(self, NULL, NULL, NULL), EXIT_FAILURE);
	assert_int_equal(IOReg_Connect(self, cpu, ppu, ctrl), EXIT_SUCCESS);

	/* Verify that connection happened */
	assert_ptr_equal((void*) self->bank1[PPUCTRL], (void*) &ppu->PPUCTRL);
	assert_ptr_equal((void*) self->bank1[PPUMASK], (void*) &ppu->PPUMASK);
	assert_ptr_equal((void*) self->bank1[PPUSTATUS], (void*) &ppu->PPUSTATUS);
	assert_ptr_equal((void*) self->bank1[OAMADDR], (void*) &ppu->OAMADDR);
	assert_ptr_equal((void*) self->bank1[OAMDATA], (void*) &ppu->OAMDATA);
	assert_ptr_equal((void*) self->bank1[PPUSCROLL], (void*) &ppu->PPUSCROLL);
	assert_ptr_equal((void*) self->bank1[PPUADDR], (void*) &ppu->PPUADDR);
	assert_ptr_equal((void*) self->bank1[PPUDATA], (void*) &ppu->PPUDATA);
	assert_ptr_equal((void*) self->bank2[OAMDMA], (void*) &cpu->OAMDMA);
	assert_ptr_equal((void*) self->bank2[JOY1], (void*) &ctrl->JOY1);
	assert_ptr_equal((void*) self->bank2[JOY2], (void*) &ctrl->JOY2);
	
	CPU_Destroy(cpu);
	PPU_Destroy(ppu);
	Controller_Destroy(ctrl);
}

static int teardown_IOReg(void **state) {
	if (*state != NULL) {
		IOReg_Destroy((IOReg*) *state);
		return 0;
	} else
		return -1;
}

int run_UTioreg(void) {
	const struct CMUnitTest test_IOReg[] = {
		cmocka_unit_test(test_IOReg_Ack_NoRead),
		cmocka_unit_test(test_IOReg_Get),
		cmocka_unit_test(test_IOReg_Ack_IsRead),
		cmocka_unit_test(test_IOReg_Ack_NoRead),
	};
	const struct CMUnitTest test_IOReg_Connection[] = {
		cmocka_unit_test(test_IOReg_Connect),
	};
	int out = 0;
	out += cmocka_run_group_tests(test_IOReg, setup_IOReg, teardown_IOReg);
	out += cmocka_run_group_tests(test_IOReg_Connection, setup_IOReg, teardown_IOReg);
	return out;
}
